#include <boost/algorithm/string.hpp>
#include "palleon/Scene.h"
#include "palleon/resources/ResourceManager.h"
#include "palleon/Sprite.h"
#include "palleon/Label.h"
#include "palleon/SpriteButton.h"
#include "palleon/NinePatchButton.h"
#include "palleon/AnimationCurve.h"
#include "palleon/ParseUtils.h"
#include "layout/VerticalLayout.h"
#include "layout/HorizontalLayout.h"
#include "layout/LayoutStretch.h"
#include "palleon/LayoutNode.h"

using namespace Palleon;

static std::map<std::string, CLabel::HORIZONTAL_ALIGNMENT> c_horizontalAlignmentTranslator =
{
	{ "left",	CLabel::HORIZONTAL_ALIGNMENT_LEFT		},
	{ "center",	CLabel::HORIZONTAL_ALIGNMENT_CENTER		},
	{ "right",	CLabel::HORIZONTAL_ALIGNMENT_RIGHT		}
};

static std::map<std::string, CLabel::VERTICAL_ALIGNMENT> c_verticalAlignmentTranslator =
{
	{ "top",		CLabel::VERTICAL_ALIGNMENT_TOP			},
	{ "center",		CLabel::VERTICAL_ALIGNMENT_CENTER		},
	{ "bottom",		CLabel::VERTICAL_ALIGNMENT_BOTTOM		}
};

static std::map<std::string, ALPHA_BLENDING_MODE> c_alphaBlendingModeTranslator =
{
	{ "none",		ALPHA_BLENDING_NONE		},
	{ "lerp",		ALPHA_BLENDING_LERP		},
	{ "add",		ALPHA_BLENDING_ADD		}
};

static std::map<std::string, ANIMATION_INTERPOLATION_TYPE> c_animationCurveInterpolationTranslator =
{
	{ "linear",				ANIMATION_INTERPOLATION_LINEAR				},
	{ "easeInOut",			ANIMATION_INTERPOLATION_EASE_INOUT			},
	{ "strongEaseInOut",	ANIMATION_INTERPOLATION_STRONG_EASE_INOUT	}
};

CScene::CScene(const CSceneDescriptor* descriptor)
{
	CreateScene(descriptor);
}

CScene::~CScene()
{

}

ScenePtr CScene::Create(const CSceneDescriptor* descriptor)
{
	return std::make_shared<CScene>(descriptor);
}

SceneNodePtr CScene::FindNode(const std::string& name)
{
	SceneNodePtr result;
	TraverseNodes(
		[&](const SceneNodePtr& node)
		{
			if(result) return false;
			if(node->GetName() == name)
			{
				result = node;
				return false;
			}
			return true;
		}
	);
	assert(result);
	return result;
}

CScene::LayoutObjectPtr CScene::GetLayout() const
{
	return m_layout;
}

SceneNodeAnimationPtr CScene::GetAnimation(const std::string& name) const
{
	auto animationIterator = m_animations.find(name);
	if(animationIterator == std::end(m_animations))
	{
		return SceneNodeAnimationPtr();
	}
	return animationIterator->second;
}

void CScene::CreateScene(const CSceneDescriptor* descriptor)
{
	CreateMaterials(descriptor);
	CreateAnimations(descriptor);
	auto rootNodeInfo = descriptor->GetRootNode();
	CreateNodes(this, descriptor, rootNodeInfo.children);
	auto layoutRootNode = descriptor->GetLayoutRootNode();
	if(!layoutRootNode.children.empty())
	{
		m_layout = CreateLayoutNode(descriptor, layoutRootNode.children[0]);
	}
}

template <typename ValueType>
static ValueType GetValueFromItemInfo(const CSceneDescriptor::ItemInfo& itemInfo, const std::string& propertyName, const ValueType& defaultValue);

