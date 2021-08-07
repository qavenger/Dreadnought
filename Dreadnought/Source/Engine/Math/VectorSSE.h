#pragma once
#include<EngineMath.h>
#include<EngineMathSSE.h>
#include <xmmintrin.h>
#include <emmintrin.h>

#define SIMD_ALIGNMENT (16)

typedef __m128		VectorRegister;
typedef __m128i		VectorRegisterInt;
typedef __m128d		VectorRegisterDouble;

namespace SSE
{
	FORCEINLINE static float rsqrt(float x)
	{
		const VectorRegister oneHalf = _mm_set_ss(0.5f);
		VectorRegister y0, x0, x1, x2, xOver2;
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

	FORCEINLINE static float rsqrt_fast(float x)
	{
		const VectorRegister oneHalf = _mm_set_ss(0.5f);
		VectorRegister y0, x0, x1, xOver2;
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

	FORCEINLINE static int32 TruncToInt(float f)
	{
		return _mm_cvtt_ss2si(_mm_set_ss(f));
	}

	FORCEINLINE static int32 FloorToInt(float f)
	{
		// from ue4
		// Note: unlike the Generic solution and the SSE4 float solution, we implement FloorToInt using a rounding instruction, rather than implementing RoundToInt using a floor instruction.  
		// We therefore need to do the same times-2 transform (with a slighly different formula) that RoundToInt does; see the note on RoundToInt
		return _mm_cvt_ss2si(_mm_set_ss(f + f - 0.5f)) >> 1;
	}

	FORCEINLINE static int32 RoundToInt(float f)
	{
		// from ue4
		// Note: the times-2 is to remove the rounding-to-nearest-even-number behavior that mm_cvt_ss2si uses when the fraction is .5
		// The formula we uses causes the round instruction to always be applied to a an odd integer when the original value was 0.5, and eliminates the rounding-to-nearest-even-number behavior
		// Input -> multiply by two and add .5 -> Round to nearest whole -> divide by two and truncate
		// N -> (2N) + .5 -> 2N (or possibly 2N+1) -> N
		// N + .5 -> 2N + 1.5 -> (round towards even now always means round up) -> 2N + 2 -> N + 1
		return _mm_cvt_ss2si(_mm_set_ss(f + f + 0.5f)) >> 1;
	}

	FORCEINLINE static int32 CeilToInt(float f)
	{
		// from ue4
		// Note: unlike the Generic solution and the SSE4 float solution, we implement CeilToInt using a rounding instruction, rather than a dedicated ceil instruction
		// We therefore need to do the same times-2 transform (with a slighly different formula) that RoundToInt does; see the note on RoundToInt
		return -(_mm_cvt_ss2si(_mm_set_ss(-0.5f - (f + f))) >> 1);
	}

#define ALIGN16 __declspec(align(16))
#define _PS_CONST(Name, Val) static const ALIGN16 float _ps_##Name[4] = {Val, Val, Val, Val}
#define _PI32_CONST(Name, Val) static const ALIGN16 int _pi32_##Name[4] = {Val, Val, Val, Val}
#define _PS_CONST_TYPE(Name,Type, Val) static const ALIGN16 Type _ps_##Name[4] = {Val, Val, Val, Val}

	_PS_CONST(one, 1.0f);
	_PS_CONST(half, 0.5f);

	_PS_CONST_TYPE(min_norm_pos, int, 0x00800000);
	_PS_CONST_TYPE(mant_mask, int, 0x7f800000);
	_PS_CONST_TYPE(inv_mant_mask, int, ~0x7f800000);

	_PS_CONST_TYPE(sign_mask, int, (int)0x80000000);
	_PS_CONST_TYPE(inv_sign_mask, int, ~0x80000000);

	_PI32_CONST(one, 1);
	_PI32_CONST(inv_one, ~1);
	_PI32_CONST(two, 2);
	_PI32_CONST(four, 4);
	_PI32_CONST(0x7f, 0x7f);

	_PS_CONST(cephes_sqrthf, 0.707106781186547524f);
	_PS_CONST(cephes_log_p0, 7.0376836292E-2f);
	_PS_CONST(cephes_log_p1, -1.1514610310E-1f);
	_PS_CONST(cephes_log_p2, 1.1676998740E-1f);
	_PS_CONST(cephes_log_p3, -1.2420140846E-1f);
	_PS_CONST(cephes_log_p4, +1.4249322787E-1f);
	_PS_CONST(cephes_log_p5, -1.6668057665E-1f);
	_PS_CONST(cephes_log_p6, +2.0000714765E-1f);
	_PS_CONST(cephes_log_p7, -2.4999993993E-1f);
	_PS_CONST(cephes_log_p8, +3.3333331174E-1f);
	_PS_CONST(cephes_log_q1, -2.12194440e-4f);
	_PS_CONST(cephes_log_q2, 0.693359375f);

	typedef union xmm_mm_union
	{
		__m128 xmm;
		__m64 mm[2];
	} xmm_mm_union;

#define COPY_XMM_TO_MM(xmm_, mm0_, mm1_){xmm_mm_union u; u.xmm = xmm_;	mm0_ = u.mm[0];	mm1_ = u.mm[1];}

#define COPYMM_TO_XMM(mm0_, mm1_, xmm_){ xmm_mm_union u; u.mm[0] = mm0_; u.mm[1] = mm1_; xmm_ = u.xmm;}

	FORCEINLINE VectorRegister SSEMath_log_ps(VectorRegister x)
	{
		VectorRegisterInt emm0;
		VectorRegister one = *(VectorRegister*)_ps_one;

		VectorRegister invalid_mask = _mm_cmple_ps(x, _mm_setzero_ps());

		x = _mm_max_ps(x, *(VectorRegister*)_ps_min_norm_pos);  /* cut off denormalized stuff */

		emm0 = _mm_srli_epi32(_mm_castps_si128(x), 23);

		/* keep only the fractional part */
		x = _mm_and_ps(x, *(VectorRegister*)_ps_inv_mant_mask);
		x = _mm_or_ps(x, *(VectorRegister*)_ps_half);

		emm0 = _mm_sub_epi32(emm0, *(VectorRegisterInt*)_pi32_0x7f);
		VectorRegister e = _mm_cvtepi32_ps(emm0);


		e = _mm_add_ps(e, one);

		/* part2:
		   if( x < SQRTHF ) {
			 e -= 1;
			 x = x + x - 1.0;
		   } else { x = x - 1.0; }
		*/
		VectorRegister mask = _mm_cmplt_ps(x, *(VectorRegister*)_ps_cephes_sqrthf);
		VectorRegister tmp = _mm_and_ps(x, mask);
		x = _mm_sub_ps(x, one);
		e = _mm_sub_ps(e, _mm_and_ps(one, mask));
		x = _mm_add_ps(x, tmp);


		VectorRegister z = _mm_mul_ps(x, x);

		VectorRegister y = *(VectorRegister*)_ps_cephes_log_p0;
		y = _mm_mul_ps(y, x);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_cephes_log_p1);
		y = _mm_mul_ps(y, x);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_cephes_log_p2);
		y = _mm_mul_ps(y, x);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_cephes_log_p3);
		y = _mm_mul_ps(y, x);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_cephes_log_p4);
		y = _mm_mul_ps(y, x);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_cephes_log_p5);
		y = _mm_mul_ps(y, x);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_cephes_log_p6);
		y = _mm_mul_ps(y, x);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_cephes_log_p7);
		y = _mm_mul_ps(y, x);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_cephes_log_p8);
		y = _mm_mul_ps(y, x);

		y = _mm_mul_ps(y, z);


		tmp = _mm_mul_ps(e, *(VectorRegister*)_ps_cephes_log_q1);
		y = _mm_add_ps(y, tmp);


		tmp = _mm_mul_ps(z, *(VectorRegister*)_ps_half);
		y = _mm_sub_ps(y, tmp);

		tmp = _mm_mul_ps(e, *(VectorRegister*)_ps_cephes_log_q2);
		x = _mm_add_ps(x, y);
		x = _mm_add_ps(x, tmp);
		x = _mm_or_ps(x, invalid_mask); // negative arg will be NAN
		return x;
	}

	_PS_CONST(exp_hi, 88.3762626647949f);
	_PS_CONST(exp_lo, -88.3762626647949f);

	_PS_CONST(cephes_LOG2EF, 1.44269504088896341f);
	_PS_CONST(cephes_exp_C1, 0.693359375f);
	_PS_CONST(cephes_exp_C2, -2.12194440e-4f);

	_PS_CONST(cephes_exp_p0, 1.9875691500E-4f);
	_PS_CONST(cephes_exp_p1, 1.3981999507E-3f);
	_PS_CONST(cephes_exp_p2, 8.3334519073E-3f);
	_PS_CONST(cephes_exp_p3, 4.1665795894E-2f);
	_PS_CONST(cephes_exp_p4, 1.6666665459E-1f);
	_PS_CONST(cephes_exp_p5, 5.0000001201E-1f);

	FORCEINLINE VectorRegister SSEMath_exp_ps(VectorRegister x) {
		VectorRegister tmp = _mm_setzero_ps(), fx;

		VectorRegisterInt emm0;

		VectorRegister one = *(VectorRegister*)_ps_one;

		x = _mm_min_ps(x, *(VectorRegister*)_ps_exp_hi);
		x = _mm_max_ps(x, *(VectorRegister*)_ps_exp_lo);

		/* express exp(x) as exp(g + n*log(2)) */
		fx = _mm_mul_ps(x, *(VectorRegister*)_ps_cephes_LOG2EF);
		fx = _mm_add_ps(fx, *(VectorRegister*)_ps_half);

		/* how to perform a floorf with SSE: just below */
		emm0 = _mm_cvttps_epi32(fx);
		tmp = _mm_cvtepi32_ps(emm0);

		/* if greater, substract 1 */
		VectorRegister mask = _mm_cmpgt_ps(tmp, fx);
		mask = _mm_and_ps(mask, one);
		fx = _mm_sub_ps(tmp, mask);

		tmp = _mm_mul_ps(fx, *(VectorRegister*)_ps_cephes_exp_C1);
		VectorRegister z = _mm_mul_ps(fx, *(VectorRegister*)_ps_cephes_exp_C2);
		x = _mm_sub_ps(x, tmp);
		x = _mm_sub_ps(x, z);

		z = _mm_mul_ps(x, x);

		VectorRegister y = *(VectorRegister*)_ps_cephes_exp_p0;
		y = _mm_mul_ps(y, x);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_cephes_exp_p1);
		y = _mm_mul_ps(y, x);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_cephes_exp_p2);
		y = _mm_mul_ps(y, x);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_cephes_exp_p3);
		y = _mm_mul_ps(y, x);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_cephes_exp_p4);
		y = _mm_mul_ps(y, x);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_cephes_exp_p5);
		y = _mm_mul_ps(y, z);
		y = _mm_add_ps(y, x);
		y = _mm_add_ps(y, one);

		emm0 = _mm_cvttps_epi32(fx);
		emm0 = _mm_add_epi32(emm0, *(VectorRegisterInt*)_pi32_0x7f);
		emm0 = _mm_slli_epi32(emm0, 23);
		VectorRegister pow2n = _mm_castsi128_ps(emm0);

		y = _mm_mul_ps(y, pow2n);
		return y;
	}

	_PS_CONST(minus_cephes_DP1, -0.78515625f);
	_PS_CONST(minus_cephes_DP2, -2.4187564849853515625e-4f);
	_PS_CONST(minus_cephes_DP3, -3.77489497744594108e-8f);
	_PS_CONST(sincof_p0, -1.9515295891E-4f);
	_PS_CONST(sincof_p1, 8.3321608736E-3f);
	_PS_CONST(sincof_p2, -1.6666654611E-1f);
	_PS_CONST(coscof_p0, 2.443315711809948E-005f);
	_PS_CONST(coscof_p1, -1.388731625493765E-003f);
	_PS_CONST(coscof_p2, 4.166664568298827E-002f);
	_PS_CONST(cephes_FOPI, 1.27323954473516f); // 4 / M_PI

	FORCEINLINE VectorRegister SSEMath_sin_ps(VectorRegister x) { // any x
		VectorRegister xmm1, xmm2 = _mm_setzero_ps(), xmm3, sign_bit, y;

		VectorRegisterInt emm0, emm2;

		sign_bit = x;
		/* take the absolute value */
		x = _mm_and_ps(x, *(VectorRegister*)_ps_inv_sign_mask);
		/* extract the sign bit (upper one) */
		sign_bit = _mm_and_ps(sign_bit, *(VectorRegister*)_ps_sign_mask);

		/* scale by 4/Pi */
		y = _mm_mul_ps(x, *(VectorRegister*)_ps_cephes_FOPI);

		/* store the integer part of y in mm0 */
		emm2 = _mm_cvttps_epi32(y);
		/* j=(j+1) & (~1) (see the cephes sources) */
		emm2 = _mm_add_epi32(emm2, *(VectorRegisterInt*)_pi32_one);
		emm2 = _mm_and_si128(emm2, *(VectorRegisterInt*)_pi32_inv_one);
		y = _mm_cvtepi32_ps(emm2);

		/* get the swap sign flag */
		emm0 = _mm_and_si128(emm2, *(VectorRegisterInt*)_pi32_four);
		emm0 = _mm_slli_epi32(emm0, 29);
		/* get the polynom selection mask
		   there is one polynom for 0 <= x <= Pi/4
		   and another one for Pi/4<x<=Pi/2

		   Both branches will be computed.
		*/
		emm2 = _mm_and_si128(emm2, *(VectorRegisterInt*)_pi32_two);
		emm2 = _mm_cmpeq_epi32(emm2, _mm_setzero_si128());

		VectorRegister swap_sign_bit = _mm_castsi128_ps(emm0);
		VectorRegister poly_mask = _mm_castsi128_ps(emm2);
		sign_bit = _mm_xor_ps(sign_bit, swap_sign_bit);


		/* The magic pass: "Extended precision modular arithmetic"
		   x = ((x - y * DP1) - y * DP2) - y * DP3; */
		xmm1 = *(VectorRegister*)_ps_minus_cephes_DP1;
		xmm2 = *(VectorRegister*)_ps_minus_cephes_DP2;
		xmm3 = *(VectorRegister*)_ps_minus_cephes_DP3;
		xmm1 = _mm_mul_ps(y, xmm1);
		xmm2 = _mm_mul_ps(y, xmm2);
		xmm3 = _mm_mul_ps(y, xmm3);
		x = _mm_add_ps(x, xmm1);
		x = _mm_add_ps(x, xmm2);
		x = _mm_add_ps(x, xmm3);

		/* Evaluate the first polynom  (0 <= x <= Pi/4) */
		y = *(VectorRegister*)_ps_coscof_p0;
		VectorRegister z = _mm_mul_ps(x, x);

		y = _mm_mul_ps(y, z);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_coscof_p1);
		y = _mm_mul_ps(y, z);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_coscof_p2);
		y = _mm_mul_ps(y, z);
		y = _mm_mul_ps(y, z);
		VectorRegister tmp = _mm_mul_ps(z, *(VectorRegister*)_ps_half);
		y = _mm_sub_ps(y, tmp);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_one);

		/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

		VectorRegister y2 = *(VectorRegister*)_ps_sincof_p0;
		y2 = _mm_mul_ps(y2, z);
		y2 = _mm_add_ps(y2, *(VectorRegister*)_ps_sincof_p1);
		y2 = _mm_mul_ps(y2, z);
		y2 = _mm_add_ps(y2, *(VectorRegister*)_ps_sincof_p2);
		y2 = _mm_mul_ps(y2, z);
		y2 = _mm_mul_ps(y2, x);
		y2 = _mm_add_ps(y2, x);

		/* select the correct result from the two polynoms */
		xmm3 = poly_mask;
		y2 = _mm_and_ps(xmm3, y2); //, xmm3);
		y = _mm_andnot_ps(xmm3, y);
		y = _mm_add_ps(y, y2);
		/* update the sign */
		y = _mm_xor_ps(y, sign_bit);
		return y;
	}

	/* almost the same as sin_ps */
	FORCEINLINE VectorRegister SseMath_cos_ps(VectorRegister x) { // any x
		VectorRegister xmm1, xmm2 = _mm_setzero_ps(), xmm3, y;
		//#ifdef USE_SSE2
#if 1
		VectorRegisterInt emm0, emm2;
#else
		v2si mm0, mm1, mm2, mm3;
#endif
		/* take the absolute value */
		x = _mm_and_ps(x, *(VectorRegister*)_ps_inv_sign_mask);

		/* scale by 4/Pi */
		y = _mm_mul_ps(x, *(VectorRegister*)_ps_cephes_FOPI);

		/* store the integer part of y in mm0 */
		emm2 = _mm_cvttps_epi32(y);
		/* j=(j+1) & (~1) (see the cephes sources) */
		emm2 = _mm_add_epi32(emm2, *(VectorRegisterInt*)_pi32_one);
		emm2 = _mm_and_si128(emm2, *(VectorRegisterInt*)_pi32_inv_one);
		y = _mm_cvtepi32_ps(emm2);

		emm2 = _mm_sub_epi32(emm2, *(VectorRegisterInt*)_pi32_two);

		/* get the swap sign flag */
		emm0 = _mm_andnot_si128(emm2, *(VectorRegisterInt*)_pi32_four);
		emm0 = _mm_slli_epi32(emm0, 29);
		/* get the polynom selection mask */
		emm2 = _mm_and_si128(emm2, *(VectorRegisterInt*)_pi32_two);
		emm2 = _mm_cmpeq_epi32(emm2, _mm_setzero_si128());

		VectorRegister sign_bit = _mm_castsi128_ps(emm0);
		VectorRegister poly_mask = _mm_castsi128_ps(emm2);

		/* The magic pass: "Extended precision modular arithmetic"
		   x = ((x - y * DP1) - y * DP2) - y * DP3; */
		xmm1 = *(VectorRegister*)_ps_minus_cephes_DP1;
		xmm2 = *(VectorRegister*)_ps_minus_cephes_DP2;
		xmm3 = *(VectorRegister*)_ps_minus_cephes_DP3;
		xmm1 = _mm_mul_ps(y, xmm1);
		xmm2 = _mm_mul_ps(y, xmm2);
		xmm3 = _mm_mul_ps(y, xmm3);
		x = _mm_add_ps(x, xmm1);
		x = _mm_add_ps(x, xmm2);
		x = _mm_add_ps(x, xmm3);

		/* Evaluate the first polynom  (0 <= x <= Pi/4) */
		y = *(VectorRegister*)_ps_coscof_p0;
		VectorRegister z = _mm_mul_ps(x, x);

		y = _mm_mul_ps(y, z);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_coscof_p1);
		y = _mm_mul_ps(y, z);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_coscof_p2);
		y = _mm_mul_ps(y, z);
		y = _mm_mul_ps(y, z);
		VectorRegister tmp = _mm_mul_ps(z, *(VectorRegister*)_ps_half);
		y = _mm_sub_ps(y, tmp);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_one);

		/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

		VectorRegister y2 = *(VectorRegister*)_ps_sincof_p0;
		y2 = _mm_mul_ps(y2, z);
		y2 = _mm_add_ps(y2, *(VectorRegister*)_ps_sincof_p1);
		y2 = _mm_mul_ps(y2, z);
		y2 = _mm_add_ps(y2, *(VectorRegister*)_ps_sincof_p2);
		y2 = _mm_mul_ps(y2, z);
		y2 = _mm_mul_ps(y2, x);
		y2 = _mm_add_ps(y2, x);

		/* select the correct result from the two polynoms */
		xmm3 = poly_mask;
		y2 = _mm_and_ps(xmm3, y2); //, xmm3);
		y = _mm_andnot_ps(xmm3, y);
		y = _mm_add_ps(y, y2);
		/* update the sign */
		y = _mm_xor_ps(y, sign_bit);

		return y;
	}

	/* since sin_ps and cos_ps are almost identical, sincos_ps could replace both of them..
	   it is almost as fast, and gives you a free cosine with your sine */
	FORCEINLINE void SseMath_sincos_ps(VectorRegister x, VectorRegister* s, VectorRegister* c) {
		VectorRegister xmm1, xmm2, xmm3 = _mm_setzero_ps(), sign_bit_sin, y;

		VectorRegisterInt emm0, emm2, emm4;

		sign_bit_sin = x;
		/* take the absolute value */
		x = _mm_and_ps(x, *(VectorRegister*)_ps_inv_sign_mask);
		/* extract the sign bit (upper one) */
		sign_bit_sin = _mm_and_ps(sign_bit_sin, *(VectorRegister*)_ps_sign_mask);

		/* scale by 4/Pi */
		y = _mm_mul_ps(x, *(VectorRegister*)_ps_cephes_FOPI);

		/* store the integer part of y in emm2 */
		emm2 = _mm_cvttps_epi32(y);

		/* j=(j+1) & (~1) (see the cephes sources) */
		emm2 = _mm_add_epi32(emm2, *(VectorRegisterInt*)_pi32_one);
		emm2 = _mm_and_si128(emm2, *(VectorRegisterInt*)_pi32_inv_one);
		y = _mm_cvtepi32_ps(emm2);

		emm4 = emm2;

		/* get the swap sign flag for the sine */
		emm0 = _mm_and_si128(emm2, *(VectorRegisterInt*)_pi32_four);
		emm0 = _mm_slli_epi32(emm0, 29);
		VectorRegister swap_sign_bit_sin = _mm_castsi128_ps(emm0);

		/* get the polynom selection mask for the sine*/
		emm2 = _mm_and_si128(emm2, *(VectorRegisterInt*)_pi32_two);
		emm2 = _mm_cmpeq_epi32(emm2, _mm_setzero_si128());
		VectorRegister poly_mask = _mm_castsi128_ps(emm2);


		/* The magic pass: "Extended precision modular arithmetic"
		   x = ((x - y * DP1) - y * DP2) - y * DP3; */
		xmm1 = *(VectorRegister*)_ps_minus_cephes_DP1;
		xmm2 = *(VectorRegister*)_ps_minus_cephes_DP2;
		xmm3 = *(VectorRegister*)_ps_minus_cephes_DP3;
		xmm1 = _mm_mul_ps(y, xmm1);
		xmm2 = _mm_mul_ps(y, xmm2);
		xmm3 = _mm_mul_ps(y, xmm3);
		x = _mm_add_ps(x, xmm1);
		x = _mm_add_ps(x, xmm2);
		x = _mm_add_ps(x, xmm3);

		emm4 = _mm_sub_epi32(emm4, *(VectorRegisterInt*)_pi32_two);
		emm4 = _mm_andnot_si128(emm4, *(VectorRegisterInt*)_pi32_four);
		emm4 = _mm_slli_epi32(emm4, 29);
		VectorRegister sign_bit_cos = _mm_castsi128_ps(emm4);


		sign_bit_sin = _mm_xor_ps(sign_bit_sin, swap_sign_bit_sin);


		/* Evaluate the first polynom  (0 <= x <= Pi/4) */
		VectorRegister z = _mm_mul_ps(x, x);
		y = *(VectorRegister*)_ps_coscof_p0;

		y = _mm_mul_ps(y, z);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_coscof_p1);
		y = _mm_mul_ps(y, z);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_coscof_p2);
		y = _mm_mul_ps(y, z);
		y = _mm_mul_ps(y, z);
		VectorRegister tmp = _mm_mul_ps(z, *(VectorRegister*)_ps_half);
		y = _mm_sub_ps(y, tmp);
		y = _mm_add_ps(y, *(VectorRegister*)_ps_one);

		/* Evaluate the second polynom  (Pi/4 <= x <= 0) */

		VectorRegister y2 = *(VectorRegister*)_ps_sincof_p0;
		y2 = _mm_mul_ps(y2, z);
		y2 = _mm_add_ps(y2, *(VectorRegister*)_ps_sincof_p1);
		y2 = _mm_mul_ps(y2, z);
		y2 = _mm_add_ps(y2, *(VectorRegister*)_ps_sincof_p2);
		y2 = _mm_mul_ps(y2, z);
		y2 = _mm_mul_ps(y2, x);
		y2 = _mm_add_ps(y2, x);

		/* select the correct result from the two polynoms */
		xmm3 = poly_mask;
		VectorRegister ysin2 = _mm_and_ps(xmm3, y2);
		VectorRegister ysin1 = _mm_andnot_ps(xmm3, y);
		y2 = _mm_sub_ps(y2, ysin2);
		y = _mm_sub_ps(y, ysin1);

		xmm1 = _mm_add_ps(ysin1, ysin2);
		xmm2 = _mm_add_ps(y, y2);

		/* update the sign */
		*s = _mm_xor_ps(xmm1, sign_bit_sin);
		*c = _mm_xor_ps(xmm2, sign_bit_cos);
	}


