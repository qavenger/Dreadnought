#pragma once
#include <EngineMath.h>
#include <Vector.h>
#include <Vector4D.h>
typedef struct _Vector2
{
	float x, y;
public:
	inline std::string ToString() const;
public:
	inline _Vector2();
	inline _Vector2(float f);
	inline _Vector2(float x, float y);
	inline _Vector2(const _Vector2& v);
	inline _Vector2(const _Vector& v);
	inline _Vector2(const _Vector4& v);
	inline void CheckNan() const
	{
#if CHECK_NAN
		/*if (HasNan())
		{
			*this = ZeroVector;
		}*/
#endif
	}

	// Vector Zero ( 0, 0 )
	static const _Vector2 ZeroVector;
	// One Vector ( 1, 1 )
	static const _Vector2 OneVector;
	// 45 degree vector (1/sqrt(2), 1/sqrt(2))
	static const _Vector2 Unit45DegVector;

	inline constexpr float LengthSquare()const
	{
		return x * x + y * y;
	}

	inline constexpr float Length()const
	{
		return GMath::Sqrt(x * x + y * y);
	}

	inline void SafeNormalize(float threshold = SMALL_NUMBER)
	{
		float len_sqr = LengthSquare();
		if (len_sqr != 1.0f)
		{
			if (len_sqr < threshold)
			{
				x = 0, y = 0;
			}
		}
		const float s = GMath::RSqrt(len_sqr);
		x *= s;
		y *= s;
	}

	inline float operator[](size_t Index)const
	{
#if defined(_DEBUG) || defined(DEBUG)
		assert(Index >= 0 && Index < 2);
#endif
		return (&x)[Index];
		return (&x)[Index];
	}
	inline float& operator[](size_t Index)
	{
#if defined(_DEBUG) || defined(DEBUG)
		assert(Index >= 0 && Index < 2);
#endif
		return (&x)[Index];
		return (&x)[Index];
	}
	inline void Set(float inX, float inY)
	{
		x = inX;
		y = inY;
	}

#define DECLARE_OP\
		OP(+=)\
		OP(-=)\
		OP(*=)\
		OP(/=)

#define OP(op) \
	inline _Vector2& operator##op(const _Vector2& V){\
		x op V.x;\
		y op V.y;\
		CheckNan();\
		return *this;}
	DECLARE_OP
#undef OP
#undef DECLARE_OP

#define DECLARE_OP\
		OP(+=)\
		OP(-=)\
		OP(*=)

#define OP(op) \
	inline _Vector2& operator##op(const float s){\
		x op s;\
		y op s;\
		CheckNan();\
		return *this;}
	DECLARE_OP