template <>
float GetValueFromItemInfo<float>(const CSceneDescriptor::ItemInfo& itemInfo, const std::string& propertyName, const float& defaultValue)
{
	auto valueIterator = itemInfo.find(propertyName);
	if(valueIterator != std::end(itemInfo))
	{
		return ParseFloat(valueIterator->second);
	}
	else
	{
		return defaultValue;
	}
}

template <>
CVector2 GetValueFromItemInfo<CVector2>(const CSceneDescriptor::ItemInfo& itemInfo, const std::string& propertyName, const CVector2& defaultValue)
{
	auto valueIterator = itemInfo.find(propertyName);
	if(valueIterator != std::end(itemInfo))
	{
		return ParseVector2(valueIterator->second);
	}
	else
	{
		return defaultValue;
	}
}

template <>
CVector3 GetValueFromItemInfo<CVector3>(const CSceneDescriptor::ItemInfo& itemInfo, const std::string& propertyName, const CVector3& defaultValue)
{
	auto valueIterator = itemInfo.find(propertyName);
	if(valueIterator != std::end(itemInfo))
	{
		return ParseVector3(valueIterator->second);
	}
	else
	{
		return defaultValue;
	}
}

template <>
CColor GetValueFromItemInfo<CColor>(const CSceneDescriptor::ItemInfo& itemInfo, const std::string& propertyName, const CColor& defaultValue)
{
	auto valueIterator = itemInfo.find(propertyName);
	if(valueIterator != std::end(itemInfo))
	{
		return ParseColor(valueIterator->second);
	}
	else
	{
		return defaultValue;
	}
}

template <>
bool GetValueFromItemInfo<bool>(const CSceneDescriptor::ItemInfo& itemInfo, const std::string& propertyName, const bool& defaultValue)
{
	auto valueIterator = itemInfo.find(propertyName);
	if(valueIterator != std::end(itemInfo))
	{
		if(valueIterator->second == "true")
		{
			return true;
		}
		else if(valueIterator->second == "false")
		{
			return false;
		}
		else
		{
			return defaultValue;
		}
	}
	else
	{
		return defaultValue;
	}
}

template <>
std::string GetValueFromItemInfo<std::string>(const CSceneDescriptor::ItemInfo& itemInfo, const std::string& propertyName, const std::string& defaultValue)
{
	auto valueIterator = itemInfo.find(propertyName);
	if(valueIterator != std::end(itemInfo))
	{
		return valueIterator->second;
	}
	else
	{
		return defaultValue;
	}
}

template <typename ReturnType>
ReturnType GetEnumFromItemInfo(const CSceneDescriptor::ItemInfo& itemInfo, const std::string& propertyName, const std::map<std::string, ReturnType>& translator)
{
	auto valueIterator = itemInfo.find(propertyName);
	if(valueIterator != std::end(itemInfo))
	{
		auto enumValueIterator = translator.find(valueIterator->second);
		if(enumValueIterator != std::end(translator))
		{
			return enumValueIterator->second;
		}
		else
		{
			return ReturnType();
		}
	}
	else
	{
		return ReturnType();
	}
}

MaterialPtr CScene::GetMaterialFromItemInfo(const CSceneDescriptor::ItemInfo& itemInfo) const
{
	auto materialValueIterator = itemInfo.find("Material");
	if(materialValueIterator != std::end(itemInfo))
	{
		auto materialIterator = m_materials.find(materialValueIterator->second);
		if(materialIterator != std::end(m_materials))
		{
			return materialIterator->second;
		}
		else
		{
			//Material was specified but wasn't found in scene
			assert(0);
			return MaterialPtr();
		}
	}
	else
	{
		return MaterialPtr();
	}
}

void CScene::RegisterNodeAnimations(const SceneNodePtr& node, const std::string& animationNamesString)
{
	std::vector<std::string> animationNames;
	boost::split(animationNames, animationNamesString, boost::is_any_of(", "), boost::algorithm::token_compress_on);
	for(const auto& animationName : animationNames)
	{
		if(animationName.empty()) continue;
		const auto& animation = GetAnimation(animationName);
		assert(animation);
		node->GetAnimationController().AddAnimation(animationName, animation);
	}
}