#define SHUFFLEMASK(a0,a1,a2,a3) ( (a0) | ((a1)<<2) | ((a2)<<4) | ((a3) << 6) )

	FORCEINLINE VectorRegister MakeVectorRegister(uint32 x, uint32 y, uint32 z, uint32 w)
	{
		union { VectorRegister v; VectorRegisterInt i; } tmp;
		tmp.i = _mm_setr_epi32(x, y, z, w);
		return tmp.v;
	}

	FORCEINLINE VectorRegister MakeVectorRegister(float x, float y, float z, float w)
	{
		return _mm_setr_ps(x, y, z, w);
	}

	FORCEINLINE VectorRegisterInt MakeVectorRegisterInt(uint32 x, uint32 y, uint32 z, uint32 w)
	{
		return _mm_setr_epi32(x, y, z, w);
	}

	namespace SSEMathConstant
	{
#undef PI
#define PI						(3.1415926535897932f)
#define FLOAT_NON_FRACTIONAL    8388608.f
		static const VectorRegister FloatOne = _mm_setr_ps(1.0f, 1.0f, 1.0f, 1.0f);
		static const VectorRegister FloatZero = _mm_setzero_ps();
		static const VectorRegister FloatMinusOne = _mm_setr_ps(-1.0f, -1.0f, -1.0f, -1.0f);
		static const VectorRegister Float0001 = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);
		static const VectorRegister Float1000 = _mm_setr_ps(1.0f, 0.0f, 0.0f, 0.0f);
		static const VectorRegister SmallLengthThreshold = _mm_setr_ps(1.e-8f, 1.e-8f, 1.e-8f, 1.e-8f);
		static const VectorRegister FloatOneHundredth = _mm_setr_ps(0.01f, 0.01f, 0.01f, 0.01f);
		static const VectorRegister Float111_Minus1 = _mm_setr_ps(1.f, 1.f, 1.f, -1.f);
		static const VectorRegister FloatMinus1_111 = _mm_setr_ps(-1.f, 1.f, 1.f, 1.f);
		static const VectorRegister FloatOneHalf = _mm_setr_ps(0.5f, 0.5f, 0.5f, 0.5f);
		static const VectorRegister FloatMinusOneHalf = _mm_setr_ps(-0.5f, -0.5f, -0.5f, -0.5f);
		static const VectorRegister KindaSmallNumber = _mm_setr_ps(1.e-4f, 1.e-4f, 1.e-4f, 1.e-4f);
		static const VectorRegister SmallNumber = _mm_setr_ps(1.e-8f, 1.e-8f, 1.e-8f, 1.e-8f);
		static const VectorRegister ThreshQuatNormalized = _mm_setr_ps(0.01, 0.01, 0.01, 0.01);
		static const VectorRegister BigNumber = _mm_setr_ps(3.4e+38f, 3.4e+38f, 3.4e+38f, 3.4e+38f);

		static const VectorRegisterInt IntOne = _mm_setr_epi32(1, 1, 1, 1);
		static const VectorRegisterInt IntZero = _mm_setr_epi32(0, 0, 0, 0);
		static const VectorRegisterInt IntMinusOne = _mm_setr_epi32(-1, -1, -1, -1);

		/** This is to speed up Quaternion Inverse. Static variable to keep sign of inverse **/
		static const VectorRegister QINV_SIGN_MASK = _mm_setr_ps(-1.f, -1.f, -1.f, 1.f);

		static const VectorRegister QMULTI_SIGN_MASK0 = _mm_setr_ps(1.f, -1.f, 1.f, -1.f);
		static const VectorRegister QMULTI_SIGN_MASK1 = _mm_setr_ps(1.f, 1.f, -1.f, -1.f);
		static const VectorRegister QMULTI_SIGN_MASK2 = _mm_setr_ps(-1.f, 1.f, 1.f, -1.f);

		static const VectorRegister DEG_TO_RAD = _mm_setr_ps(PI / (180.f), PI / (180.f), PI / (180.f), PI / (180.f));
		static const VectorRegister DEG_TO_RAD_HALF = _mm_setr_ps((PI / 180.f) * 0.5f, (PI / 180.f) * 0.5f, (PI / 180.f) * 0.5f, (PI / 180.f) * 0.5f);
		static const VectorRegister RAD_TO_DEG = _mm_setr_ps((180.f) / PI, (180.f) / PI, (180.f) / PI, (180.f) / PI);

		/** Bitmask to AND out the XYZ components in a vector */
		static const VectorRegister XYZMask = MakeVectorRegister((uint32)0xffffffff, (uint32)0xffffffff, (uint32)0xffffffff, (uint32)0x00000000);

