#include "palleon/ParseUtils.h"
#include <vector>
#include <sstream>
#include <boost/algorithm/string.hpp>

using namespace Palleon;

float Palleon::ParseFloat(const std::string& valueString)
{
	float result = 0;
	std::stringstream ss;
	ss.imbue(std::locale::classic());
	ss << valueString;
	ss >> result;
	return result;
}

CVector2 Palleon::ParseVector2(const std::string& vectorString)
{
	std::vector<std::string> components;
	boost::split(components, vectorString, boost::is_any_of(", "), boost::algorithm::token_compress_on);
	CVector2 result(0, 0);
	if(components.size() != 2) return result;
	result.x = ParseFloat(components[0]);
	result.y = ParseFloat(components[1]);
	return result;
}

CVector3 Palleon::ParseVector3(const std::string& vectorString)
{
	std::vector<std::string> components;
	boost::split(components, vectorString, boost::is_any_of(", "), boost::algorithm::token_compress_on);
	CVector3 result(0, 0, 0);
	if(components.size() != 3) return result;
	result.x = ParseFloat(components[0]);
	result.y = ParseFloat(components[1]);
	result.z = ParseFloat(components[2]);
	return result;
}

CColor Palleon::ParseColor(const std::string& vectorString)
{
	std::vector<std::string> components;
	boost::split(components, vectorString, boost::is_any_of(", "), boost::algorithm::token_compress_on);
	CColor result(0, 0, 0, 0);
	if(components.size() != 4) return result;
	result.r = ParseFloat(components[0]);
	result.g = ParseFloat(components[1]);
	result.b = ParseFloat(components[2]);
	result.a = ParseFloat(components[3]);
	return result;
}
