/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include <folly/Benchmark.h>
#include "expression/test/ExpressionContextMock.h"
#include "expression/ArithmeticExpression.h"
#include "expression/ConstantExpression.h"
#include "expression/SymbolPropertyExpression.h"
#include "expression/RelationalExpression.h"

nebula::ExpressionContextMock gExpCtxt;

namespace nebula {
size_t add2Constant(size_t iters) {
    constexpr size_t ops = 1000000UL;
    Value eval;
    for (size_t i = 0; i < iters * ops; ++i) {
        ArithmeticExpression add(
                Expression::Type::EXP_ADD, new ConstantExpression(1), new ConstantExpression(2));
        eval = add.eval();
    }
    folly::doNotOptimizeAway(eval);
    return iters * ops;
}

size_t add3Constant(size_t iters) {
    constexpr size_t ops = 1000000UL;
    Value eval;
    for (size_t i = 0; i < iters * ops; ++i) {
        ArithmeticExpression add(
                Expression::Type::EXP_ADD,
                new ArithmeticExpression(
                    Expression::Type::EXP_ADD,
                    new ConstantExpression(1), new ConstantExpression(2)),
                new ConstantExpression(3));
        eval = add.eval();
    }
    folly::doNotOptimizeAway(eval);
    return iters * ops;
}

size_t add2Constant1EdgeProp(size_t iters) {
    constexpr size_t ops = 1000000UL;
    Value eval;
    for (size_t i = 0; i < iters * ops; ++i) {
        ArithmeticExpression add(
                Expression::Type::EXP_ADD,
                new ArithmeticExpression(
                    Expression::Type::EXP_ADD,
                    new ConstantExpression(1), new ConstantExpression(2)),
                new EdgePropertyExpression(new std::string("e1"), new std::string("int")));
        add.setExpCtxt(&gExpCtxt);
        eval = add.eval();
    }
    folly::doNotOptimizeAway(eval);
    return iters * ops;
}

size_t concat2String(size_t iters) {
    constexpr size_t ops = 1000000UL;
    Value eval;
    for (size_t i = 0; i < iters * ops; ++i) {
        ArithmeticExpression add(
                Expression::Type::EXP_ADD,
                new EdgePropertyExpression(new std::string("e1"), new std::string("string16")),
                new EdgePropertyExpression(new std::string("e1"), new std::string("string16")));
        add.setExpCtxt(&gExpCtxt);
        eval = add.eval();
    }
    folly::doNotOptimizeAway(eval);
    return iters * ops;
}

size_t inList(size_t iters) {
    constexpr size_t ops = 1000000UL;
    Value eval;
    for (size_t i = 0; i < iters * ops; ++i) {
        RelationalExpression expr(
                Expression::Type::EXP_REL_IN,
                new ConstantExpression("aaaa"),
                new EdgePropertyExpression(new std::string("e1"), new std::string("list")));
        expr.setExpCtxt(&gExpCtxt);
        eval = expr.eval();
    }
    folly::doNotOptimizeAway(eval);
    return iters * ops;
}

size_t isNull(size_t iters, const char* var) {
    constexpr size_t ops = 1000000UL;
    Value eval;
    for (size_t i = 0; i < iters * ops; ++i) {
        RelationalExpression expr(
                Expression::Type::EXP_REL_EQ,
                new EdgePropertyExpression(new std::string("e1"), new std::string(var)),
                new ConstantExpression(Value(NullType::NaN)));
        expr.setExpCtxt(&gExpCtxt);
        eval = expr.eval();
    }
    folly::doNotOptimizeAway(eval);
    return iters * ops;
}

size_t isListEq(size_t iters, const char* var) {
    constexpr size_t ops = 1000000UL;
    Value eval;
    for (size_t i = 0; i < iters * ops; ++i) {
        RelationalExpression expr(
                Expression::Type::EXP_REL_EQ,
                new EdgePropertyExpression(new std::string("e1"), new std::string(var)),
                new EdgePropertyExpression(new std::string("e1"), new std::string(var)));
        expr.setExpCtxt(&gExpCtxt);
        eval = expr.eval();
    }
    folly::doNotOptimizeAway(eval);
    return iters * ops;
}

// TODO(cpw): more test cases.

BENCHMARK_NAMED_PARAM_MULTI(add2Constant, 1_add_2)
BENCHMARK_NAMED_PARAM_MULTI(add3Constant, 1_add_2_add_3)
BENCHMARK_NAMED_PARAM_MULTI(add2Constant1EdgeProp, 1_add_2_add_e1_int)
BENCHMARK_NAMED_PARAM_MULTI(concat2String, concat_string_string)
BENCHMARK_NAMED_PARAM_MULTI(inList, in_list)
BENCHMARK_NAMED_PARAM_MULTI(isNull, is_list_eq_null, "list")
BENCHMARK_NAMED_PARAM_MULTI(isNull, is_listoflist_eq_Null, "list_of_list")
BENCHMARK_NAMED_PARAM_MULTI(isListEq, is_list_eq_list, "list")
BENCHMARK_NAMED_PARAM_MULTI(isListEq, is_listoflist_eq_listoflist, "list_of_list")
}  // namespace nebula

int main(int argc, char** argv) {
    folly::init(&argc, &argv, true);
    folly::runBenchmarks();
    return 0;
}

/*
Intel(R) Xeon(R) CPU E5-2690 v2 @ 3.00GHz
============================================================================
ExpressionBenchmark.cpprelative                           time/iter  iters/s
============================================================================
add2Constant(1_add_2)                                       95.24ns   10.50M
add3Constant(1_add_2_add_3)                                165.98ns    6.02M
add2Constant1EdgeProp(1_add_2_add_e1_int)                  276.85ns    3.61M
concat2String(concat_string_string)                        404.72ns    2.47M
inList(in_list)                                            534.80ns    1.87M
isNull(is_list_eq_null)                                    502.46ns    1.99M
isNull(is_listoflist_eq_Null)                                5.04us  198.40K
isListEq(is_list_eq_list)                                    1.08us  929.97K
isListEq(is_listoflist_eq_listoflist)                       13.34us   74.98K
============================================================================
*/