#define SIGN_BIT ((1 << 31))
		static const VectorRegister SignBit = MakeVectorRegister((uint32)SIGN_BIT, (uint32)SIGN_BIT, (uint32)SIGN_BIT, (uint32)SIGN_BIT);
		static const VectorRegister SignMask = MakeVectorRegister((uint32)(~SIGN_BIT), (uint32)(~SIGN_BIT), (uint32)(~SIGN_BIT), (uint32)(~SIGN_BIT));
		static const VectorRegisterInt IntSignBit = MakeVectorRegisterInt(SIGN_BIT, SIGN_BIT, SIGN_BIT, SIGN_BIT);
		static const VectorRegisterInt IntSignMask = MakeVectorRegisterInt((~SIGN_BIT), (~SIGN_BIT), (~SIGN_BIT), (~SIGN_BIT));
#undef SIGN_BIT

		static const VectorRegister AllMask = MakeVectorRegister(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
		static const VectorRegisterInt IntAllMask = MakeVectorRegisterInt(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);

		/** Vector full of positive infinity */
		static const VectorRegister FloatInfinity = MakeVectorRegister((uint32)0x7F800000, (uint32)0x7F800000, (uint32)0x7F800000, (uint32)0x7F800000);


		static const VectorRegister Pi = MakeVectorRegister(PI, PI, PI, PI);
		static const VectorRegister TwoPi = MakeVectorRegister(2.0f * PI, 2.0f * PI, 2.0f * PI, 2.0f * PI);
		static const VectorRegister PiByTwo = MakeVectorRegister(0.5f * PI, 0.5f * PI, 0.5f * PI, 0.5f * PI);
		static const VectorRegister PiByFour = MakeVectorRegister(0.25f * PI, 0.25f * PI, 0.25f * PI, 0.25f * PI);
		static const VectorRegister OneOverPi = MakeVectorRegister(1.0f / PI, 1.0f / PI, 1.0f / PI, 1.0f / PI);
		static const VectorRegister OneOverTwoPi = MakeVectorRegister(1.0f / (2.0f * PI), 1.0f / (2.0f * PI), 1.0f / (2.0f * PI), 1.0f / (2.0f * PI));

		static const VectorRegister Float255 = MakeVectorRegister(255.0f, 255.0f, 255.0f, 255.0f);
		static const VectorRegister Float127 = MakeVectorRegister(127.0f, 127.0f, 127.0f, 127.0f);
		static const VectorRegister FloatNeg127 = MakeVectorRegister(-127.0f, -127.0f, -127.0f, -127.0f);
		static const VectorRegister Float360 = MakeVectorRegister(360.f, 360.f, 360.f, 360.f);
		static const VectorRegister Float180 = MakeVectorRegister(180.f, 180.f, 180.f, 180.f);

		// All float numbers greater than or equal to this have no fractional value.
		static const VectorRegister FloatNonFractional = MakeVectorRegister(FLOAT_NON_FRACTIONAL, FLOAT_NON_FRACTIONAL, FLOAT_NON_FRACTIONAL, FLOAT_NON_FRACTIONAL);

		static const VectorRegister FloatTwo = MakeVectorRegister(2.0f, 2.0f, 2.0f, 2.0f);
		static const uint32 AlmostTwoBits = 0x3fffffff;
		static const VectorRegister FloatAlmostTwo = MakeVectorRegister(*(float*)&AlmostTwoBits, *(float*)&AlmostTwoBits, *(float*)&AlmostTwoBits, *(float*)&AlmostTwoBits);
	}


	FORCEINLINE VectorRegister VectorZero()
	{
		return _mm_setzero_ps();
	}

	FORCEINLINE VectorRegister VectorOne()
	{
		return SSEMathConstant::FloatOne;
	}

	FORCEINLINE float VectorGetComponent(VectorRegister Vec, uint32 Index)
	{
		return ((float*)(&Vec))[Index];
	}

	FORCEINLINE VectorRegister VectorLoad(const void* ptr)
	{
		return _mm_loadu_ps((float*)ptr);
	}

