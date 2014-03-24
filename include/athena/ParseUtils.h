#pragma once

#include <string>
#include "Vector2.h"
#include "Vector3.h"
#include "Color.h"

namespace Athena
{
	float		ParseFloat(const std::string&);
	CVector2	ParseVector2(const std::string&);
	CVector3	ParseVector3(const std::string&);
	CColor		ParseColor(const std::string&);
};
