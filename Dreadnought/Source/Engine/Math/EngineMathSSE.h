#pragma once
#include <immintrin.h>
namespace SSE
{
	inline static float rsqrt(float x)
	{
		const __m128 oneHalf = _mm_set_ss(0.5f);
		__m128 y0, x0, x1, x2, xOver2;
		float result;
		y0 = _mm_set_ss(x);
		x0 = _mm_rsqrt_ss(y0);
		xOver2 = _mm_mul_ss(y0, oneHalf);

		x1 = _mm_mul_ss(x0, x0);
		x1 = _mm_sub_ss(oneHalf, _mm_mul_ss(xOver2, x1));
		x1 = _mm_add_ss(x0, _mm_mul_ss(x0, x1));

		x2 = _mm_mul_ss(x1, x1);
		x2 = _mm_sub_ss(oneHalf, _mm_mul_ss(xOver2, x2));
		x2 = _mm_add_ss(x1, _mm_mul_ss(x1, x2));

		_mm_store_ss(&result, x2);
		return result;
	}

	inline static float rsqrt_fast(float x)
	{
		const __m128 oneHalf = _mm_set_ss(0.5f);
		__m128 y0, x0, x1, xOver2;
		float result;
		y0 = _mm_set_ss(x);
		x0 = _mm_rsqrt_ss(y0);
		xOver2 = _mm_mul_ss(y0, oneHalf);

		x1 = _mm_mul_ss(x0, x0);
		x1 = _mm_sub_ss(oneHalf, _mm_mul_ss(xOver2, x1));
		x1 = _mm_add_ss(x0, _mm_mul_ss(x0, x1));

		_mm_store_ss(&result, x1);
		return result;
	}
}