#define VectorLoadFloat3(ptr) MakeVectorRegister( ((const float*)(ptr))[0], ((const float*)(ptr))[1], ((const float*)(ptr))[2], 0.0f )
#define VectorLoadFloat3_W0(ptr) VectorLoadFloat3(ptr)
#define VectorLoadFloat3_W1(ptr) MakeVectorRegister( ((const float*)(ptr))[0], ((const float*)(ptr))[1], ((const float*)(ptr))[2], 1.0f )

#define VectorLoadAligned(ptr) _mm_load_ps((const float*)(ptr))
#define VectorLoadFloat1(ptr) _mm_load1_ps((const float*)(ptr))
#define VectorLoadFloat2(ptr) _mm_castpd_ps(mm_load1_pd((const float*)(ptr)))

#define VectorSetFloat3(x, y, z) MakeVectorRegister( x, y, z, 0.0f)
#define VectorSetFloat1(f) _mm_set1_ps(f)

	FORCEINLINE VectorRegister VectorSet(float x, float y, float z, float w)
	{
		return MakeVectorRegister(x, y, z, w);
	}

#define VectorStoreAligned(vec, ptr) _mm_store_ps( (float*)(ptr), vec )
#define VectorStoreAlignedStreamed(vec, ptr) _mm_stream_ps( (float*)(ptr), vec )

	FORCEINLINE void VectorStore(const VectorRegister& vec, void* ptr)
	{
		_mm_storeu_ps((float*)ptr, vec);
	}

	FORCEINLINE void VectorStoreFloat3(const VectorRegister& vec, void* ptr)
	{
		union { VectorRegister v; float f[4]; } tmp;
		tmp.v = vec;
		float* floatPtr = (float*)ptr;
		floatPtr[0] = tmp.f[0];
		floatPtr[1] = tmp.f[1];
		floatPtr[2] = tmp.f[2];
	}

#define VectorStoreFloat1(vec, ptr) _mm_store_ss((float*)(ptr), vec)

#define VectorReplicate(vec, idx) _mm_shuffle_ps(vec, vec, SHUFFLEMASK(idx, idx, idx, idx))

#define VectorAbs(vec) _mm_and_ps(vec, SSEMathConstant::SignMask)

#define VectorNegate(vec) _mm_sub_ps( _mm_setzero_ps(), vec )
	FORCEINLINE VectorRegister operator-(const VectorRegister& vec)
	{
		return VectorNegate(vec);
	}
#define VectorAdd(vec1, vec2) _mm_add_ps(vec1, vec2)
	FORCEINLINE VectorRegister operator+(const VectorRegister& vec1, const VectorRegister& vec2)
	{
		return VectorAdd(vec1, vec2);
	}
#define VectorSubtract(vec1, vec2) _mm_sub_ps(vec1, vec2)
	FORCEINLINE VectorRegister operator-(const VectorRegister& vec1, const VectorRegister& vec2)
	{
		return VectorSubtract(vec1, vec2);
	}
#define VectorMultiply(vec1, vec2) _mm_mul_ps(vec1, vec2)
	FORCEINLINE VectorRegister operator*(const VectorRegister& vec1, const VectorRegister& vec2)
	{
		return VectorMultiply(vec1, vec2);
	}
#define VectorMultiplyAdd(vec1, vec2, vec3) _mm_add_ps( _mm_mul_ps(vec1, vec2),vec3 )

	FORCEINLINE VectorRegister VectorDot3(const VectorRegister& vec1, const VectorRegister& vec2)
	{
		VectorRegister tmp = vec1 * vec2;
		return VectorReplicate(tmp, 1) + VectorReplicate(tmp, 2) + VectorReplicate(tmp, 0);
	}

	FORCEINLINE VectorRegister VectorDot4(const VectorRegister& vec1, const VectorRegister& vec2)
	{
		VectorRegister tmp = vec1 * vec2;
		VectorRegister tmp2 = _mm_shuffle_ps(tmp, tmp, SHUFFLEMASK(2, 3, 0, 1));
		tmp = tmp + tmp2;
		tmp2 = _mm_shuffle_ps(tmp, tmp, SHUFFLEMASK(1, 2, 3, 0));
		return tmp + tmp2;
	}

#define VectorCompareEQ(vec1, vec2) _mm_cmpeq_ps(vec1, vec2)
	FORCEINLINE VectorRegister operator==(const VectorRegister& vec1, const VectorRegister& vec2)
	{
		return VectorCompareEQ(vec1, vec2);
	}

#define VectorCompareNE(vec1, vec2) _mm_cmpneq_ps(vec1, vec2)
	FORCEINLINE VectorRegister operator!=(const VectorRegister& vec1, const VectorRegister& vec2)
	{
		return VectorCompareNE(vec1, vec2);
	}

#define VectorCompareGT(vec1, vec2) _mm_cmpgt_ps(vec1, vec2)
	FORCEINLINE VectorRegister operator>(const VectorRegister& vec1, const VectorRegister& vec2)
	{
		return VectorCompareGT(vec1, vec2);
	}

#define VectorCompareGE(vec1, vec2) _mm_cmpge_ps(vec1, vec2)
	FORCEINLINE VectorRegister operator>=(const VectorRegister& vec1, const VectorRegister& vec2)
	{
		return VectorCompareGE(vec1, vec2);
	}

#define VectorCompareLT(vec1, vec2) _mm_cmplt_ps(vec1, vec2)
	FORCEINLINE VectorRegister operator<(const VectorRegister& vec1, const VectorRegister& vec2)
	{
		return VectorCompareLT(vec1, vec2);
	}

#define VectorCompareLE(vec1, vec2) _mm_cmple_ps(vec1, vec2)
	FORCEINLINE VectorRegister operator<=(const VectorRegister& vec1, const VectorRegister& vec2)
	{
		return VectorCompareLE(vec1, vec2);
	}

	FORCEINLINE VectorRegister VectorSelect(const VectorRegister& mask, const VectorRegister& vec1, const VectorRegister& vec2)
	{
		return _mm_xor_ps(
			vec2, _mm_and_ps(
				mask, _mm_xor_ps(vec1, vec2)
			)
		);
	}

#define VectorBitwiseOr(vec1, vec2) _mm_or_ps(vec1, vec2)
	FORCEINLINE VectorRegister operator|(const VectorRegister& vec1, const VectorRegister& vec2)
	{
		return VectorBitwiseOr(vec1, vec2);
	}

#define VectorBitwiseAnd(vec1, vec2) _mm_and_ps(vec1, vec2)
	FORCEINLINE VectorRegister operator&(const VectorRegister& vec1, const VectorRegister& vec2)
	{
		return VectorBitwiseAnd(vec1, vec2);
	}

#define VectorBitwiseXor(vec1, vec2) _mm_xor_ps(vec1, vec2)
	FORCEINLINE VectorRegister operator^(const VectorRegister& vec1, const VectorRegister& vec2)
	{
		return VectorBitwiseXor(vec1, vec2);
	}

	FORCEINLINE VectorRegister VectorCross(const VectorRegister& vec1, const VectorRegister& vec2)
	{
		VectorRegister A_YZXW = _mm_shuffle_ps(vec1, vec1, SHUFFLEMASK(1, 2, 0, 3));
		VectorRegister B_ZXYW = _mm_shuffle_ps(vec2, vec2, SHUFFLEMASK(2, 0, 1, 3));
		VectorRegister A_ZXYW = _mm_shuffle_ps(vec1, vec1, SHUFFLEMASK(2, 0, 1, 3));
		VectorRegister B_YZXW = _mm_shuffle_ps(vec2, vec2, SHUFFLEMASK(1, 2, 0, 3));

		return (A_YZXW * B_ZXYW) - (A_ZXYW * B_YZXW);
	}

	FORCEINLINE VectorRegister VectorPow(const VectorRegister& base, const VectorRegister& exponent)
	{
		// x ^ y = z
		// e^log(x)
		return SSE::SSEMath_exp_ps(SSE::SSEMath_log_ps(base) * exponent);
	}

