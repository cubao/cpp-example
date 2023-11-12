#pragma once

#include "../expression.hpp"

#include <memory>

namespace mbgl
{
namespace style
{
namespace expression
{

class Any : public Expression
{
  public:
    Any(std::vector<std::unique_ptr<Expression>> inputs_)
        : Expression(Kind::Any, type::Boolean), inputs(std::move(inputs_))
    {
    }

    static ParseResult parse(const Convertible &value,
                             ParsingContext &ctx);

    EvaluationResult evaluate(const EvaluationContext &params) const override;
    void eachChild(
        const std::function<void(const Expression &)> &visit) const override;
    bool operator==(const Expression &e) const override;
    std::vector<optional<Value>> possibleOutputs() const override;

    std::string getOperator() const override { return "any"; }

  private:
    std::vector<std::unique_ptr<Expression>> inputs;
};

class All : public Expression
{
  public:
    All(std::vector<std::unique_ptr<Expression>> inputs_)
        : Expression(Kind::All, type::Boolean), inputs(std::move(inputs_))
    {
    }

    static ParseResult parse(const Convertible &value,
                             ParsingContext &ctx);

    EvaluationResult evaluate(const EvaluationContext &params) const override;
    void eachChild(
        const std::function<void(const Expression &)> &visit) const override;
    bool operator==(const Expression &e) const override;
    std::vector<optional<Value>> possibleOutputs() const override;

    std::string getOperator() const override { return "all"; }

  private:
    std::vector<std::unique_ptr<Expression>> inputs;
};

} // namespace expression
} // namespace style
} // namespace mbgl
