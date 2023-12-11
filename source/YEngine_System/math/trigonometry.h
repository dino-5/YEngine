#pragma once

#include "common.h"
#include <cmath>

namespace math
{
	using degrees = float;
	using radians = float;
	inline radians convertToRadians(degrees aAngle)
	{
		return aAngle * constants::PI / 180.f;
	}

	class Radians
	{
	public:
		Radians(degrees aDegrees) : m_radians(convertToRadians(aDegrees))
		{}
		static Radians CreateFromDegrees(degrees aDegrees)
		{
			return Radians(aDegrees);
		}

		static Radians CreateFromRadians(radians aRadians)
		{
			Radians ret;
			ret.m_radians = aRadians;
			return ret;

		}
		Radians operator+(const Radians& obj)
		{
			Radians ret;
			ret.m_radians = m_radians + obj.m_radians;
			ret.round();
			return ret;
		}
		void operator+=(const Radians& obj)
		{
			m_radians += obj.m_radians;
			round();
		}
		Radians operator-(const Radians& obj)
		{
			Radians ret;
			ret.m_radians = m_radians - obj.m_radians;
			ret.round();
			return ret;
		}
		operator float() { return m_radians; }
		Radians() = default;
		inline void round()
		{
			if (m_isRoundable)
				m_radians = std::fmod(m_radians, 2 * constants::PI);
		}
	private:
		radians m_radians;
		float m_isRoundable = true; // set false if there is no need in rounding below 2PI
	};

	Radians ToRadians(float angle);

}