#define VectorInverseSqrt(vec) _mm_rsqrt_ps(vec)

#define VectorReciprocal(vec) _mm_rcp_ps(vec)

	FORCEINLINE VectorRegister VectorReciprocalLen(const VectorRegister& vec)
	{
		VectorRegister recLen = VectorDot4(vec, vec);
		return VectorInverseSqrt(recLen);
	}

	FORCEINLINE VectorRegister VectorReciprocalSqrtAccurate(const VectorRegister& vec)
	{
		const VectorRegister oneHalf = SSEMathConstant::FloatOneHalf;
		const VectorRegister vecDivBy2 = vec * oneHalf;

		const VectorRegister x0 = VectorInverseSqrt(vec);
		VectorRegister x1 = x0 * x0;
		x1 = oneHalf - (vecDivBy2 * x1);
		x1 = VectorMultiplyAdd(x0, x1, x0);

		VectorRegister x2 = x1 * x1;
		x2 = oneHalf - (vecDivBy2 * x2);
		return VectorMultiplyAdd(x1, x2, x1);
	}

	FORCEINLINE VectorRegister VectorReciprocalAccurate(const VectorRegister& vec)
	{
		const VectorRegister x0 = VectorReciprocal(vec);

		const VectorRegister x0Sqr = x0 * x0;
		const VectorRegister x0Times2 = x0 + x0;
		const VectorRegister x1 = x0Times2 - (vec * x0Sqr);

		const VectorRegister x1Sqr = x1 * x1;
		const VectorRegister x1Times2 = x1 + x1;
		return x1Times2 - (vec * x1Sqr);
	}

	FORCEINLINE VectorRegister VectorNormalize(const VectorRegister& vec)
	{
		return vec * VectorReciprocalLen(vec);
	}

#define VectorSet_W0(vec) _mm_and_ps(vec, SSEMathConstant::XYZMask)
	FORCEINLINE VectorRegister VectorSet_W1(const VectorRegister& vec)
	{
		VectorRegister tmp = _mm_movehl_ps(VectorOne(), vec);
		return _mm_shuffle_ps(vec, tmp, SHUFFLEMASK(0, 1, 0, 3));
	}

	FORCEINLINE void MatrixMultiply(void* rs, const void* mat1, const void* mat2)
	{
		const VectorRegister* A = (const VectorRegister*)mat1;
		const VectorRegister* B = (const VectorRegister*)mat2;
		VectorRegister* R = (VectorRegister*)rs;
		VectorRegister tmp, r0, r1, r2, r3;

		tmp = VectorReplicate(A[0], 0) * B[0];
		tmp = VectorMultiplyAdd(VectorReplicate(A[0], 1), B[1], tmp);
		tmp = VectorMultiplyAdd(VectorReplicate(A[0], 2), B[2], tmp);
		r0 = VectorMultiplyAdd(VectorReplicate(A[0], 3), B[3], tmp);

		tmp = VectorReplicate(A[1], 0) * B[0];
		tmp = VectorMultiplyAdd(VectorReplicate(A[1], 1), B[1], tmp);
		tmp = VectorMultiplyAdd(VectorReplicate(A[1], 2), B[2], tmp);
		r1 = VectorMultiplyAdd(VectorReplicate(A[1], 3), B[3], tmp);

		tmp = VectorReplicate(A[2], 0) * B[0];
		tmp = VectorMultiplyAdd(VectorReplicate(A[2], 1), B[1], tmp);
		tmp = VectorMultiplyAdd(VectorReplicate(A[2], 2), B[2], tmp);
		r2 = VectorMultiplyAdd(VectorReplicate(A[2], 3), B[3], tmp);

		tmp = VectorReplicate(A[3], 0) * B[0];
		tmp = VectorMultiplyAdd(VectorReplicate(A[3], 1), B[1], tmp);
		tmp = VectorMultiplyAdd(VectorReplicate(A[3], 2), B[2], tmp);
		r3 = VectorMultiplyAdd(VectorReplicate(A[3], 3), B[3], tmp);


		R[0] = r0;
		R[1] = r1;
		R[2] = r2;
		R[3] = r3;
	}

	FORCEINLINE void MatrixInverse(void* dstMat, const void* mat)
	{
		typedef float Float4x4[4][4];
		const Float4x4& M = *((const Float4x4*)mat);
		Float4x4 rs;
		float det[4];
		Float4x4 tmp;

		tmp[0][0] = M[2][2] * M[3][3] - M[2][3] * M[3][2];
		tmp[0][1] = M[1][2] * M[3][3] - M[1][3] * M[3][2];
		tmp[0][2] = M[1][2] * M[2][3] - M[1][3] * M[2][2];

		tmp[1][0] = M[2][2] * M[3][3] - M[2][3] * M[3][2];
		tmp[1][1] = M[0][2] * M[3][3] - M[0][3] * M[3][2];
		tmp[1][2] = M[0][2] * M[2][3] - M[0][3] * M[2][2];

		tmp[2][0] = M[1][2] * M[3][3] - M[1][3] * M[3][2];
		tmp[2][1] = M[0][2] * M[3][3] - M[0][3] * M[3][2];
		tmp[2][2] = M[0][2] * M[1][3] - M[0][3] * M[1][2];

		tmp[3][0] = M[1][2] * M[2][3] - M[1][3] * M[2][2];
		tmp[3][1] = M[0][2] * M[2][3] - M[0][3] * M[2][2];
		tmp[3][2] = M[0][2] * M[1][3] - M[0][3] * M[1][2];

		det[0] = M[1][1] * tmp[0][0] - M[2][1] * tmp[0][1] + M[3][1] * tmp[0][2];
		det[1] = M[0][1] * tmp[1][0] - M[2][1] * tmp[1][1] + M[3][1] * tmp[1][2];
		det[2] = M[0][1] * tmp[2][0] - M[1][1] * tmp[2][1] + M[3][1] * tmp[2][2];
		det[3] = M[0][1] * tmp[3][0] - M[1][1] * tmp[3][1] + M[2][1] * tmp[3][2];

		float Determinant = M[0][0] * det[0] - M[1][0] * det[1] + M[2][0] * det[2] - M[3][0] * det[3];
		const float	RDet = 1.0f / Determinant;

		rs[0][0] = RDet * det[0];
		rs[0][1] = -RDet * det[1];
		rs[0][2] = RDet * det[2];
		rs[0][3] = -RDet * det[3];
		rs[1][0] = -RDet * (M[1][0] * tmp[0][0] - M[2][0] * tmp[0][1] + M[3][0] * tmp[0][2]);
		rs[1][1] = RDet * (M[0][0] * tmp[1][0] - M[2][0] * tmp[1][1] + M[3][0] * tmp[1][2]);
		rs[1][2] = -RDet * (M[0][0] * tmp[2][0] - M[1][0] * tmp[2][1] + M[3][0] * tmp[2][2]);
		rs[1][3] = RDet * (M[0][0] * tmp[3][0] - M[1][0] * tmp[3][1] + M[2][0] * tmp[3][2]);
		rs[2][0] = RDet * (
			M[1][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) -
			M[2][0] * (M[1][1] * M[3][3] - M[1][3] * M[3][1]) +
			M[3][0] * (M[1][1] * M[2][3] - M[1][3] * M[2][1])
			);
		rs[2][1] = -RDet * (
			M[0][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) -
			M[2][0] * (M[0][1] * M[3][3] - M[0][3] * M[3][1]) +
			M[3][0] * (M[0][1] * M[2][3] - M[0][3] * M[2][1])
			);
		rs[2][2] = RDet * (
			M[0][0] * (M[1][1] * M[3][3] - M[1][3] * M[3][1]) -
			M[1][0] * (M[0][1] * M[3][3] - M[0][3] * M[3][1]) +
			M[3][0] * (M[0][1] * M[1][3] - M[0][3] * M[1][1])
			);
		rs[2][3] = -RDet * (
			M[0][0] * (M[1][1] * M[2][3] - M[1][3] * M[2][1]) -
			M[1][0] * (M[0][1] * M[2][3] - M[0][3] * M[2][1]) +
			M[2][0] * (M[0][1] * M[1][3] - M[0][3] * M[1][1])
			);
		rs[3][0] = -RDet * (
			M[1][0] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]) -
			M[2][0] * (M[1][1] * M[3][2] - M[1][2] * M[3][1]) +
			M[3][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1])
			);
		rs[3][1] = RDet * (
			M[0][0] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]) -
			M[2][0] * (M[0][1] * M[3][2] - M[0][2] * M[3][1]) +
			M[3][0] * (M[0][1] * M[2][2] - M[0][2] * M[2][1])
			);
		rs[3][2] = -RDet * (
			M[0][0] * (M[1][1] * M[3][2] - M[1][2] * M[3][1]) -
			M[1][0] * (M[0][1] * M[3][2] - M[0][2] * M[3][1]) +
			M[3][0] * (M[0][1] * M[1][2] - M[0][2] * M[1][1])
			);
		rs[3][3] = RDet * (
			M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1]) -
			M[1][0] * (M[0][1] * M[2][2] - M[0][2] * M[2][1]) +
			M[2][0] * (M[0][1] * M[1][2] - M[0][2] * M[1][1])
			);

		memcpy(dstMat, &rs, 16 * sizeof(float));
	}

	FORCEINLINE VectorRegister VectorTransformVector(const VectorRegister& vec, const void* mat)
	{
		const VectorRegister* M = (const VectorRegister*)mat;
		VectorRegister vtmpX, vtmpY, vtmpZ, vtmpW;

		vtmpX = VectorReplicate(vec, 0) * M[0];
		vtmpY = VectorReplicate(vec, 1) * M[1];
		vtmpZ = VectorReplicate(vec, 2) * M[2];
		vtmpW = VectorReplicate(vec, 3) * M[3];

		vtmpX = vtmpX + vtmpY;
		vtmpZ = vtmpZ + vtmpW;
		return vtmpX + vtmpZ;
	}

