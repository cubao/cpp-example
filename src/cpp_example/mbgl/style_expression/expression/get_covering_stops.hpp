#pragma once

#include "../expression.hpp"
#include "../range.hpp"
#include <memory>
#include <map>

namespace mbgl
{
namespace style
{
namespace expression
{

// Return the smallest range of stops that covers the interval [lower, upper]
Range<float>
getCoveringStops(const std::map<double, std::unique_ptr<Expression>> &stops,
                 double lower, double upper);

} // namespace expression
} // namespace style
} // namespace mbgl
