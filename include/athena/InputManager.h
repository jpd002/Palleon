#pragma once

#include "Vector2.h"
#include "SceneNode.h"

namespace Athena
{
	enum INPUT_EVENT
	{
		INPUT_EVENT_PRESSED,
		INPUT_EVENT_MOVED,
		INPUT_EVENT_RELEASED,
	};

	class CInputManager
	{
	public:
		static bool		SendInputEvent(const SceneNodePtr&, const CVector2&, INPUT_EVENT);
		static void		SendInputEventToTree(const SceneNodePtr&, const CVector2&, INPUT_EVENT);
	};
}