SceneNodePtr CScene::CreateNode(const CSceneDescriptor* descriptor, const CSceneDescriptor::NODE_INFO& baseNodeDesc)
{
	CSceneDescriptor::NODE_INFO nodeDesc = baseNodeDesc;

	auto className = GetValueFromItemInfo<std::string>(nodeDesc.properties, "Class", "");
	const auto& styles = descriptor->GetStyles();
	auto styleInfoIterator = styles.find(className);
	if(styleInfoIterator != std::end(styles))
	{
		for(const auto& styleItemPair : styleInfoIterator->second)
		{
			if(nodeDesc.properties.find(styleItemPair.first) != std::end(nodeDesc.properties)) continue;
			nodeDesc.properties.insert(styleItemPair);
		}
	}

	auto position = GetValueFromItemInfo<CVector3>(nodeDesc.properties, "Position", CVector3(0, 0, 0));
	auto scale = GetValueFromItemInfo<CVector3>(nodeDesc.properties, "Scale", CVector3(1, 1, 1));
	auto hotspot = GetValueFromItemInfo<CVector3>(nodeDesc.properties, "Hotspot", CVector3(0, 0, 0));
	bool visible = GetValueFromItemInfo<bool>(nodeDesc.properties, "Visible", true);
	auto animations = GetValueFromItemInfo<std::string>(nodeDesc.properties, "Animations", "");

	SceneNodePtr result;
	if(nodeDesc.type == "Sprite")
	{
		auto size = GetValueFromItemInfo<CVector2>(nodeDesc.properties, "Size", CVector2(1, 1));
		auto material = GetMaterialFromItemInfo(nodeDesc.properties);

		auto sprite = CSprite::Create();
		sprite->SetSize(size);
		if(material)
		{
			sprite->SetMaterial(material);
		}
		result = sprite;
	}
	else if(nodeDesc.type == "NinePatch")
	{
		auto size = GetValueFromItemInfo<CVector2>(nodeDesc.properties, "Size", CVector2(1, 1));
		auto ninePatch = GetValueFromItemInfo<std::string>(nodeDesc.properties, "NinePatch", "");

		auto ninePatchNode = CNinePatch::Create();
		ninePatchNode->SetDescriptor(CResourceManager::GetInstance().GetResource<CNinePatchDescriptor>(ninePatch));
		ninePatchNode->SetSize(size);
		result = ninePatchNode;
	}
	else if(nodeDesc.type == "Label")
	{
		auto size = GetValueFromItemInfo<CVector2>(nodeDesc.properties, "Size", CVector2(1, 1));
		auto font = GetValueFromItemInfo<std::string>(nodeDesc.properties, "Font", "");
		auto text = GetValueFromItemInfo<std::string>(nodeDesc.properties, "Text", "");
		auto textScale = GetValueFromItemInfo<CVector2>(nodeDesc.properties, "TextScale", CVector2(1, 1));
		auto horizontalAlignment = GetEnumFromItemInfo(nodeDesc.properties, "HorizontalAlignment", c_horizontalAlignmentTranslator);
		auto verticalAlignment = GetEnumFromItemInfo(nodeDesc.properties, "VerticalAlignment", c_verticalAlignmentTranslator);

		auto label = CLabel::Create();
		label->SetSize(size);
		label->SetFont(CResourceManager::GetInstance().GetResource<CFontDescriptor>(font));
		label->SetText(text);
		label->SetHorizontalAlignment(horizontalAlignment);
		label->SetVerticalAlignment(verticalAlignment);
		label->SetTextScale(textScale);
		result = label;
	}
	else if(nodeDesc.type == "SpriteButton")
	{
		auto size = GetValueFromItemInfo<CVector2>(nodeDesc.properties, "Size", CVector2(1, 1));
		auto font = GetValueFromItemInfo<std::string>(nodeDesc.properties, "Font", "");
		auto text = GetValueFromItemInfo<std::string>(nodeDesc.properties, "Text", "");
		auto releasedTexture = GetValueFromItemInfo<std::string>(nodeDesc.properties, "ReleasedTexture", "");
		auto pressedTexture = GetValueFromItemInfo<std::string>(nodeDesc.properties, "PressedTexture", "");

		auto button = CSpriteButton::Create();
		button->SetSize(size);
		if(!font.empty())
		{
			button->SetFont(CResourceManager::GetInstance().GetResource<CFontDescriptor>(font));
		}
		if(!releasedTexture.empty())
		{
			button->SetReleasedTexture(CResourceManager::GetInstance().GetTexture(releasedTexture));
		}
		if(!pressedTexture.empty())
		{
			button->SetPressedTexture(CResourceManager::GetInstance().GetTexture(pressedTexture));
		}
		button->SetText(text);
		result = button;
	}
	else if(nodeDesc.type == "NinePatchButton")
	{
		auto size = GetValueFromItemInfo<CVector2>(nodeDesc.properties, "Size", CVector2(1, 1));
		auto font = GetValueFromItemInfo<std::string>(nodeDesc.properties, "Font", "");
		auto text = GetValueFromItemInfo<std::string>(nodeDesc.properties, "Text", "");
		auto releasedNinePatch = GetValueFromItemInfo<std::string>(nodeDesc.properties, "ReleasedNinePatch", "");
		auto pressedNinePatch = GetValueFromItemInfo<std::string>(nodeDesc.properties, "PressedNinePatch", "");

		auto button = CNinePatchButton::Create();
		button->SetSize(size);
		if(!font.empty())
		{
			button->SetFont(CResourceManager::GetInstance().GetResource<CFontDescriptor>(font));
		}
		button->SetText(text);
		if(!releasedNinePatch.empty())
		{
			button->SetReleasedDescriptor(CResourceManager::GetInstance().GetResource<CNinePatchDescriptor>(releasedNinePatch));
		}
		if(!pressedNinePatch.empty())
		{
			button->SetPressedDescriptor(CResourceManager::GetInstance().GetResource<CNinePatchDescriptor>(pressedNinePatch));
		}
		result = button;
	}
	else
	{
		result = CSceneNode::Create();
	}

	assert(result);
	if(result)
	{
		result->SetName(nodeDesc.name);
		result->SetPosition(position);
		result->SetScale(scale);
		result->SetVisible(visible);
		result->SetHotspot(hotspot);
		RegisterNodeAnimations(result, animations);

		CreateNodes(result.get(), descriptor, nodeDesc.children);
	}

	return result;
}

