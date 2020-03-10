/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef DATATYPES_PATHOPS_H_
#define DATATYPES_PATHOPS_H_

#include "base/Base.h"

#include <thrift/lib/cpp2/GeneratedSerializationCodeHelper.h>
#include <thrift/lib/cpp2/gen/module_types_tcc.h>
#include <thrift/lib/cpp2/protocol/ProtocolReaderStructReadState.h>

#include "datatypes/Path.h"

namespace apache {
namespace thrift {

/**************************************
 *
 * Ops for class Step
 *
 *************************************/
namespace detail {

template<>
struct TccStructTraits<nebula::Step> {
    static void translateFieldName(
            FOLLY_MAYBE_UNUSED folly::StringPiece _fname,
            FOLLY_MAYBE_UNUSED int16_t& fid,
            FOLLY_MAYBE_UNUSED apache::thrift::protocol::TType& _ftype) {
        if (_fname == "dst") {
            fid = 1;
            _ftype = apache::thrift::protocol::T_STRUCT;
        } else if (_fname == "type") {
            fid = 2;
            _ftype = apache::thrift::protocol::T_I32;
        } else if (_fname == "ranking") {
            fid = 3;
            _ftype = apache::thrift::protocol::T_I64;
        } else if (_fname == "props") {
            fid = 4;
            _ftype = apache::thrift::protocol::T_MAP;
        }
    }
};

}  // namespace detail


template<>
inline void Cpp2Ops<nebula::Step>::clear(nebula::Step* obj) {
    return obj->clear();
}


template<>
inline constexpr protocol::TType Cpp2Ops<nebula::Step>::thriftType() {
    return apache::thrift::protocol::T_STRUCT;
}


template<>
template<class Protocol>
uint32_t Cpp2Ops<nebula::Step>::write(Protocol* proto, nebula::Step const* obj) {
    uint32_t xfer = 0;
    xfer += proto->writeStructBegin("Step");

    xfer += proto->writeFieldBegin("dst", apache::thrift::protocol::T_STRUCT, 1);
    xfer += Cpp2Ops<nebula::Vertex>::write(proto, &obj->dst);
    xfer += proto->writeFieldEnd();

    xfer += proto->writeFieldBegin("type", apache::thrift::protocol::T_I32, 2);
    xfer += detail::pm::protocol_methods<type_class::integral, nebula::EdgeType>
        ::write(*proto, obj->type);
    xfer += proto->writeFieldEnd();

    xfer += proto->writeFieldBegin("ranking", apache::thrift::protocol::T_I64, 3);
    xfer += detail::pm::protocol_methods<type_class::integral, nebula::EdgeRanking>
        ::write(*proto, obj->ranking);
    xfer += proto->writeFieldEnd();

    xfer += proto->writeFieldBegin("props", apache::thrift::protocol::T_MAP, 4);
    xfer += detail::pm::protocol_methods<
            type_class::map<type_class::binary, type_class::structure>,
            std::unordered_map<std::string, nebula::Value>
        >::write(*proto, obj->props);
    xfer += proto->writeFieldEnd();

    xfer += proto->writeFieldStop();
    xfer += proto->writeStructEnd();
    return xfer;
}


template<>
template<class Protocol>
void Cpp2Ops<nebula::Step>::read(Protocol* proto, nebula::Step* obj) {
    detail::ProtocolReaderStructReadState<Protocol> readState;

    readState.readStructBegin(proto);

    using apache::thrift::TProtocolException;


    if (UNLIKELY(!readState.advanceToNextField(proto, 0, 1, protocol::T_STRUCT))) {
        goto _loop;
    }

_readField_dst:
    {
        Cpp2Ops<nebula::Vertex>::read(proto, &obj->dst);
    }

    if (UNLIKELY(!readState.advanceToNextField(proto, 1, 2, protocol::T_I32))) {
        goto _loop;
    }

_readField_type:
    {
        detail::pm::protocol_methods<type_class::integral, nebula::EdgeType>
            ::read(*proto, obj->type);
    }

    if (UNLIKELY(!readState.advanceToNextField(proto, 2, 3, protocol::T_I64))) {
        goto _loop;
    }

_readField_ranking:
    {
        detail::pm::protocol_methods<type_class::integral, nebula::EdgeRanking>
            ::read(*proto, obj->ranking);
    }

    if (UNLIKELY(!readState.advanceToNextField(proto, 3, 4, protocol::T_MAP))) {
        goto _loop;
    }

_readField_props:
    {
        obj->props = std::unordered_map<std::string, nebula::Value>();
        detail::pm::protocol_methods<
                type_class::map<type_class::binary, type_class::structure>,
                std::unordered_map<std::string, nebula::Value>
            >::read(*proto, obj->props);
    }

    if (UNLIKELY(!readState.advanceToNextField(proto, 4, 0, protocol::T_STOP))) {
        goto _loop;
    }

_end:
    readState.readStructEnd(proto);

    return;

_loop:
    if (readState.fieldType == apache::thrift::protocol::T_STOP) {
        goto _end;
    }

    if (proto->kUsesFieldNames()) {
        detail::TccStructTraits<nebula::Step>::translateFieldName(readState.fieldName(),
                                                                  readState.fieldId,
                                                                  readState.fieldType);
    }

    switch (readState.fieldId) {
        case 1:
        {
            if (LIKELY(readState.fieldType == apache::thrift::protocol::T_STRUCT)) {
                goto _readField_dst;
            } else {
                goto _skip;
            }
        }
        case 2:
        {
            if (LIKELY(readState.fieldType == apache::thrift::protocol::T_I32)) {
                goto _readField_type;
            } else {
                goto _skip;
            }
        }
        case 3:
        {
            if (LIKELY(readState.fieldType == apache::thrift::protocol::T_I64)) {
                goto _readField_ranking;
            } else {
                goto _skip;
            }
        }
        case 4:
        {
            if (LIKELY(readState.fieldType == apache::thrift::protocol::T_MAP)) {
                goto _readField_props;
            } else {
                goto _skip;
            }
        }
        default:
        {
_skip:
          proto->skip(readState.fieldType);
          readState.readFieldEnd(proto);
          readState.readFieldBeginNoInline(proto);
          goto _loop;
        }
    }
}


template<>
template<class Protocol>
uint32_t Cpp2Ops<nebula::Step>::serializedSize(Protocol const* proto,
                                               nebula::Step const* obj) {
    uint32_t xfer = 0;
    xfer += proto->serializedStructSize("Step");

    xfer += proto->serializedFieldSize("dst", apache::thrift::protocol::T_STRUCT, 1);
    xfer += Cpp2Ops<nebula::Vertex>::serializedSize(proto, &obj->dst);

    xfer += proto->serializedFieldSize("type", apache::thrift::protocol::T_I32, 2);
    xfer += detail::pm::protocol_methods<type_class::integral, nebula::EdgeType>
        ::serializedSize<false>(*proto, obj->type);

    xfer += proto->serializedFieldSize("ranking", apache::thrift::protocol::T_I64, 3);
    xfer += detail::pm::protocol_methods<type_class::integral, nebula::EdgeRanking>
        ::serializedSize<false>(*proto, obj->ranking);

    xfer += proto->serializedFieldSize("props", apache::thrift::protocol::T_MAP, 4);
    xfer += detail::pm::protocol_methods<
            type_class::map<type_class::binary, type_class::structure>,
            std::unordered_map<std::string, nebula::Value>
        >::serializedSize<false>(*proto, obj->props);

    xfer += proto->serializedSizeStop();
    return xfer;
}


template<>
template<class Protocol>
uint32_t Cpp2Ops<nebula::Step>::serializedSizeZC(Protocol const* proto,
                                                 nebula::Step const* obj) {
    uint32_t xfer = 0;
    xfer += proto->serializedStructSize("Step");

    xfer += proto->serializedFieldSize("dst", apache::thrift::protocol::T_STRUCT, 1);
    xfer += Cpp2Ops<nebula::Vertex>::serializedSizeZC(proto, &obj->dst);

    xfer += proto->serializedFieldSize("type", apache::thrift::protocol::T_I32, 2);
    xfer += detail::pm::protocol_methods<type_class::integral, nebula::EdgeType>
        ::serializedSize<false>(*proto, obj->type);

    xfer += proto->serializedFieldSize("ranking", apache::thrift::protocol::T_I64, 3);
    xfer += detail::pm::protocol_methods<type_class::integral, nebula::EdgeRanking>
        ::serializedSize<false>(*proto, obj->ranking);

    xfer += proto->serializedFieldSize("props", apache::thrift::protocol::T_MAP, 4);
    xfer += detail::pm::protocol_methods<
            type_class::map<type_class::binary, type_class::structure>,
            std::unordered_map<std::string, nebula::Value>
        >::serializedSize<false>(*proto, obj->props);

    xfer += proto->serializedSizeStop();
    return xfer;
}


/**************************************
 *
 * Ops for class Path
 *
 *************************************/
namespace detail {

template<>
struct TccStructTraits<nebula::Path> {
    static void translateFieldName(
            FOLLY_MAYBE_UNUSED folly::StringPiece _fname,
            FOLLY_MAYBE_UNUSED int16_t& fid,
            FOLLY_MAYBE_UNUSED apache::thrift::protocol::TType& _ftype) {
        if (_fname == "src") {
            fid = 1;
            _ftype = apache::thrift::protocol::T_STRUCT;
        } else if (_fname == "steps") {
            fid = 2;
            _ftype = apache::thrift::protocol::T_LIST;
        }
    }
};

}  // namespace detail


template<>
inline void Cpp2Ops<nebula::Path>::clear(nebula::Path* obj) {
    return obj->clear();
}


template<>
inline constexpr protocol::TType Cpp2Ops<nebula::Path>::thriftType() {
    return apache::thrift::protocol::T_STRUCT;
}


template<>
template<class Protocol>
uint32_t Cpp2Ops<nebula::Path>::write(Protocol* proto, nebula::Path const* obj) {
    uint32_t xfer = 0;
    xfer += proto->writeStructBegin("Path");

    xfer += proto->writeFieldBegin("src", apache::thrift::protocol::T_STRUCT, 1);
    xfer += Cpp2Ops< nebula::Vertex>::write(proto, &obj->src);
    xfer += proto->writeFieldEnd();

    xfer += proto->writeFieldBegin("steps", apache::thrift::protocol::T_LIST, 2);
    xfer += detail::pm::protocol_methods<
            type_class::list<type_class::structure>,
            std::vector<nebula::Step>
        >::write(*proto, obj->steps);
    xfer += proto->writeFieldEnd();

    xfer += proto->writeFieldStop();
    xfer += proto->writeStructEnd();
    return xfer;
}


template<>
template<class Protocol>
void Cpp2Ops<nebula::Path>::read(Protocol* proto, nebula::Path* obj) {
    detail::ProtocolReaderStructReadState<Protocol> readState;

    readState.readStructBegin(proto);

    using apache::thrift::TProtocolException;

    if (UNLIKELY(!readState.advanceToNextField(proto, 0, 1, protocol::T_STRUCT))) {
        goto _loop;
    }

_readField_src:
    {
        Cpp2Ops<nebula::Vertex>::read(proto, &obj->src);
    }

    if (UNLIKELY(!readState.advanceToNextField(proto, 1, 2, protocol::T_LIST))) {
        goto _loop;
    }

_readField_steps:
    {
        obj->steps = std::vector<nebula::Step>();
        detail::pm::protocol_methods<
                type_class::list<type_class::structure>,
                std::vector<nebula::Step>
            >::read(*proto, obj->steps);
    }

    if (UNLIKELY(!readState.advanceToNextField(proto, 2, 0, protocol::T_STOP))) {
        goto _loop;
    }

_end:
    readState.readStructEnd(proto);

    return;

_loop:
    if (readState.fieldType == apache::thrift::protocol::T_STOP) {
        goto _end;
    }

    if (proto->kUsesFieldNames()) {
        detail::TccStructTraits<nebula::Path>::translateFieldName(readState.fieldName(),
                                                                  readState.fieldId,
                                                                  readState.fieldType);
    }

    switch (readState.fieldId) {
        case 1:
        {
            if (LIKELY(readState.fieldType == apache::thrift::protocol::T_STRUCT)) {
                goto _readField_src;
            } else {
                goto _skip;
            }
        }
        case 2:
        {
            if (LIKELY(readState.fieldType == apache::thrift::protocol::T_LIST)) {
                goto _readField_steps;
            } else {
                goto _skip;
            }
        }
        default:
        {
_skip:
            proto->skip(readState.fieldType);
            readState.readFieldEnd(proto);
            readState.readFieldBeginNoInline(proto);
            goto _loop;
        }
    }
}


template<>
template<class Protocol>
uint32_t Cpp2Ops<nebula::Path>::serializedSize(Protocol const* proto,
                                               nebula::Path const* obj) {
    uint32_t xfer = 0;
    xfer += proto->serializedStructSize("Path");

    xfer += proto->serializedFieldSize("src", apache::thrift::protocol::T_STRUCT, 1);
    xfer += Cpp2Ops<nebula::Vertex>::serializedSize(proto, &obj->src);

    xfer += proto->serializedFieldSize("steps", apache::thrift::protocol::T_LIST, 2);
    xfer += detail::pm::protocol_methods<
            type_class::list<type_class::structure>,
            std::vector<nebula::Step>
        >::serializedSize<false>(*proto, obj->steps);

    xfer += proto->serializedSizeStop();
    return xfer;
}


template<>
template<class Protocol>
uint32_t Cpp2Ops<nebula::Path>::serializedSizeZC(Protocol const* proto,
                                                 nebula::Path const* obj) {
    uint32_t xfer = 0;
    xfer += proto->serializedStructSize("Path");

    xfer += proto->serializedFieldSize("src", apache::thrift::protocol::T_STRUCT, 1);
    xfer += Cpp2Ops<nebula::Vertex>::serializedSizeZC(proto, &obj->src);

    xfer += proto->serializedFieldSize("steps", apache::thrift::protocol::T_LIST, 2);
    xfer += detail::pm::protocol_methods<
            type_class::list<type_class::structure>,
            std::vector<nebula::Step>
        >::serializedSize<false>(*proto, obj->steps);

    xfer += proto->serializedSizeStop();
    return xfer;
}

}  // namespace thrift
}  // namespace apache
#endif  // DATATYPES_PATHOPS_H_

