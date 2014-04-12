#include <functional>
#include <assert.h>
#include "palleon/InputManager.h"
#include "palleon/Widget.h"
#include "palleon/Box2.h"

using namespace Palleon;

bool CInputManager::SendInputEvent(const SceneNodePtr& node, const CVector2& inputPosition, INPUT_EVENT event, InputEventHandlerArray& eventHandlers)
{
	if(node->GetNodeType() == SCENE_NODE_WIDGET)
	{
		if(!node->GetWorldVisibility()) return false;

		auto widget = std::static_pointer_cast<CWidget>(node);
		CMatrix4 transformation = widget->GetWorldTransformation();
		CVector2 position(transformation(3, 0), transformation(3, 1));
		CVector2 scale(transformation(0, 0), transformation(1, 1));
		CVector2 size = widget->GetSize();
		size.x *= scale.x;
		size.y *= scale.y;
		CBox2 boundingBox(position, size);
		bool inside = boundingBox.IsPointInBox(inputPosition);
		switch(event)
		{
			case INPUT_EVENT_PRESSED:
				eventHandlers.push_back(
					[widget, inside]()
					{
						widget->OnTouchPressed(inside);
					});
				break;
			case INPUT_EVENT_RELEASED:
				eventHandlers.push_back(
					[widget, inside]()
					{
						widget->OnTouchReleased(inside);
					});
				break;
			case INPUT_EVENT_MOVED:
				if(inside)
				{
					CVector2 localPosition = inputPosition - position;
					eventHandlers.push_back(
						[widget, localPosition]()
						{
							widget->OnTouchMoved(localPosition);
						});
				}
				break;
			default:
				assert(0);
				break;
		}
	}
	return true;
}

void CInputManager::SendInputEventToTree(const SceneNodePtr& root, const CVector2& inputPosition, INPUT_EVENT event)
{
	InputEventHandlerArray eventHandlers;
	root->TraverseNodes(
		[&] (const SceneNodePtr& node)
		{
			return SendInputEvent(node, inputPosition, event, eventHandlers);
		}
	);
	for(const auto& eventHandler : eventHandlers)
	{
		eventHandler();
	}
}