#undef OP
#undef DECLARE_OP
	inline _Vector2& operator/=(const float s) {
		float inv_s = 1.0f / s;
		x *= inv_s;	y *= inv_s;
		CheckNan();
		return *this;
	}


	inline void Negate()
	{
		x = -x;
		y = -y;
	}

	inline void Normalize()
	{
		float s = GMath::RSqrt(LengthSquare());
		x *= s;
		y *= s;
	}

	inline static _Vector2 Normalize(const _Vector2& v)
	{
		const float s = GMath::RSqrt(v.LengthSquare());
		return _Vector2(v.x * s, v.y * s);
	}

	inline static _Vector2 SafeNormalize(const _Vector2& v, float threshold = SMALL_NUMBER)
	{
		const float l = v.LengthSquare();
		if (l == 1.0f) return v;
		else if (l <= SMALL_NUMBER) return _Vector2::ZeroVector;
		const float s = GMath::RSqrt(l);
		return _Vector2(v.x * s, v.y * s);
	}

	inline constexpr float operator|(const _Vector2& V)const
	{
		return x * V.x + y * V.y;
	}

	inline float operator^(const _Vector2& V)const
	{
		// x			y		
		// vx			vy		
		// xvy - yvx
		return 	x * V.y - y * V.x;
	}

	inline _Vector2 operator-()const {
		return _Vector2(-x, -y);
	}

	inline constexpr float Dot(const _Vector2& V)const
	{
		return x * V.x + y * V.y;
	}

	inline float Cross(const _Vector2& V)const
	{
		return x * V.y - y * V.x;
	}

	inline void SetLength(float length)
	{
		if (length < KINDA_SMALL_NUMBER)
		{
			x = 0; y = 0;
		}
		const float len_sqr = LengthSquare();
		const float s = length * GMath::RSqrt(len_sqr);
		x *= s;
		y *= s;
	}

	/// <summary>
	/// Get a new vector with new length but pointed to the same direction as this
	/// </summary>
	/// <param name="length">target length</param>
	/// <returns>new vector</returns>
	inline _Vector2 GetWithLengthOf(float length)const
	{
		if (length < KINDA_SMALL_NUMBER)
		{
			return _Vector2(0.0f, 0.0f);
		}
		const float len_sqr = LengthSquare();
		const float s = length * GMath::RSqrt(len_sqr);
		return _Vector2(
			x * s,
			y * s
		);
	}

	inline constexpr void ClampLength(float length)
	{
		if (length < KINDA_SMALL_NUMBER)
		{
			x = 0; y = 0;
		}
		const float len_sqr = LengthSquare();
		if (len_sqr > length * length)
		{
			const float s = length * GMath::RSqrt(len_sqr);
			x *= s;
			y *= s;
		}
	}

	inline _Vector2 GetClampedToLength(float length)const
	{
		if (length < KINDA_SMALL_NUMBER)
		{
			return ZeroVector;
		}
		const float len_sqr = LengthSquare();
		if (len_sqr > length * length)
		{
			const float s = length * GMath::RSqrt(len_sqr);
			return _Vector2(
				x * s,
				y * s
			);
		}
		else
		{
			return *this;
		}
	}

	inline _Vector2 Reciprocal()const
	{
		return _Vector2(
			x != 0 ? 1.0f / x : BIG_NUMBER,
			y != 0 ? 1.0f / y : BIG_NUMBER
		);
	}

	inline constexpr bool IsZero()const
	{
		return x == 0.0f && y == 0.0f;
	}

	inline constexpr bool IsNearlyZero(float epsilon = 0.001)const
	{
		return GMath::Abs(x) <= epsilon && GMath::Abs(y) <= epsilon;
	}

	inline static _Vector2 Max(const _Vector2& v0, const _Vector2& v1)
	{
		return _Vector2( GMath::Max(v0.x, v1.x), GMath::Max(v0.y, v1.y) );
	}

	inline static _Vector2 Min(const _Vector2& v0, const _Vector2& v1)
	{
		return _Vector2(GMath::Min(v0.x, v1.x), GMath::Min(v0.y, v1.y));
	}

	inline constexpr void ToDirectionAndLength(_Vector2& outDir, float& outLength)const
	{
		outLength = Length();
		if (outLength > SMALL_NUMBER)
		{
			float inv_length = 1.0f / outLength;
			outDir = _Vector2(x * inv_length, y * inv_length);
		}
		else
		{
			outDir = ZeroVector;
		}
	}

	inline constexpr float GetMinComponent()const
	{
		return GMath::Min(x, y);
	}

	inline constexpr float GetMaxComponent()const
	{
		return GMath::Max(x, y);
	}

	inline constexpr float GetAbsMinComponent()const
	{
		return GMath::Min(GMath::Abs(x), GMath::Abs(y));
	}

	inline constexpr float GetAbsMaxComponent()const
	{
		return GMath::Max(GMath::Abs(x), GMath::Abs(y));
	}

	inline _Vector2 Min(const _Vector2& v)const
	{
		return _Vector2(GMath::Min(x, v.x), GMath::Min(y, v.y));
	}

	inline _Vector2 Max(const _Vector2& v)const
	{
		return _Vector2(GMath::Max(x, v.x), GMath::Max(y, v.y));
	}

	inline _Vector2 Abs()const
	{
		return _Vector2(GMath::Abs(x), GMath::Abs(y));
	}

	inline constexpr bool IsNormalized(float epsilon = THRESH_VECTOR_NORMALIZED)const
	{
		return (GMath::Abs(1 - LengthSquare()) < epsilon);
	}

	inline _Vector2 Sign()const
	{
		return _Vector2(GMath::Sign(x), GMath::Sign(y));
	}

	inline constexpr void Clamp(const _Vector2& minV, const _Vector2& maxV)
	{
		x = GMath::Clamp(x, minV.x, maxV.x);
		y = GMath::Clamp(y, minV.y, maxV.y);
	}

	inline constexpr void Saturate()
	{
		x = GMath::Saturate(x);
		y = GMath::Saturate(y);
	}

	inline static bool PointsAreClose(const _Vector2& p1, const _Vector2& p2, float threshold = THRESH_POINTS_ARE_SAME)
	{
		return
			GMath::Abs(p1.x - p2.x) < threshold &&
			GMath::Abs(p1.y - p2.y) < threshold;
	}

	inline constexpr float DistanceSquaredTo(const _Vector2& p)
	{
		return GMath::Square(p.x - x) + GMath::Square(p.y - y);
	}

	inline constexpr float DistanceTo(const _Vector2& p)
	{
		return GMath::Sqrt(DistanceSquaredTo(p));
	}

	inline static constexpr float DistanceSquared(const _Vector2& v1, const _Vector2& v2)
	{
		return GMath::Square(v2.x - v1.x) + GMath::Square(v2.y - v1.y);
	}

	inline static constexpr float Distance(const _Vector2& v1, const _Vector2& v2)
	{
		return GMath::Sqrt(DistanceSquared(v1, v2));
	}



	inline float HeadingAngle()const;

	inline _Vector SphericalToUnitCartesian()const;
	inline _Vector SphericalToUnitCartesianEst()const;
	inline bool HasNan()const { return GMath::IsInf(x) || GMath::IsInf(y); }
} _float2;


