#pragma once

#include <string>
#include "palleon/Vector2.h"
#include "palleon/Vector3.h"
#include "palleon/graphics/Color.h"

namespace Palleon
{
	float		ParseFloat(const std::string&);
	CVector2	ParseVector2(const std::string&);
	CVector3	ParseVector3(const std::string&);
	CColor		ParseColor(const std::string&);
};
