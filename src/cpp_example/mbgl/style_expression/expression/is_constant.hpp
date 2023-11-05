#pragma once

#include "../expression.hpp"
#include <mbgl/style/expression/compound_expression.hpp>

namespace mbgl
{
namespace style
{
namespace expression
{

template <typename T>
bool isGlobalPropertyConstant(const Expression &expression, const T &properties)
{
    if (expression.getKind() == Kind::CompoundExpression) {
        auto e = static_cast<const CompoundExpression *>(&expression);
        for (const std::string &property : properties) {
            if (e->getOperator() == property) {
                return false;
            }
        }
    }

    bool isConstant = true;
    expression.eachChild([&](const Expression &e) {
        if (isConstant && !isGlobalPropertyConstant(e, properties)) {
            isConstant = false;
        }
    });
    return isConstant;
}

bool isFeatureConstant(const Expression &expression);
} // namespace expression
} // namespace style
} // namespace mbgl
