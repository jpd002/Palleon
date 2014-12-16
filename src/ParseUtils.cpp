#include "string_format.h"
#include "palleon/ParseUtils.h"
#include "math/MathStringUtils.h"
#include <vector>
#include <sstream>
#include <boost/algorithm/string.hpp>

using namespace Palleon;

CColor Palleon::ParseColor(const std::string& vectorString)
{
	std::vector<std::string> components;
	boost::split(components, vectorString, boost::is_any_of(", "), boost::algorithm::token_compress_on);
	CColor result(0, 0, 0, 0);
	if(components.size() != 4) return result;
	result.r = MathStringUtils::ParseFloat(components[0]);
	result.g = MathStringUtils::ParseFloat(components[1]);
	result.b = MathStringUtils::ParseFloat(components[2]);
	result.a = MathStringUtils::ParseFloat(components[3]);
	return result;
}
