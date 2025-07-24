#pragma once
#include "EngineHeader.h"
#include <random>
#include <cstdint>

namespace CHIKU
{
	namespace Utils
	{
		static std::random_device  m_RandomDevice;
		static std::mt19937_64 m_Range;

		template<typename T>
		T GetRandomNumber()
		{
			ZoneScoped;

			std::uniform_int_distribution<uint64_t> dist;
			return dist(m_Range);
		}

		// hash_combine helper
		inline void hash_combine(std::size_t& seed, std::size_t value) {
			seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
	}
}