void CScene::CreateNodes(CSceneNode* parentNode, const CSceneDescriptor* descriptor, const CSceneDescriptor::NodeInfoArray& nodeInfos)
{
	for(const auto& nodeInfo : nodeInfos)
	{
		auto childNode = CreateNode(descriptor, nodeInfo);
		parentNode->AppendChild(childNode);
	}
}

CScene::LayoutObjectPtr CScene::CreateLayoutNode(const CSceneDescriptor* descriptor, const CSceneDescriptor::NODE_INFO& nodeDesc)
{
	LayoutObjectPtr result;
	if(nodeDesc.type == "Horizontal")
	{
		auto layout = Framework::CHorizontalLayout::Create();
		result = layout;
	}
	else if(nodeDesc.type == "Vertical")
	{
		auto layout = Framework::CVerticalLayout::Create();
		result = layout;
	}
	else if(nodeDesc.type == "Stretch")
	{
		auto stretch = Framework::CLayoutStretch::Create();
		result = stretch;
	}
	else if(nodeDesc.type == "Item")
	{
		auto size = GetValueFromItemInfo<CVector2>(nodeDesc.properties, "Size", CVector2(1, 1));
		auto targetNode = FindNode<ILayoutable>(nodeDesc.name);
		assert(targetNode);
		auto item = std::make_shared<CLayoutNode>(size.x, size.y, 0, 0, targetNode);
		result = item;
	}
	else
	{
		assert(0);
	}

	if(auto flatLayout = std::dynamic_pointer_cast<Framework::CFlatLayout>(result))
	{
		for(const auto& nodeInfo : nodeDesc.children)
		{
			auto childNode = CreateLayoutNode(descriptor, nodeInfo);
			flatLayout->InsertObject(childNode);
		}
	}
	else
	{
		assert(nodeDesc.children.empty());
	}

	return result;
}

