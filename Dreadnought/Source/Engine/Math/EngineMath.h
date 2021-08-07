#pragma once
#include <cmath>
#include <assert.h>

namespace MathConstant
{
#undef PI
#define PI						(3.1415926535897932f)
#define TWO_PI					(6.2831853071795864f)
#define HALF_PI					(1.57079632679f)
#define INV_PI					(0.31830988618f)
#define INV_TWO_PI				(0.15915494309f)
#define BIG_NUMBER				(3.4e+38f)
#define SMALL_NUMBER			(1.0e-8f)
#define KINDA_SMALL_NUMBER		(1.0e-4f)
#define EULER_NUMBER			(2.71828182845904523536f)
#define GOLDEN_RATIO			(1.6180339887498948482045868343656381f)
#define MAX_INT_PART_FLOAT		(8388608.f)
#define SQRT_2					(1.4142135623730950488016887242097f)
#define SQRT_3					(1.7320508075688772935274463415059f)
#define INV_SQRT_2				(0.70710678118654752440084436210485f)
#define INV_SQRT_3				(0.57735026918962576450914878050196f)
#define HALF_SQRT_2				(0.70710678118654752440084436210485f)
#define HALF_SQRT_3				(0.86602540378443864676372317075294f)
};

namespace Threshold
{
#define DELTA					(0.00001f)
#define FLOAT_NORMAL_THRESH		(0.0001f)

	//
	// Magic numbers for numerical precision.
	//
#define THRESH_POINT_ON_PLANE			(0.10f)		/* Thickness of plane for front/back/inside test */
#define THRESH_POINT_ON_SIDE			(0.20f)		/* Thickness of polygon side's side-plane for point-inside/outside/on side test */
#define THRESH_POINTS_ARE_SAME			(0.00002f)	/* Two points are same if within this distance */
#define THRESH_POINTS_ARE_NEAR			(0.015f)	/* Two points are near if within this distance and can be combined if imprecise math is ok */
#define THRESH_NORMALS_ARE_SAME			(0.00002f)	/* Two normal points are same if within this distance */
#define THRESH_UVS_ARE_SAME			    (0.0009765625f)/* Two UV are same if within this threshold (1.0f/1024f) */
													/* Making this too large results in incorrect CSG classification and disaster */
#define THRESH_VECTORS_ARE_NEAR			(0.0004f)	/* Two vectors are near if within this distance and can be combined if imprecise math is ok */
													/* Making this too large results in lighting problems due to inaccurate texture coordinates */
#define THRESH_SPLIT_POLY_WITH_PLANE	(0.25f)		/* A plane splits a polygon in half */
#define THRESH_SPLIT_POLY_PRECISELY		(0.01f)		/* A plane exactly splits a polygon */
#define THRESH_ZERO_NORM_SQUARED		(0.0001f)	/* Size of a unit normal that is considered "zero", squared */
#define THRESH_NORMALS_ARE_PARALLEL		(0.999845f)	/* Two unit vectors are parallel if abs(A dot B) is greater than or equal to this. This is roughly cosine(1.0 degrees). */
#define THRESH_NORMALS_ARE_ORTHOGONAL	(0.017455f)	/* Two unit vectors are orthogonal (perpendicular) if abs(A dot B) is less than or equal this. This is roughly cosine(89.0 degrees). */

#define THRESH_VECTOR_NORMALIZED		(0.01f)		/** Allowed error for a normalized vector (against squared magnitude) */
#define THRESH_QUAT_NORMALIZED			(0.01f)		/** Allowed error for a normalized quaternion (against squared magnitude) */
};

struct _Vector2;
struct _Vector;
struct _Vector4;
struct _Rotator;
struct _Quaternion;
struct _Matrix;
//typedef _Vector2 float2;

typedef _Vector2 float2;
typedef _Vector float3;
typedef _Vector4 float4;

struct GMath
{
	template<typename T>
	FORCEINLINE static constexpr T Min(const T a, const T b)
	{
		return (a > b) ? (b) : (a);
	}

	template<typename T>
	FORCEINLINE static constexpr T Max(const T a,const T b)
	{
		return (a > b) ? (a) : (b);
	}

