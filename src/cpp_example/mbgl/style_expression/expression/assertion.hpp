#pragma once

#include "../expression.hpp"
#include "../parsing_context.hpp"

#include <memory>
#include <vector>

namespace mbgl
{
namespace style
{
namespace expression
{

class Assertion : public Expression
{
  public:
    Assertion(type::Type type_,
              std::vector<std::unique_ptr<Expression>> inputs_);

    static ParseResult parse(const mbgl::style::conversion::Convertible &value,
                             ParsingContext &ctx);

    EvaluationResult evaluate(const EvaluationContext &params) const override;
    void eachChild(
        const std::function<void(const Expression &)> &visit) const override;

    bool operator==(const Expression &e) const override;

    std::vector<optional<Value>> possibleOutputs() const override;

    mbgl::Value serialize() const override;
    std::string getOperator() const override;

  private:
    std::vector<std::unique_ptr<Expression>> inputs;
};

} // namespace expression
} // namespace style
} // namespace mbgl