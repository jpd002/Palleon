#pragma once

#include "Vector2.h"
#include "SceneNode.h"

namespace Palleon
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
		static void		SendInputEventToTree(const SceneNodePtr&, const CVector2&, INPUT_EVENT);

	private:
		typedef std::vector<std::function<void ()>> InputEventHandlerArray;

		static bool		SendInputEvent(const SceneNodePtr&, const CVector2&, INPUT_EVENT, InputEventHandlerArray&);
	};
}