void CScene::CreateMaterials(const CSceneDescriptor* descriptor)
{
	for(const auto& materialInfoPair : descriptor->GetMaterials())
	{
		const auto& materialInfo = materialInfoPair.second;
		auto material = CMaterial::Create();
		auto texture0 = GetValueFromItemInfo<std::string>(materialInfo, "Texture0", "");
		auto alphaBlendingMode = GetEnumFromItemInfo(materialInfo, "AlphaBlendingMode", c_alphaBlendingModeTranslator);
		auto color = GetValueFromItemInfo<CColor>(materialInfo, "Color", CColor(1, 1, 1, 1));
		if(!texture0.empty())
		{
			material->SetTexture(0, CResourceManager::GetInstance().GetTexture(texture0.c_str()));
		}
		material->SetAlphaBlendingMode(alphaBlendingMode);
		material->SetColor(color);
		m_materials.insert(std::make_pair(materialInfoPair.first, material));
	}
}

template <typename ModifierType>
std::shared_ptr<CAnimationCurve<ModifierType>> CreateAnimationCurveFromAnimationInfo(const CSceneDescriptor::ANIMATION_INFO& animationInfo)
{
	auto animationCurve = std::make_shared<CAnimationCurve<ModifierType>>();
	float previousTime = 0;
	typename ModifierType::ItemType previousValue = typename ModifierType::ItemType();
	ANIMATION_INTERPOLATION_TYPE previousInterpolation = ANIMATION_INTERPOLATION_TYPE();
	bool previousValid = false;
	for(const auto& animationKeyInfo : animationInfo.keys)
	{
		auto time = GetValueFromItemInfo<float>(animationKeyInfo, "Time", 0);
		auto value = GetValueFromItemInfo<typename ModifierType::ItemType>(animationKeyInfo, "Value", typename ModifierType::ItemType());
		auto interpolation = GetEnumFromItemInfo(animationKeyInfo, "Interpolation", c_animationCurveInterpolationTranslator);
		if(previousValid)
		{
			assert((time - previousTime) > 0);
			animationCurve->AddPart(previousValue, value, time - previousTime, previousInterpolation);
		}
		previousTime = time;
		previousValue = value;
		previousInterpolation = interpolation;
		previousValid = true;
	}
	return animationCurve;
}

void CScene::CreateAnimations(const CSceneDescriptor* descriptor)
{
	for(const auto& animationInfoPair : descriptor->GetAnimations())
	{
		const auto& animationInfo = animationInfoPair.second;
		auto type = GetValueFromItemInfo<std::string>(animationInfo.properties, "Type", "");
		if(type.empty()) continue;

		SceneNodeAnimationPtr animation;
		if(type == "OpacityCurve")
		{
			animation = CreateAnimationCurveFromAnimationInfo<COpacityModifier>(animationInfo);
		}
		else if(type == "ColorCurve")
		{
			animation = CreateAnimationCurveFromAnimationInfo<CColorModifier>(animationInfo);
		}
		else if(type == "PositionCurve")
		{
			animation = CreateAnimationCurveFromAnimationInfo<CPositionModifier>(animationInfo);
		}
		else if(type == "ScaleCurve")
		{
			animation = CreateAnimationCurveFromAnimationInfo<CScaleModifier>(animationInfo);
		}
		else
		{
			//Unknown animation type
			assert(0);
		}

		if(animation)
		{
			m_animations.insert(std::make_pair(animationInfoPair.first, animation));
		}
	}
}
