/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSes_ directory.
 */

#ifndef JIT_NEBULAJIT_H
#define JIT_NEBULAJIT_H

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/Orc/Legacy.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/RTDyldMemoryManager.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Mangler.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace nebula {

class NebulaJIT {
public:
    using ObjLayerT = llvm::orc::LegacyRTDyldObjectLinkingLayer;
    using CompileLayerT = llvm::orc::LegacyIRCompileLayer<ObjLayerT, llvm::orc::SimpleCompiler>;

    /**
     * Init should be called only once after the process starting.
     * */
    static void init() {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
        llvm::InitializeNativeTargetAsmParser();
    }

    NebulaJIT()
        : resolver_(llvm::orc::createLegacyLookupResolver(
                    es_,
                    [this](const std::string& name) {
                        return findMangledSymbol(name);
                    },
                    [](llvm::Error err) {
                        llvm::cantFail(std::move(err), "lookupFlags failed");
                    }))
        , tm_(llvm::EngineBuilder().selectTarget())
        , dl_(tm_->createDataLayout())
        , objectLayer_(llvm::AcknowledgeORCv1Deprecation,
                       es_,
                       [this](llvm::orc::VModuleKey) {
                           return ObjLayerT::Resources {
                               std::make_shared<llvm::SectionMemoryManager>(), resolver_};
                       })
        , compileLayer_(llvm::AcknowledgeORCv1Deprecation,
                        objectLayer_,
                        llvm::orc::SimpleCompiler(*tm_)) {
        llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
    }

    llvm::TargetMachine& getTargetMachine() {
        return *tm_;
    }

    llvm::orc::VModuleKey addModule(std::unique_ptr<llvm::Module> M) {
        auto K = es_.allocateVModule();
        llvm::cantFail(compileLayer_.addModule(K, std::move(M)));
        moduleKeys_.push_back(K);
        return K;
    }

    void removeModule(llvm::orc::VModuleKey K) {
        auto it = std::find(moduleKeys_.begin(), moduleKeys_.end(), K);
        if (it == moduleKeys_.end()) {
            fprintf(stderr, "Can't find the module k \n");
            return;
        }
        moduleKeys_.erase(it);
        llvm::cantFail(compileLayer_.removeModule(K));
    }

    llvm::JITSymbol findSymbol(const std::string& name) {
        return findMangledSymbol(mangle(name));
    }

    std::string mangle(const std::string& name) {
        std::string mangledName;
        llvm::raw_string_ostream mangledNameStream(mangledName);
        llvm::Mangler::getNameWithPrefix(mangledNameStream, name, dl_);
        VLOG(1) << "Mangle function name " << name << " to " << mangledName;
        return mangledName;
    }

private:
    llvm::JITSymbol findMangledSymbol(const std::string& name) {
        // Search modules in reverse order: from last added to first added.
        // This is the opposite of the usual search order for dlsym, but makes more
        // sense in a REPL where we want to bind to the newest available definition.
        for (auto H : llvm::make_range(moduleKeys_.rbegin(), moduleKeys_.rend())) {
            if (auto sym = compileLayer_.findSymbolIn(H, name, true)) {
                return sym;
            }
        }

        // If we can't find the symbol in the JIT, try looking in the host process.
        if (auto symAddr = llvm::RTDyldMemoryManager::getSymbolAddressInProcess(name)) {
            return llvm::JITSymbol(symAddr, llvm::JITSymbolFlags::Exported);
        }
        return nullptr;
    }

    llvm::orc::ExecutionSession es_;
    std::shared_ptr<llvm::orc::SymbolResolver> resolver_;
    std::unique_ptr<llvm::TargetMachine> tm_;
    const llvm::DataLayout dl_;
    ObjLayerT objectLayer_;
    CompileLayerT compileLayer_;
    std::vector<llvm::orc::VModuleKey> moduleKeys_;
};

}  // end namespace nebula

#endif  // JIT_NEBULAJIT_H
