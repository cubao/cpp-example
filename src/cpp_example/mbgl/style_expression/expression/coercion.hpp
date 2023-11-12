#pragma once

#include "../expression.hpp"
#include <mbgl/style/conversion.hpp>

#include <memory>
#include <vector>

namespace mbgl
{
namespace style
{
namespace expression
{

class Coercion : public Expression
{
  public:
    Coercion(type::Type type_,
             std::vector<std::unique_ptr<Expression>> inputs_);

    static ParseResult parse(const Convertible &value,
                             ParsingContext &ctx);

    EvaluationResult evaluate(const EvaluationContext &params) const override;
    void eachChild(
        const std::function<void(const Expression &)> &visit) const override;

    bool operator==(const Expression &e) const override;

    std::vector<optional<Value>> possibleOutputs() const override;

    std::string getOperator() const override;

  private:
    EvaluationResult (*coerceSingleValue)(const Value &v);
    std::vector<std::unique_ptr<Expression>> inputs;
};

} // namespace expression
} // namespace style
} // namespace mbgl
