#include <mbgl/style/expression/collator.hpp>
#include <mbgl/style/expression/comparison.hpp>
#include <mbgl/style/expression/dsl.hpp>
#include <utility>

namespace mbgl
{
namespace style
{
namespace expression
{

static bool isComparableType(const std::string &op, const type::Type &type)
{
    if (op == "==" || op == "!=") {
        return type == type::String || type == type::Number ||
               type == type::Boolean || type == type::Null ||
               type == type::Value;
    } else {
        return type == type::String || type == type::Number ||
               type == type::Value;
    }
}

bool eq(const Value &a, const Value &b) { return a == b; }
bool neq(const Value &a, const Value &b) { return a != b; }
bool lt(const Value &lhs, const Value &rhs)
{
    return lhs.match(
        [&](const std::string &a) { return a < rhs.get<std::string>(); },
        [&](double a) { return a < rhs.get<double>(); },
        [&](const auto &) {
            assert(false);
            return false;
        });
}
bool gt(const Value &lhs, const Value &rhs)
{
    return lhs.match(
        [&](const std::string &a) { return a > rhs.get<std::string>(); },
        [&](double a) { return a > rhs.get<double>(); },
        [&](const auto &) {
            assert(false);
            return false;
        });
}
bool lteq(const Value &lhs, const Value &rhs)
{
    return lhs.match(
        [&](const std::string &a) { return a <= rhs.get<std::string>(); },
        [&](double a) { return a <= rhs.get<double>(); },
        [&](const auto &) {
            assert(false);
            return false;
        });
}
bool gteq(const Value &lhs, const Value &rhs)
{
    return lhs.match(
        [&](const std::string &a) { return a >= rhs.get<std::string>(); },
        [&](double a) { return a >= rhs.get<double>(); },
        [&](const auto &) {
            assert(false);
            return false;
        });
}

static BasicComparison::CompareFunctionType
getBasicCompareFunction(const std::string &op)
{
    if (op == "==")
        return eq;
    else if (op == "!=")
        return neq;
    else if (op == ">")
        return gt;
    else if (op == "<")
        return lt;
    else if (op == ">=")
        return gteq;
    else if (op == "<=")
        return lteq;
    assert(false);
    return nullptr;
}

BasicComparison::BasicComparison(std::string op_,
                                 std::unique_ptr<Expression> lhs_,
                                 std::unique_ptr<Expression> rhs_)
    : Expression(Kind::Comparison, type::Boolean), op(std::move(op_)),
      compare(getBasicCompareFunction(op)), lhs(std::move(lhs_)),
      rhs(std::move(rhs_))
{
    assert(isComparableType(op, lhs->getType()) &&
           isComparableType(op, rhs->getType()));
    assert(lhs->getType() == rhs->getType() || lhs->getType() == type::Value ||
           rhs->getType() == type::Value);

    needsRuntimeTypeCheck =
        (op != "==" && op != "!=") &&
        (lhs->getType() == type::Value || rhs->getType() == type::Value);
}

EvaluationResult
BasicComparison::evaluate(const EvaluationContext &params) const
{
    EvaluationResult lhsResult = lhs->evaluate(params);
    if (!lhsResult)
        return lhsResult;

    EvaluationResult rhsResult = rhs->evaluate(params);
    if (!rhsResult)
        return rhsResult;

    if (needsRuntimeTypeCheck) {
        type::Type lhsType = typeOf(*lhsResult);
        type::Type rhsType = typeOf(*rhsResult);
        // check that type is string or number, and equal
        if (lhsType != rhsType ||
            !(lhsType == type::String || lhsType == type::Number)) {
            return EvaluationError{
                R"(Expected arguments for ")" + op + R"(")" +
                " to be (string, string) or (number, number), but found (" +
                toString(lhsType) + ", " + toString(rhsType) + ") instead."};
        }
    }

    return compare(*lhsResult, *rhsResult);
}

void BasicComparison::eachChild(
    const std::function<void(const Expression &)> &visit) const
{
    visit(*lhs);
    visit(*rhs);
}

std::string BasicComparison::getOperator() const { return op; }

bool BasicComparison::operator==(const Expression &e) const
{
    if (e.getKind() == Kind::Comparison) {
        auto comp = static_cast<const BasicComparison *>(&e);
        return comp->op == op && *comp->lhs == *lhs && *comp->rhs == *rhs;
    }
    return false;
}

std::vector<optional<Value>> BasicComparison::possibleOutputs() const
{
    return {{true}, {false}};
}

using namespace mbgl::style::conversion;
ParseResult parseComparison(const Convertible &value, ParsingContext &ctx)
{
    std::size_t length = arrayLength(value);

    if (length != 3 && length != 4) {
        ctx.error("Expected two or three arguments.");
        return ParseResult();
    }

    std::string op = *toString(arrayMember(value, 0));

    assert(getBasicCompareFunction(op));

    ParseResult lhs = ctx.parse(arrayMember(value, 1), 1, {type::Value});
    if (!lhs)
        return ParseResult();
    type::Type lhsType = (*lhs)->getType();
    if (!isComparableType(op, lhsType)) {
        ctx.error(R"(")" + op +
                      R"(" comparisons are not supported for type ')" +
                      toString(lhsType) + "'.",
                  1);
        return ParseResult();
    }

    ParseResult rhs = ctx.parse(arrayMember(value, 2), 2, {type::Value});
    if (!rhs)
        return ParseResult();
    type::Type rhsType = (*rhs)->getType();
    if (!isComparableType(op, rhsType)) {
        ctx.error(R"(")" + op +
                      R"(" comparisons are not supported for type ')" +
                      toString(rhsType) + "'.",
                  2);
        return ParseResult();
    }

    if (lhsType != rhsType && lhsType != type::Value &&
        rhsType != type::Value) {
        ctx.error("Cannot compare types '" + toString(lhsType) + "' and '" +
                  toString(rhsType) + "'.");
        return ParseResult();
    }

    if (op != "==" && op != "!=") {
        // typing rules specific to less/greater than operators
        if (lhsType == type::Value && rhsType != type::Value) {
            // (value, T)
            lhs = dsl::assertion(rhsType, std::move(*lhs));
        } else if (lhsType != type::Value && rhsType == type::Value) {
            // (T, value)
            rhs = dsl::assertion(lhsType, std::move(*rhs));
        }
    }

    return ParseResult(std::make_unique<BasicComparison>(op, std::move(*lhs),
                                                         std::move(*rhs)));
}

} // namespace expression
} // namespace style
} // namespace mbgl