	template<typename T>
	FORCEINLINE static constexpr T Clamp(const T x, const T min, const T max)
	{
		return Min(Max(x, min), max);
	}
	
	template<typename T, typename U>
	FORCEINLINE static constexpr T Lerp(const T& A, const T& B, const U& Alpha)
	{
		return (T)(A+Alpha * (B-A));
	}


	template<typename U>
	FORCEINLINE static constexpr _Rotator Lerp(const _Rotator& A, const _Rotator& B, const U& Alpha);

	template<typename U>
	FORCEINLINE static constexpr _Rotator LerpRange(const _Rotator& A, const _Rotator& B, const U& Alpha);

	template<typename T>
	FORCEINLINE static constexpr bool IsNan(const T x)
	{
		return isnan(x);
	}

	template<typename T>
	FORCEINLINE static constexpr bool IsInf(const T x)
	{
		return std::isinf(x);
	}

	template<typename T>
	FORCEINLINE static constexpr bool IsFinite(const T x)
	{
		return isfinite(x);
	}

	template<typename T>
	FORCEINLINE static constexpr T Sqrt(const T x)
	{
		return std::sqrt(x);
	}

	template<typename T>
	FORCEINLINE static constexpr T Square(const T x)
	{
		return x * x;
	}

	template<typename T>
	FORCEINLINE static constexpr T Pow(const T x, const T exp)
	{
		return std::pow(x, exp);
	}

	template<typename T>
	FORCEINLINE static constexpr T Sin(const T x)
	{
		return std::sin(x);
	}

	template<typename T>
	FORCEINLINE static constexpr T Asin(const T x)
	{
		return std::asin(x);
	}

	template<typename T>
	FORCEINLINE static constexpr T Acos(const T x)
	{
		return std::acos(x);
	}

	/// Sine phase with frequency of 2pi
	template<typename T>
	FORCEINLINE static constexpr T Sin01(const T x)
	{
		return std::sin(TWO_PI * x);
	}

	template<typename T>
	FORCEINLINE static constexpr T Cos(const T x)
	{
		return std::cos(x);
	}

	/// Cosine phase with frequency of 2pi
	template<typename T>
	FORCEINLINE static constexpr T Cos01(const T x)
	{
		return std::cos(TWO_PI * x);
	}

	template<typename T>
	FORCEINLINE static constexpr T Floor(const T x)
	{
		return std::floor(x);
	}

	template<typename T>
	FORCEINLINE static constexpr T Ceil(const T x)
	{
		return std::ceil(x);
	}

	template<typename T>
	FORCEINLINE static constexpr T Frac(const T x)
	{
		return x - Floor(x);
	}

	/// Linearly estimated Sine phase (might slower than normal version on new devices)
	template<typename T>
	FORCEINLINE static constexpr T SinEst(const T x)
	{
		T p = Abs(2 * Frac(x * static_cast<T>(INV_TWO_PI) + static_cast<T>(0.75)) - 1);
		return (6-4*p)*p*p - 1;
	}

	/// Linearly estimated Cosine phase (might slower than normal version on new devices)
	template<typename T>
	FORCEINLINE static constexpr T CosEst(const T x)
	{
		T p = Abs(2 * Frac(x * static_cast<T>(INV_TWO_PI)) - 1);
		return (6 - 4 * p) * p * p - 1;
	}

	/// <summary>
	/// Estimated sin and cos phase
	/// </summary>
	/// <param name="outSin">sin result</param>
	/// <param name="outCos">cos result</param>
	/// <param name="x">input param</param>
	FORCEINLINE static constexpr void SinCos(float* outSin, float* outCos, float x)
	{
		float quotient = INV_TWO_PI * x;
		quotient = (float)((int)(x >= 0.0f ? quotient + 0.5f : quotient - 0.5f));
		float y = x - TWO_PI * quotient;
		float sign;
		if (y > HALF_PI)
		{
			y = PI - y;
			sign = -1;
		}
		else if (y < -HALF_PI)
		{
			y = -PI - y;
			sign = -1;
		}
		else
		{
			sign = 1;
		}
		float y2 = y * y;

		*outSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;
		float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
		*outCos = sign * p;
	}