#define VectorMin(vec1, vec2) _mm_min_ps(vec1, vec2)
#define VectorMax(vec1, vec2) _mm_max_ps(vec1, vec2)
#define VectorSwizzle(vec, x, y, z, w) _mm_shuffle_ps(vec, vec, SHUFFLEMASK(x,y,z,w))
#define VectorShuffle(vec1, vec2, x, y, z, w) _mm_shuffle_ps(vec1, vec2, SHUFFLEMASK(x,y,z,w))

	FORCEINLINE VectorRegister VectorCombineHigh(const VectorRegister& vec1, const VectorRegister& vec2)
	{
		return VectorShuffle(vec1, vec2, 2, 3, 2, 3);
	}

	FORCEINLINE VectorRegister VectorCombineLow(const VectorRegister& vec1, const VectorRegister& vec2)
	{
		return VectorShuffle(vec1, vec2, 0, 1, 0, 1);
	}

#define VectorMaskBits(vec) _mm_movemask_ps(vec)

	
#define	VectorDivide(vec1,vec2) _mm_div_ps(vec1, vec2)
	FORCEINLINE VectorRegister operator/(const VectorRegister& vec1, const VectorRegister& vec2)
	{
		return VectorDivide(vec1, vec2);
	}

#define VectorMergeVectorXYZ_VecW(vecXYZ, vecW) VectorSelect( XYZMask, vecXYZ, vecW )
#define VectorLoadByte4(ptr) _mm_cvtepi32_ps(_mm_unpacklo_epi16(_mm_unpacklo_epi8(_mm_cvtsi32_si128(*(int32*)ptr), _mm_setzero_si128()), _mm_setzero_si128()))

	FORCEINLINE VectorRegister VectorLoadSignedByte4(const void* ptr)
	{
		auto Temp = _mm_unpacklo_epi16(_mm_unpacklo_epi8(_mm_cvtsi32_si128(*(int32*)ptr), _mm_setzero_si128()), _mm_setzero_si128());
		auto Mask = _mm_cmpgt_epi32(Temp, _mm_set1_epi32(127));
		auto Comp = _mm_and_si128(Mask, _mm_set1_epi32(~127));
		return _mm_cvtepi32_ps(_mm_or_si128(Comp, Temp));
	}

	FORCEINLINE VectorRegister VectorLoadByte4Reverse(void* Ptr)
	{
		VectorRegister Temp = VectorLoadByte4(Ptr);
		return _mm_shuffle_ps(Temp, Temp, SHUFFLEMASK(3, 2, 1, 0));
	}

	FORCEINLINE void VectorStoreByte4(const VectorRegister& Vec, void* Ptr)
	{
		// Looks complex but is really quite straightforward:
		// Convert 4x floats to 4x 32-bit ints, then pack into 4x 16-bit ints, then into 4x 8-bit unsigned ints, then store as a 32-bit value
		*(int32*)Ptr = _mm_cvtsi128_si32(_mm_packus_epi16(_mm_packs_epi32(_mm_cvttps_epi32(Vec), _mm_setzero_si128()), _mm_setzero_si128()));
	}

	FORCEINLINE VectorRegister VectorLoadURGB10A2N(void* Ptr)
	{
		VectorRegister Tmp;

		Tmp = _mm_and_ps(_mm_load_ps1((const float*)Ptr), MakeVectorRegister(0x3FFu, 0x3FFu << 10, 0x3FFu << 20, 0x3u << 30));
		Tmp = _mm_xor_ps(Tmp, VectorSet(0, 0, 0, 0x80000000));
		Tmp = _mm_cvtepi32_ps(*(const VectorRegisterInt*)&Tmp);
		Tmp = _mm_add_ps(Tmp, VectorSet(0, 0, 0, 32768.0f * 65536.0f));
		Tmp = _mm_mul_ps(Tmp, VectorSet(1.0f / 1023.0f, 1.0f / (1023.0f * 1024.0f), 1.0f / (1023.0f * 1024.0f * 1024.0f), 1.0f / (3.0f * 1024.0f * 1024.0f * 1024.0f)));

		return Tmp;
	}

	FORCEINLINE void VectorStoreURGB10A2N(const VectorRegister& Vec, void* Ptr)
	{
		VectorRegister Tmp;
		Tmp = _mm_max_ps(Vec, MakeVectorRegister(0.0f, 0.0f, 0.0f, 0.0f));
		Tmp = _mm_min_ps(Tmp, MakeVectorRegister(1.0f, 1.0f, 1.0f, 1.0f));
		Tmp = _mm_mul_ps(Tmp, MakeVectorRegister(1023.0f, 1023.0f * 1024.0f * 0.5f, 1023.0f * 1024.0f * 1024.0f, 3.0f * 1024.0f * 1024.0f * 1024.0f * 0.5f));

		VectorRegisterInt TmpI;
		TmpI = _mm_cvttps_epi32(Tmp);
		TmpI = _mm_and_si128(TmpI, MakeVectorRegisterInt(0x3FFu, 0x3FFu << (10 - 1), 0x3FFu << 20, 0x3u << (30 - 1)));

		VectorRegisterInt TmpI2;
		TmpI2 = _mm_shuffle_epi32(TmpI, _MM_SHUFFLE(3, 2, 3, 2));
		TmpI = _mm_or_si128(TmpI, TmpI2);

		TmpI2 = _mm_shuffle_epi32(TmpI, _MM_SHUFFLE(1, 1, 1, 1));
		TmpI2 = _mm_add_epi32(TmpI2, TmpI2);
		TmpI = _mm_or_si128(TmpI, TmpI2);

		_mm_store_ss((float*)Ptr, *(const VectorRegister*)&TmpI);
	}

	#define VectorLoadURGBA16N( Ptr ) _mm_cvtepi32_ps(_mm_unpacklo_epi16(_mm_loadl_epi64((const __m128i*)Ptr), _mm_setzero_si128()))

	FORCEINLINE VectorRegister VectorLoadSRGBA16N(const void* Ptr)
	{
		auto Temp = _mm_unpacklo_epi16(_mm_loadl_epi64((const __m128i*)Ptr), _mm_setzero_si128());
		auto Mask = _mm_cmpgt_epi32(Temp, _mm_set1_epi32(32767));
		auto Comp = _mm_and_si128(Mask, _mm_set1_epi32(~32767));
		return _mm_cvtepi32_ps(_mm_or_si128(Comp, Temp));
	}

	FORCEINLINE void VectorStoreURGBA16N(const VectorRegister& Vec, void* Ptr)
	{

		VectorRegister Tmp;
		Tmp = _mm_max_ps(Vec, MakeVectorRegister(0.0f, 0.0f, 0.0f, 0.0f));
		Tmp = _mm_min_ps(Tmp, MakeVectorRegister(1.0f, 1.0f, 1.0f, 1.0f));
		Tmp = _mm_mul_ps(Tmp, MakeVectorRegister(65535.0f, 65535.0f, 65535.0f, 65535.0f));

		VectorRegisterInt TmpI = _mm_cvtps_epi32(Tmp);

		uint16* Out = (uint16*)Ptr;
		Out[0] = static_cast<int16>(_mm_extract_epi16(TmpI, 0));
		Out[1] = static_cast<int16>(_mm_extract_epi16(TmpI, 2));
		Out[2] = static_cast<int16>(_mm_extract_epi16(TmpI, 4));
		Out[3] = static_cast<int16>(_mm_extract_epi16(TmpI, 6));
	}