inline _Vector2::_Vector2(const _Vector4& v)
{
	x = v.x;
	y = v.y;
}

#define DECLARE_OP\
		OP(+)\
		OP(-)\
		OP(*)OP(/)

#define OP(op)\
inline _Vector2 operator##op(const _Vector2& A, const _Vector2& B){\
	_Vector2 rs(A);\
	rs##op##= B;\
	rs.CheckNan();\
	return rs;}

DECLARE_OP
#undef OP

#define OP(op)\
inline _Vector2 operator##op(const float s, const _Vector2& V){\
	_Vector2 rs( s op V.x, s op V.y );\
	rs.CheckNan(); return rs;}
DECLARE_OP
#undef OP
#undef DECLARE_OP

#define DECLARE_OP\
		OP(+)\
		OP(-)\
		OP(*)

#define OP(op)\
inline _Vector2 operator##op(const _Vector2& V, const float s){\
	_Vector2 rs( V.x op s, V.y op s );\
	rs.CheckNan(); return rs;}
DECLARE_OP
inline _Vector2 operator/(const _Vector2& V, const float s) {
	float inv_s = 1.0f / s;
	_Vector2 rs(V.x * inv_s, V.y * inv_s);
	rs.CheckNan(); return rs;
}
#undef OP
#undef DECLARE_OP

const _Vector2 _Vector2::ZeroVector(0);
const _Vector2 _Vector2::OneVector(1);
const _Vector2 _Vector2::Unit45DegVector(INV_SQRT_2, INV_SQRT_2);

inline _Vector2::_Vector2() 
{}

inline _Vector2::_Vector2(float f)
	:
	x(f), y(f)
{
	CheckNan();
}

inline _Vector2::_Vector2(float x, float y)
	:x(x), y(y)
{
	CheckNan();
}

inline _Vector2::_Vector2(const _Vector2& v)
	:x(v.x), y(v.y)
{
	CheckNan();
}


inline float _Vector2::HeadingAngle() const
{
	return GMath::Atan2(y, x);
}

inline _Vector4::_Vector4(const _Vector2& v, float z, float w)
	:x(v.x),y(v.y),z(z),w(w)
{
	CheckNan();
}

inline _Vector4::_Vector4(const _Vector2& v0, const _Vector2& v1)
	:x(v0.x),y(v0.y),z(v1.x),w(v1.y)
{
	CheckNan();
}

inline std::string _Vector2::ToString()const
{
	return FormatString("X=%3.3f Y=%3.3f", x, y);
}