	/// Linearly estimated Sine phase with frequency of 2pi (might slower than normal version on new devices)
	template<typename T>
	FORCEINLINE static constexpr T Sin01Est(const T x)
	{
		T p = Abs(2 * Frac(x + static_cast<T>(0.75)) - 1);
		return (6 - 4 * p) * p * p - 1;
	}

	/// Linearly estimated Cosine phase with frequency of 2pi (might slower than normal version on new devices)
	template<typename T>
	FORCEINLINE static constexpr T Cos01Est(const T x)
	{
		T p = Abs(2 * Frac(x) - 1);
		return (6 - 4 * p) * p * p - 1;
	}

	template<typename T>
	FORCEINLINE static constexpr T Tan(const T x)
	{
		return std::tan(x);
	}

	template<typename T>
	FORCEINLINE static constexpr T Cot(const T x)
	{
		return static_cast<T>(1) / std::tan(x);
	}

	template<typename T>
	FORCEINLINE static constexpr T Atan(const T x)
	{
		return std::atan(x);
	}

	template<typename T>
	FORCEINLINE static constexpr T Atan2(const T y, const T x)
	{
		return std::atan2(y, x);
	}

	FORCEINLINE static float RSqrt(const float x);

	template<typename T>
	FORCEINLINE static constexpr T Abs(const T x)
	{
		return std::abs(x);
	}

	template<typename T>
	FORCEINLINE static constexpr T Select(const T x, const T ge, const T lt)
	{
		return x >= 0 ? ge : lt;
	}

	template<typename T>
	FORCEINLINE static constexpr T Sign(const T x)
	{
		return Select(x, static_cast<T>(1), static_cast<T>(-1));
	}

	template<typename T>
	FORCEINLINE static constexpr T Saturate(const T x)
	{
		return Clamp(x, static_cast<T>(0), static_cast<T>(1));
	}

	static constexpr float Rad2Deg = 180.f / PI;
	static constexpr float Deg2Rad = PI / 180.f;

	template<typename T>
	FORCEINLINE static T RadiansToDegree(const T x)
	{
		return x * Rad2Deg;
	}

	template<typename T>
	FORCEINLINE static T DegreeToRadians(const T x)
	{
		return x * Deg2Rad;
	}

	template<typename T>
	FORCEINLINE static T Exp2(const T x)
	{
		return pow(static_cast<T>(2), x);
	}
	template<typename T>
	FORCEINLINE static T Ln(const T x)
	{
		return log(x);
	}

	template<typename T>
	FORCEINLINE static T Trunc(T x)
	{
		return trunc(x);
	}

	FORCEINLINE static float FMod(float X, float Y);

	template<typename T>
	FORCEINLINE static T LogX(const T base, const T value)
	{
		return log(value) / log(base);
	}

	template<typename T>
	FORCEINLINE static T Log2(const T x)
	{
		return log(x) * 1.442695f;
	}

	FORCEINLINE static uint32 RoundUp256(uint32 x)
	{
		return (x + 255) & ~255;
	}


#define FASTASIN_HALF_PI		(1.5707963050f)
	FORCEINLINE static float FastASin(float x)
	{
		bool bNonNegative = x >= 0.0f;
		x = Abs(x);
		float omX = 1 - x;
		if (omX < 0.0f) omX = 0.0f;
		float root = Sqrt(omX);
		float rs = ((((((-0.0012624911f * x + 0.0066700901f) * x - 0.0170881256f) * x + 0.0308918810f) * x - 0.0501743046f) * x + 0.0889789874f) * x - 0.2145988016f) * x + FASTASIN_HALF_PI;
		rs *= root;
		return bNonNegative ? FASTASIN_HALF_PI - rs : rs - FASTASIN_HALF_PI;
	}
#undef FASTASIN_HALF_PI

	static int32 TruncToInt(float f);

	static int32 RoundToInt(float f);

	static int32 FloorToInt(float f);

	static int32 CeilToInt(float f);
};
