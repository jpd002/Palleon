#ifndef _INPUTMANAGER_H_
#define _INPUTMANAGER_H_

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
		static bool     SendInputEvent(CSceneNode*, const CVector2&, INPUT_EVENT);
	};
}

#endif
