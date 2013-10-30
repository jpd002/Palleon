#include <sstream>
#include "athena/Scene.h"
#include "athena/resources/ResourceManager.h"
#include "athena/Sprite.h"
#include "athena/Label.h"
#include "athena/AnimationCurve.h"
#include <boost/algorithm/string.hpp>

using namespace Athena;

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
	return result;
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
	CreateNodes(this, rootNodeInfo.children);
}

static float ParseFloat(const std::string& valueString)
{
	float result = 0;
	std::stringstream ss;
	ss.imbue(std::locale::classic());
	ss << valueString;
	ss >> result;
	return result;
}

static CVector2 ParseVector2(const std::string& vectorString)
{
	std::vector<std::string> components;
	boost::split(components, vectorString, boost::is_any_of(", "), boost::algorithm::token_compress_on);
	CVector2 result(0, 0);
	if(components.size() != 2) return result;
	result.x = ParseFloat(components[0]);
	result.y = ParseFloat(components[1]);
	return result;
}

static CVector3 ParseVector3(const std::string& vectorString)
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

static CColor ParseColor(const std::string& vectorString)
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

template <typename ValueType>
static ValueType GetValueFromItemInfo(const CSceneDescriptor::ItemInfo& itemInfo, const std::string& propertyName, const ValueType& defaultValue);

template <>
static float GetValueFromItemInfo<float>(const CSceneDescriptor::ItemInfo& itemInfo, const std::string& propertyName, const float& defaultValue)
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
static CVector2 GetValueFromItemInfo<CVector2>(const CSceneDescriptor::ItemInfo& itemInfo, const std::string& propertyName, const CVector2& defaultValue)
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
static CVector3 GetValueFromItemInfo<CVector3>(const CSceneDescriptor::ItemInfo& itemInfo, const std::string& propertyName, const CVector3& defaultValue)
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
static CColor GetValueFromItemInfo<CColor>(const CSceneDescriptor::ItemInfo& itemInfo, const std::string& propertyName, const CColor& defaultValue)
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
static bool GetValueFromItemInfo<bool>(const CSceneDescriptor::ItemInfo& itemInfo, const std::string& propertyName, const bool& defaultValue)
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
static std::string GetValueFromItemInfo<std::string>(const CSceneDescriptor::ItemInfo& itemInfo, const std::string& propertyName, const std::string& defaultValue)
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

SceneNodePtr CScene::CreateNode(const CSceneDescriptor::NODE_INFO& nodeDesc)
{
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
		label->SetFont(CResourceManager::GetInstance().GetResource<CFontDescriptor>(font.c_str()));
		label->SetText(text.c_str());
		label->SetHorizontalAlignment(horizontalAlignment);
		label->SetVerticalAlignment(verticalAlignment);
		label->SetTextScale(textScale);
		result = label;
	}
	else
	{
		result = CSceneNode::Create();
	}

	result->SetName(nodeDesc.name);
	result->SetPosition(position);
	result->SetScale(scale);
	result->SetVisible(visible);
	result->SetHotspot(hotspot);
	RegisterNodeAnimations(result, animations);

	CreateNodes(result.get(), nodeDesc.children);

	return result;
}

void CScene::CreateNodes(CSceneNode* parentNode, const CSceneDescriptor::NodeInfoArray& nodeInfos)
{
	for(const auto& nodeInfo : nodeInfos)
	{
		auto childNode = CreateNode(nodeInfo);
		parentNode->AppendChild(childNode);
	}
}

void CScene::CreateMaterials(const CSceneDescriptor* descriptor)
{
	for(const auto& materialInfoPair : descriptor->GetMaterials())
	{
		const auto& materialInfo = materialInfoPair.second;
		auto material = CMaterial::Create();
		auto texture0 = GetValueFromItemInfo<std::string>(materialInfo, "Texture0", "");
		auto alphaBlendingMode = GetEnumFromItemInfo(materialInfo, "AlphaBlendingMode", c_alphaBlendingModeTranslator);
		if(!texture0.empty())
		{
			material->SetTexture(0, CResourceManager::GetInstance().GetTexture(texture0.c_str()));
		}
		material->SetAlphaBlendingMode(alphaBlendingMode);
		m_materials.insert(std::make_pair(materialInfoPair.first, material));
	}
}

template <typename ModifierType>
std::shared_ptr<CAnimationCurve<ModifierType>> CreateAnimationCurveFromAnimationInfo(const CSceneDescriptor::ANIMATION_INFO& animationInfo)
{
	auto animationCurve = std::make_shared<CAnimationCurve<ModifierType>>();
	float previousTime = 0;
	typename ModifierType::ItemType previousValue = ModifierType::ItemType();
	ANIMATION_INTERPOLATION_TYPE previousInterpolation = ANIMATION_INTERPOLATION_TYPE();
	bool previousValid = false;
	for(const auto& animationKeyInfo : animationInfo.keys)
	{
		auto time = GetValueFromItemInfo<float>(animationKeyInfo, "Time", 0);
		auto value = GetValueFromItemInfo<ModifierType::ItemType>(animationKeyInfo, "Value", ModifierType::ItemType());
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
