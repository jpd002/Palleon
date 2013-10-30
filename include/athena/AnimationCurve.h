#pragma once

#include "Vector3.h"
#include "IAnimation.h"
#include "SceneNode.h"
#include "Mesh.h"
#include "Tween.h"
#include <vector>

namespace Athena
{
	enum ANIMATION_INTERPOLATION_TYPE
	{
		ANIMATION_INTERPOLATION_LINEAR,
		ANIMATION_INTERPOLATION_EASE_INOUT,
		ANIMATION_INTERPOLATION_STRONG_EASE_INOUT
	};

	class CPositionModifier
	{
	public:
		typedef CVector3 ItemType;
		typedef CSceneNode TargetType;
		
		void operator()(CSceneNode* target, const ItemType& value) const
		{
			target->SetPosition(value);
		}
	};

	class CScaleModifier
	{
	public:
		typedef CVector3 ItemType;
		typedef CSceneNode TargetType;
		
		void operator()(CSceneNode* target, const ItemType& value) const
		{
			target->SetScale(value);
		}
	};

	class CColorModifier
	{
	public:
		typedef CColor ItemType;
		typedef CSceneNode TargetType;

		void operator()(CSceneNode* target, const ItemType& value) const
		{
			if(CMesh* mesh = dynamic_cast<CMesh*>(target))
			{
				mesh->GetMaterial()->SetColor(value);
			}
		}
	};

	class COpacityModifier
	{
	public:
		typedef float ItemType;
		typedef CSceneNode TargetType;

		void operator()(CSceneNode* target, const ItemType& value) const
		{
			if(CMesh* mesh = dynamic_cast<CMesh*>(target))
			{
				auto color = mesh->GetMaterial()->GetColor();
				color.a = value;
				mesh->GetMaterial()->SetColor(color);
			}
		}
	};

	template<typename Modifier>
	class CAnimationCurve : public IAnimation<typename Modifier::TargetType>
	{
	public:
		typedef typename Modifier::ItemType ItemType;
		typedef typename Modifier::TargetType TargetType;

		CAnimationCurve()
			: m_length(0)
		{

		}

		virtual ~CAnimationCurve()
		{

		}

		void AddPart(const ItemType& from, const ItemType& to, float length, ANIMATION_INTERPOLATION_TYPE interpolationType = ANIMATION_INTERPOLATION_LINEAR)
		{
			assert(length != 0);

			if(m_parts.size() != 0)
			{
				const auto& prevPart = m_parts[m_parts.size() - 1];
				assert(prevPart.to == from);
			}

			CURVE_PART part;
			part.from				= from;
			part.to					= to;
			part.length				= length;
			part.interpolationType	= interpolationType;
			m_parts.push_back(part);

			m_length += length;
		}

		virtual void Animate(TargetType* target, float t) const
		{
			for(auto partIterator(m_parts.begin());
				partIterator != m_parts.end(); partIterator++)
			{
				const auto& part(*partIterator);
				if(t <= part.length)
				{
					float alpha = t / part.length;
					ItemType value;
					switch(part.interpolationType)
					{
					default:
					case ANIMATION_INTERPOLATION_LINEAR:
						value = (alpha * part.to) + ((1 - alpha) * part.from);
						break;
					case ANIMATION_INTERPOLATION_EASE_INOUT:
						value = CTween::EaseInOut(alpha, part.from, part.to, 1);
						break;
					case ANIMATION_INTERPOLATION_STRONG_EASE_INOUT:
						value = CTween::StrongEaseInOut(alpha, part.from, part.to, 1);
						break;
					}
					Modifier()(target, value);
					return;
				}
				t -= part.length;
			}

			if(m_parts.size() != 0)
			{
				const auto& part(*m_parts.rbegin());
				Modifier()(target, part.to);
			}
		}

		virtual float GetLength() const
		{
			return m_length;
		}

	private:
		struct CURVE_PART
		{
			ItemType						from;
			ItemType						to;
			float							length;
			ANIMATION_INTERPOLATION_TYPE	interpolationType;
		};

		typedef std::vector<CURVE_PART> CurvePartArray;

		CurvePartArray		m_parts;
		float				m_length;
	};
}