#define VectorAnyGreaterThan( Vec1, Vec2 )		_mm_movemask_ps( _mm_cmpgt_ps(Vec1, Vec2) )
#define VectorResetFloatRegisters()
#define VectorGetControlRegister()		_mm_getcsr()
#define	VectorSetControlRegister(ControlStatus) _mm_setcsr( ControlStatus )
#define VECTOR_ROUND_TOWARD_ZERO		_MM_ROUND_TOWARD_ZERO
	FORCEINLINE VectorRegister VectorQuaternionMultiply2(const VectorRegister& Quat1, const VectorRegister& Quat2)
	{
		VectorRegister Result = VectorMultiply(VectorReplicate(Quat1, 3), Quat2);
		Result = VectorMultiplyAdd(VectorMultiply(VectorReplicate(Quat1, 0), VectorSwizzle(Quat2, 3, 2, 1, 0)), SSEMathConstant::QMULTI_SIGN_MASK0, Result);
		Result = VectorMultiplyAdd(VectorMultiply(VectorReplicate(Quat1, 1), VectorSwizzle(Quat2, 2, 3, 0, 1)), SSEMathConstant::QMULTI_SIGN_MASK1, Result);
		Result = VectorMultiplyAdd(VectorMultiply(VectorReplicate(Quat1, 2), VectorSwizzle(Quat2, 1, 0, 3, 2)), SSEMathConstant::QMULTI_SIGN_MASK2, Result);

		return Result;
	}

	FORCEINLINE void VectorQuaternionMultiply(void* __restrict Result, const void* __restrict Quat1, const void* __restrict Quat2)
	{
		*((VectorRegister*)Result) = VectorQuaternionMultiply2(*((const VectorRegister*)Quat1), *((const VectorRegister*)Quat2));
	}

	// Returns true if the vector contains a component that is either NAN or +/-infinite.
	FORCEINLINE bool VectorContainsNaNOrInfinite(const VectorRegister& Vec)
	{
		// https://en.wikipedia.org/wiki/IEEE_754-1985
		// Infinity is represented with all exponent bits set, with the correct sign bit.
		// NaN is represented with all exponent bits set, plus at least one fraction/significand bit set.
		// This means finite values will not have all exponent bits set, so check against those bits.

		union { float F; uint32 U; } InfUnion;
		InfUnion.U = 0x7F800000;
		const float Inf = InfUnion.F;
		const VectorRegister FloatInfinity = MakeVectorRegister(Inf, Inf, Inf, Inf);

		// Mask off Exponent
		VectorRegister ExpTest = Vec & FloatInfinity;
		// Compare to full exponent. If any are full exponent (not finite), the signs copied to the mask are non-zero, otherwise it's zero and finite.
		bool IsFinite = VectorMaskBits(ExpTest == FloatInfinity) == 0;
		return !IsFinite;
	}

	FORCEINLINE VectorRegister VectorTruncate(const VectorRegister& X)
	{
		return _mm_cvtepi32_ps(_mm_cvttps_epi32(X));
	}

	FORCEINLINE VectorRegister VectorFractional(const VectorRegister& X)
	{
		return X - VectorTruncate(X);
	}

	FORCEINLINE VectorRegister VectorCeil(const VectorRegister& X)
	{
		const VectorRegister Trunc = VectorTruncate(X);
		const VectorRegister Frac = X - Trunc;
		const VectorRegister FracMask = Frac > (SSEMathConstant::FloatZero);
		const VectorRegister Add = VectorSelect(FracMask, (SSEMathConstant::FloatOne), (SSEMathConstant::FloatZero));
		return Trunc + Add;
	}

	FORCEINLINE VectorRegister VectorFloor(const VectorRegister& X)
	{
		const VectorRegister Trunc = VectorTruncate(X);
		const VectorRegister Frac = X - Trunc;
		const VectorRegister FracMask = Frac < (SSEMathConstant::FloatZero);
		const VectorRegister Add = VectorSelect(FracMask, (SSEMathConstant::FloatMinusOne), (SSEMathConstant::FloatZero));
		return Trunc + Add;
	}

	FORCEINLINE VectorRegister VectorMod(const VectorRegister& X, const VectorRegister& Y)
	{
		VectorRegister Div = X / Y;
		// Floats where abs(f) >= 2^23 have no fractional portion, and larger values would overflow VectorTruncate.
		VectorRegister NoFractionMask = VectorAbs(Div) >= SSEMathConstant::FloatNonFractional;
		VectorRegister Temp = VectorSelect(NoFractionMask, Div, VectorTruncate(Div));
		VectorRegister Result = X - (Y * Temp);
		// Clamp to [-AbsY, AbsY] because of possible failures for very large numbers (>1e10) due to precision loss.
		VectorRegister AbsY = VectorAbs(Y);
		return VectorMax((-AbsY), VectorMin(Result, AbsY));
	}

	FORCEINLINE VectorRegister VectorSign(const VectorRegister& X)
	{
		VectorRegister Mask = X >= SSEMathConstant::FloatZero;
		return VectorSelect(Mask, (SSEMathConstant::FloatOne), (SSEMathConstant::FloatMinusOne));
	}

	FORCEINLINE VectorRegister VectorStep(const VectorRegister& X)
	{
		VectorRegister Mask = X >= (SSEMathConstant::FloatZero);
		return Mask & SSEMathConstant::FloatOne;
	}

	//TODO: Vectorize
	FORCEINLINE VectorRegister VectorExp(const VectorRegister& X)
	{
		return SSEMath_exp_ps(X);
	}

	//TODO: Vectorize
	FORCEINLINE VectorRegister VectorExp2(const VectorRegister& X);

	//TODO: Vectorize
	FORCEINLINE VectorRegister VectorLog(const VectorRegister& X)
	{
		return SSEMath_log_ps(X);
	}

	//TODO: Vectorize
	FORCEINLINE VectorRegister VectorLog2(const VectorRegister& X);

	namespace SSESinConstants
	{
		static const float p = 0.225f;
		// 16 * sqrt(0.225)
		static const float a = (16 * 0.47434164902525689979983403166491f);
		// (1-0.225) / sqrt(0.225)
		static const float b = (0.775 / 0.47434164902525689979983403166491f);

		static const VectorRegister A = MakeVectorRegister(a, a, a, a);
		static const VectorRegister B = MakeVectorRegister(b, b, b, b);
	}

	FORCEINLINE VectorRegister VectorSin(const VectorRegister& x)
	{
		//Sine approximation using a squared parabola restrained to f(0) = 0, f(PI) = 0, f(PI/2) = 1.
	//based on a good discussion here http://forum.devmaster.net/t/fast-and-accurate-sine-cosine/9648
	//After approx 2.5 million tests comparing to sin(): 
	//Average error of 0.000128
	//Max error of 0.001091
		VectorRegister y = VectorMultiply(x, SSEMathConstant::OneOverTwoPi);
		y = VectorSubtract(y, VectorFloor(VectorAdd(y, SSEMathConstant::FloatOneHalf)));
		y = VectorMultiply(SSESinConstants::A, VectorMultiply(y, VectorSubtract(SSEMathConstant::FloatOneHalf, VectorAbs(y))));
		return VectorMultiply(y, VectorAdd(SSESinConstants::B, VectorAbs(y)));
	}

	FORCEINLINE VectorRegister VectorCos(const VectorRegister& x)
	{
		return VectorSin(VectorAdd(x, SSEMathConstant::PiByTwo));
	}

	FORCEINLINE void VectorSinCos(VectorRegister* outSin, VectorRegister* outCos, const VectorRegister* inAngles)
	{
		// Map to [-pi, pi]
		// X = A - 2pi * round(A/2pi)
		// Note the round(), not truncate(). In this case round() can round halfway cases using round-to-nearest-even OR round-to-nearest.

		// Quotient = round(A/2pi)
		VectorRegister quotient = VectorMultiply(*inAngles, SSEMathConstant::OneOverTwoPi);
		quotient = _mm_cvtepi32_ps(_mm_cvtps_epi32(quotient));
		// X = A - 2pi * Quotient
		VectorRegister x = VectorSubtract(*inAngles, VectorMultiply(SSEMathConstant::TwoPi, quotient));

		// Map in [-pi/2,pi/2]
		VectorRegister sign = VectorBitwiseAnd(x, SSEMathConstant::SignBit);
		VectorRegister c = VectorBitwiseOr(SSEMathConstant::Pi, sign);  // pi when x >= 0, -pi when x < 0
		VectorRegister absx = VectorAbs(x);
		VectorRegister rflx = VectorSubtract(c, x);
		VectorRegister comp = VectorCompareGT(absx, SSEMathConstant::PiByTwo);
		x = VectorSelect(comp, rflx, x);
		sign = VectorSelect(comp, SSEMathConstant::FloatMinusOne, SSEMathConstant::FloatOne);

		const VectorRegister XSquared = VectorMultiply(x, x);
		// 11-degree minimax approximation
		//*ScalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;
		const VectorRegister SinCoeff0 = MakeVectorRegister(1.0f, -0.16666667f, 0.0083333310f, -0.00019840874f);
		const VectorRegister SinCoeff1 = MakeVectorRegister(2.7525562e-06f, -2.3889859e-08f, /*unused*/ 0.f, /*unused*/ 0.f);

		VectorRegister S;
		S = VectorReplicate(SinCoeff1, 1);
		S = VectorMultiplyAdd(XSquared, S, VectorReplicate(SinCoeff1, 0));
		S = VectorMultiplyAdd(XSquared, S, VectorReplicate(SinCoeff0, 3));
		S = VectorMultiplyAdd(XSquared, S, VectorReplicate(SinCoeff0, 2));
		S = VectorMultiplyAdd(XSquared, S, VectorReplicate(SinCoeff0, 1));
		S = VectorMultiplyAdd(XSquared, S, VectorReplicate(SinCoeff0, 0));
		*outSin = VectorMultiply(S, x);

		// 10-degree minimax approximation
		//*ScalarCos = sign * (((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f);
		const VectorRegister CosCoeff0 = MakeVectorRegister(1.0f, -0.5f, 0.041666638f, -0.0013888378f);
		const VectorRegister CosCoeff1 = MakeVectorRegister(2.4760495e-05f, -2.6051615e-07f, /*unused*/ 0.f, /*unused*/ 0.f);

		VectorRegister C;
		C = VectorReplicate(CosCoeff1, 1);
		C = VectorMultiplyAdd(XSquared, C, VectorReplicate(CosCoeff1, 0));
		C = VectorMultiplyAdd(XSquared, C, VectorReplicate(CosCoeff0, 3));
		C = VectorMultiplyAdd(XSquared, C, VectorReplicate(CosCoeff0, 2));
		C = VectorMultiplyAdd(XSquared, C, VectorReplicate(CosCoeff0, 1));
		C = VectorMultiplyAdd(XSquared, C, VectorReplicate(CosCoeff0, 0));
		*outCos = VectorMultiply(C, sign);
	}

	FORCEINLINE VectorRegister VectorTan(const VectorRegister& x)
	{
		VectorRegister s, c;
		VectorSinCos(&s, &c, &x);
		return s / c;
	}

	FORCEINLINE VectorRegister VectorCot(const VectorRegister& x)
	{
		VectorRegister s, c;
		VectorSinCos(&s, &c, &x);
		return c / s;
	}

	//TODO: Vectorize
	FORCEINLINE VectorRegister VectorASin(const VectorRegister& X)
	{
		return MakeVectorRegister(GMath::Asin(VectorGetComponent(X, 0)), GMath::Asin(VectorGetComponent(X, 1)), GMath::Asin(VectorGetComponent(X, 2)), GMath::Asin(VectorGetComponent(X, 3)));
	}

	//TODO: Vectorize
	FORCEINLINE VectorRegister VectorACos(const VectorRegister& X)
	{
		return MakeVectorRegister(GMath::Acos(VectorGetComponent(X, 0)), GMath::Acos(VectorGetComponent(X, 1)), GMath::Acos(VectorGetComponent(X, 2)), GMath::Acos(VectorGetComponent(X, 3)));
	}

	//TODO: Vectorize
	FORCEINLINE VectorRegister VectorATan(const VectorRegister& X)
	{
		return MakeVectorRegister(GMath::Atan(VectorGetComponent(X, 0)), GMath::Atan(VectorGetComponent(X, 1)), GMath::Atan(VectorGetComponent(X, 2)), GMath::Atan(VectorGetComponent(X, 3)));
	}

	//TODO: Vectorize
	FORCEINLINE VectorRegister VectorATan2(const VectorRegister& X, const VectorRegister& Y)
	{
		return MakeVectorRegister(GMath::Atan2(VectorGetComponent(X, 0), VectorGetComponent(Y, 0)),
			GMath::Atan2(VectorGetComponent(X, 1), VectorGetComponent(Y, 1)),
			GMath::Atan2(VectorGetComponent(X, 2), VectorGetComponent(Y, 2)),
			GMath::Atan2(VectorGetComponent(X, 3), VectorGetComponent(Y, 3)));
	}


	FORCEINLINE bool VectorIsAligned(const void* ptr)
	{
		return !(PTRINT(ptr) & (SIMD_ALIGNMENT - 1));
	}

	FORCEINLINE VectorRegister VectorNormalizeAccurate(const VectorRegister& v)
	{
		const VectorRegister sqrSum = VectorDot4(v, v);
		const VectorRegister invLen = VectorReciprocalSqrtAccurate(sqrSum);
		return v * invLen;
	}

	FORCEINLINE VectorRegister VectorNormalizeSafe(const VectorRegister& v, const VectorRegister& defaultValue)
	{
		VectorRegister rs = VectorDot4(v, v);
		const VectorRegister NonZeroMask = rs >= SSEMathConstant::SmallLengthThreshold;
		const VectorRegister invLen = VectorReciprocalSqrtAccurate(rs);
		rs = v * invLen;
		return VectorSelect(NonZeroMask, rs, defaultValue);
	}

	/// <returns>non zero if any elements in v1 are less than in v2, otherwise 0</returns>
	FORCEINLINE uint32 VectorAnyLessThan(const VectorRegister& v1, const VectorRegister& v2)
	{
		return VectorAnyGreaterThan(v2, v1);
	}

	/// <returns>non zero if all elements in v1 are greater than in v2, otherwise 0</returns>
	FORCEINLINE uint32 VectorAllGreaterThan(const VectorRegister& v1, const VectorRegister& v2)
	{
		return !VectorAnyGreaterThan(v2, v1);
	}

	/// <returns>non zero if all elements in v1 are less than in v2, otherwise 0</returns>
	FORCEINLINE uint32 VectorAllLessThan(const VectorRegister& v1, const VectorRegister& v2)
	{
		return !VectorAnyGreaterThan(v1, v2);
	}
}

