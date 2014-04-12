#ifndef _ATHENA_TWEEN_H_
#define _ATHENA_TWEEN_H_

//Based on http://jstween.sourceforge.net/Tween.js

namespace Athena
{
	class CTween
	{
	public:
		template <typename VariableType>
		static VariableType EaseOut(float time, const VariableType& begin, const VariableType& end, float duration)
		{
			float normTime = time / duration;
			VariableType delta = end - begin;
			return -delta * (normTime) * (normTime - 2) + begin;
		}

		template <typename VariableType>
		static VariableType EaseInOut(float time, const VariableType& begin, const VariableType& end, float duration)
		{
			float normTime = time / (duration / 2);
			VariableType delta = end - begin;

			if(normTime < 1)
			{
				return (delta / 2) * normTime * normTime + begin;
			}
			else
			{
				return (-delta / 2) * ((--normTime) * (normTime - 2) - 1) + begin;
			}
		}

		template <typename VariableType>
		static VariableType StrongEaseInOut(float time, const VariableType& begin, const VariableType& end, float duration)
		{
			float normTime = time / (duration / 2);
			VariableType delta = end - begin;

			if(normTime < 1)
			{
				return (delta / 2) * normTime * normTime * normTime * normTime * normTime + begin;
			}
			else
			{
				return (delta / 2) * ((normTime -= 2) * normTime * normTime * normTime * normTime + 2) + begin;
			}
		}
	};
};

#endif
