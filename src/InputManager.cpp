#include <functional>
#include <assert.h>
#include "athena/InputManager.h"
#include "athena/Widget.h"
#include "athena/Box2.h"

using namespace Athena;

bool CInputManager::SendInputEvent(CSceneNode* node, const CVector2& inputPosition, INPUT_EVENT event)
{
	if(node->GetNodeType() == SCENE_NODE_WIDGET)
	{
		if(!node->GetWorldVisibility()) return false;

		CWidget* widget = static_cast<CWidget*>(node);
		CVector2 position = widget->GetWorldPosition().xy();
		CVector2 scale = widget->GetWorldScale().xy();
		CVector2 size = widget->GetSize();
		size.x *= scale.x;
		size.y *= scale.y;
		CBox2 boundingBox(position, size);
		bool inside = boundingBox.IsPointInBox(inputPosition);
		switch(event)
		{
			case INPUT_EVENT_PRESSED:
				widget->OnTouchPressed(inside);
				break;
			case INPUT_EVENT_RELEASED:
				widget->OnTouchReleased(inside);
				break;
			case INPUT_EVENT_MOVED:
				break;
			default:
				assert(0);
				break;
		}
	}
	return true;
}
