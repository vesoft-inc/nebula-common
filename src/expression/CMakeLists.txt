nebula_add_library(
    expression_obj OBJECT
    ConstantExpression.cpp
    ArithmeticExpression.cpp
    UnaryExpression.cpp
    RelationalExpression.cpp
    LogicalExpression.cpp
    TypeCastingExpression.cpp
)

#nebula_add_subdirectory(test)


