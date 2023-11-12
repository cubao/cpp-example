#pragma once

#include "../expression.hpp"
#include "../parsing_context.hpp"

#include <memory>

namespace mbgl
{
namespace style
{
namespace expression
{

ParseResult parseComparison(const Convertible &,
                            ParsingContext &);

class BasicComparison : public Expression
{
  public:
    using CompareFunctionType = bool (*)(const Value &, const Value &);

    BasicComparison(std::string op, std::unique_ptr<Expression> lhs,
                    std::unique_ptr<Expression> rhs);

    void eachChild(
        const std::function<void(const Expression &)> &visit) const override;
    bool operator==(const Expression &) const override;
    EvaluationResult evaluate(const EvaluationContext &) const override;
    std::vector<optional<Value>> possibleOutputs() const override;
    std::string getOperator() const override;

  private:
    std::string op;
    CompareFunctionType compare;
    std::unique_ptr<Expression> lhs;
    std::unique_ptr<Expression> rhs;
    bool needsRuntimeTypeCheck;
};

} // namespace expression
} // namespace style
} // namespace mbgl
