#pragma once

#include <random>
#include <cassert>

namespace ext {
	extern std::default_random_engine e;

	inline void init() {
		// ???
	}
	
	inline std::default_random_engine & get_engine() {
		return e;
	}


	inline int8_t uniform_i8(int8_t A, int8_t B)
	{
		assert(B > A);
		std::uniform_int_distribution<int8_t> d{A, int8_t(B-1)};
		return d(e);
	}

	inline int16_t uniform_i16(int16_t A, int16_t B)
	{
		assert(B > A);
		std::uniform_int_distribution<int16_t> d{A, int16_t(B-1)};
		return d(e);
	}

	inline int32_t uniform_i32(int32_t a, int32_t b)
	{
		assert(b > a);
		std::uniform_int_distribution<int32_t> d{a, int32_t(b-1)};
		return d(e);
	}

	inline float uniform_f(float a, float b)
	{
		std::uniform_real_distribution<float> d{a, b};
		return d(e);
	}

	inline float normal_f(float m, float sd)
	{
		std::normal_distribution<float> d{m,sd};
		return d(e);
	}

	inline float bernoulli_f(float p)
	{
		std::bernoulli_distribution d{p};
		return d(e);
	}

}