/// <returns>smaller value of two vectors each component</returns>
template<>
FORCEINLINE static VectorRegister GMath::Min(const VectorRegister a, const VectorRegister b)
{
	return VectorMin(a, b);
}

/// <returns>larger value of two vectors each component</returns>
template<>
FORCEINLINE static VectorRegister GMath::Max(const VectorRegister a, const VectorRegister b)
{
	return VectorMax(a, b);
}

template<>
FORCEINLINE static VectorRegister GMath::Lerp(const VectorRegister& a, const VectorRegister& b, const VectorRegister& alpha)
{
	const VectorRegister delta = b - a;
	return VectorMultiplyAdd(alpha, delta, a);
}

/// <summary>
/// 
/// </summary>
/// <param name="a">quaternion</param>
/// <param name="b">quaternion</param>
/// <returns> a + ( |a*b| > 0 ? 1 : -1 ) * b </returns>
FORCEINLINE static VectorRegister VectorAccumulateQuaternionShortestPath(const VectorRegister& a, const VectorRegister& b)
{
using namespace SSE;
	const VectorRegister rotDot = VectorDot4(a, b);
	const VectorRegister quatRotDirMask = rotDot >= VectorZero();
	const VectorRegister NegativeB = VectorNegate(b);
	const VectorRegister BiasTimesB = SSE::VectorSelect(quatRotDirMask, b, NegativeB);
	return VectorAdd(a, BiasTimesB);
}

FORCEINLINE static VectorRegister VectorNormalizeQuaternion(const VectorRegister& quat)
{
	return SSE::VectorNormalizeSafe(quat, SSE::SSEMathConstant::Float0001);
}

FORCEINLINE static VectorRegister VectorNormalizeRotator(const VectorRegister& rotator)
{
	using namespace SSE;
	VectorRegister v0 = VectorMod(rotator, SSEMathConstant::Float360);
	VectorRegister v1 = VectorAdd(v0, SSEMathConstant::Float360);
	VectorRegister v2 = VectorSelect(VectorCompareGE(v0, VectorZero()), v0, v1);

	VectorRegister v3 = VectorSubtract(v2, SSEMathConstant::Float360);
	return VectorSelect(VectorCompareGT(v2, SSEMathConstant::Float180), v3, v2);
}

/**
 * Fast Linear Quaternion Interpolation for quaternions stored in VectorRegisters.
 * Result is NOT normalized.
 */
FORCEINLINE static VectorRegister VectorSlerp(const VectorRegister& a, const VectorRegister& b, const VectorRegister& alpha)
{
	using namespace SSE;
	// Blend rotation
	//     To ensure the 'shortest route', we make sure the dot product between the both rotations is positive.
	//     const float Bias = (|A.B| >= 0 ? 1 : -1)
	//     Rotation = (B * Alpha) + (A * (Bias * (1.f - Alpha)));
	const VectorRegister Zero = VectorZero();

	const VectorRegister OneMinusAlpha = VectorSubtract(VectorOne(), alpha);

	const VectorRegister RotationDot = VectorDot4(a, b);
	const VectorRegister QuatRotationDirMask = VectorCompareGE(RotationDot, Zero);
	const VectorRegister NegativeA = VectorNegate(a);
	const VectorRegister BiasTimesA = VectorSelect(QuatRotationDirMask, a, NegativeA);
	const VectorRegister BTimesWeight = VectorMultiply(b, alpha);
	const VectorRegister UnnormalizedResult = VectorMultiplyAdd(BiasTimesA, OneMinusAlpha, BTimesWeight);

	return UnnormalizedResult;
}

/**
 * Bi-Linear Quaternion interpolation for quaternions stored in VectorRegisters.
 * Result is NOT normalized.
 */
FORCEINLINE static VectorRegister VectorBiSlerp(const VectorRegister& P00, const VectorRegister& P10, const VectorRegister& P01, const VectorRegister& P11, const VectorRegister& FracX, const VectorRegister& FracY)
{
	return VectorSlerp(
		VectorSlerp(P00, P10, FracX),
		VectorSlerp(P01, P11, FracX),
		FracY);
}

/**
 * Inverse quaternion ( -X, -Y, -Z, W)
 */
FORCEINLINE static VectorRegister VectorQuaternionInverse(const VectorRegister& NormalizedQuat)
{
	return VectorMultiply(SSE::SSEMathConstant::QINV_SIGN_MASK, NormalizedQuat);
}

/**
 * Rotate a vector using a unit Quaternion.
 *
 * @param Quat Unit Quaternion to use for rotation.
 * @param VectorW0 Vector to rotate. W component must be zero.
 * @return Vector after rotation by Quat.
 */
FORCEINLINE static VectorRegister VectorQuaternionRotateVector(const VectorRegister& Quat, const VectorRegister& VectorW0)
{
	// Q * V * Q.Inverse
	//const VectorRegister InverseRotation = VectorQuaternionInverse(Quat);
	//const VectorRegister Temp = VectorQuaternionMultiply2(Quat, VectorW0);
	//const VectorRegister Rotated = VectorQuaternionMultiply2(Temp, InverseRotation);

	// Equivalence of above can be shown to be:
	// http://people.csail.mit.edu/bkph/articles/Quaternions.pdf
	// V' = V + 2w(Q x V) + (2Q x (Q x V))
	// refactor:
	// V' = V + w(2(Q x V)) + (Q x (2(Q x V)))
	// T = 2(Q x V);
	// V' = V + w*(T) + (Q x T)
	using namespace SSE;

	const VectorRegister QW = VectorReplicate(Quat, 3);
	VectorRegister T = VectorCross(Quat, VectorW0);
	T = VectorAdd(T, T);
	const VectorRegister VTemp0 = VectorMultiplyAdd(QW, T, VectorW0);
	const VectorRegister VTemp1 = VectorCross(Quat, T);
	const VectorRegister Rotated = VectorAdd(VTemp0, VTemp1);
	return Rotated;
}

/**
 * Rotate a vector using the inverse of a unit Quaternion (rotation in the opposite direction).
 *
 * @param Quat Unit Quaternion to use for rotation.
 * @param VectorW0 Vector to rotate. W component must be zero.
 * @return Vector after rotation by the inverse of Quat.
 */
FORCEINLINE static VectorRegister VectorQuaternionInverseRotateVector(const VectorRegister& Quat, const VectorRegister& VectorW0)
{
	// Q.Inverse * V * Q
	//const VectorRegister InverseRotation = VectorQuaternionInverse(Quat);
	//const VectorRegister Temp = VectorQuaternionMultiply2(InverseRotation, VectorW0);
	//const VectorRegister Rotated = VectorQuaternionMultiply2(Temp, Quat);

	const VectorRegister QInv = VectorQuaternionInverse(Quat);
	return VectorQuaternionRotateVector(QInv, VectorW0);
}

/**
* Rotate a vector using a pointer to a unit Quaternion.
*
* @param Result		Pointer to where the result should be stored
* @param Quat		Pointer to the unit quaternion (must not be the destination)
* @param VectorW0	Pointer to the vector (must not be the destination). W component must be zero.
*/
FORCEINLINE static void VectorQuaternionRotateVectorPtr(void* __restrict Result, const void* __restrict Quat, const void* __restrict VectorW0)
{
	*((VectorRegister*)Result) = VectorQuaternionRotateVector(*((const VectorRegister*)Quat), *((const VectorRegister*)VectorW0));
}

/**
* Rotate a vector using the inverse of a unit Quaternion (rotation in the opposite direction).
*
* @param Result		Pointer to where the result should be stored
* @param Quat		Pointer to the unit quaternion (must not be the destination)
* @param VectorW0	Pointer to the vector (must not be the destination). W component must be zero.
*/
FORCEINLINE void VectorQuaternionInverseRotateVectorPtr(void* __restrict Result, const void* __restrict Quat, const void* __restrict VectorW0)
{
	*((VectorRegister*)Result) = VectorQuaternionInverseRotateVector(*((const VectorRegister*)Quat), *((const VectorRegister*)VectorW0));
}

/**
 * Counts the number of trailing zeros in the bit representation of the value,
 * counting from least-significant bit to most.
 *
 * @param Value the value to determine the number of leading zeros for
 * @return the number of zeros before the first "on" bit
 */
#if defined(_MSC_VER)
#pragma intrinsic( _BitScanForward )
FORCEINLINE uint32 appCountTrailingZeros(uint32 Value)
{
	if (Value == 0)
	{
		return 32;
	}
	unsigned long BitIndex;	// 0-based, where the LSB is 0 and MSB is 31
	_BitScanForward(&BitIndex, Value);	// Scans from LSB to MSB
	return BitIndex;
}
#else // !defined(_MSC_VER)
FORCEINLINE uint32 appCountTrailingZeros(uint32 Value)
{
	if (Value == 0)
	{
		return 32;
	}
	return __builtin_ffs(Value) - 1;
}
#endif // _MSC_VER


FORCEINLINE float GMath::RSqrt(const float x)
{
	return SSE::rsqrt(x);
}
