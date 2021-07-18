#pragma once
#include<EngineMath.h>

_declspec(align(16)) typedef struct _Vector4
{
	float x, y, z, w;
public:
	FORCEINLINE std::string ToString()const
	{
		return FormatString("X=%3.3f Y=%3.3f Z=%3.3f W=%3.3f", x, y, z, w);
	}
public:
	explicit FORCEINLINE _Vector4(bool bAsPoint = true);
	explicit FORCEINLINE _Vector4(float x, float y, float z, float w = 1);
	FORCEINLINE _Vector4(const _Vector2& v, float z, float w);
	FORCEINLINE _Vector4(const _Vector2& v0, const _Vector2& v1);
	FORCEINLINE _Vector4(const _Vector& v, float w);
	FORCEINLINE _Vector4(float x, const _Vector& v);
	FORCEINLINE _Vector4(const _Vector4& v);
	FORCEINLINE _Vector4& operator=(const _Vector4& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.x;
		return *this;
	}
	FORCEINLINE _Vector4& operator=(_Vector4&& other)noexcept
	{
		x = std::move(other.x);
		y = std::move(other.y);
		z = std::move(other.z);
		w = std::move(other.x);
		return *this;
	}
	FORCEINLINE void CheckNan()const
	{
#if CHECK_NAN
		if (HasNan())
		{
			x = 0;
			y = 0;
			z = 0;
			w = 0;
		}
#endif
	}
	FORCEINLINE bool constexpr HasNan()
	{
		return GMath::IsInf(x) || GMath::IsInf(y) || GMath::IsInf(z) || GMath::IsInf(w);
	}

public:
	FORCEINLINE float& operator[](int32 Index) {
#if defined(_DEBUG) || defined(DEBUG)
		assert(Index >= 0 && Index < 4);
#endif
		return (&x)[Index];
	}
	FORCEINLINE float operator[](int32 Index)const
	{
#if defined(_DEBUG) || defined(DEBUG)
		assert(Index >= 0 && Index < 4);
#endif
		return (&x)[Index];
	}

	FORCEINLINE void Set(float inX, float inY, float inZ, float inW)
	{
		x = inX;
		y = inY;
		z = inZ;
		w = inW;
	}

	FORCEINLINE _Vector4 operator-()const;

#define DECLARE_OP\
		OP(+=)\
		OP(-=)\
		OP(*=)\
		OP(/=)

#define OP(op) \
	FORCEINLINE _Vector4& operator##op(const _Vector4& V){\
		x op V.x;\
		y op V.y;\
		z op V.z;\
		z op V.w;\
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
	FORCEINLINE _Vector4& operator##op(const float s){\
		x op s;\
		y op s;\
		z op s;\
		w op s;\
		CheckNan();\
		return *this;}
	DECLARE_OP
#undef OP
#undef DECLARE_OP
	FORCEINLINE _Vector4& operator/=(const float s) {
		float inv_s = 1.0f / s;
		x *= inv_s;	y *= inv_s;	z *= inv_s; w *= inv_s;
		CheckNan();
		return *this;
	}

	FORCEINLINE void Negate()
	{
		x = -x;
		y = -y;
		z = -z;
		w = -w;
	}

	FORCEINLINE constexpr bool operator==(const _Vector4& v)const
	{
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}

	FORCEINLINE constexpr bool operator!=(const _Vector4& v)const
	{
		return !(x == v.x && y == v.y && z == v.z && w == v.w);
	}

	FORCEINLINE constexpr bool IsEqual(const _Vector4& v, float epsilon = KINDA_SMALL_NUMBER)
	{
		return  GMath::Abs(x - v.x) <= epsilon &&
			GMath::Abs(y - v.y) <= epsilon &&
			GMath::Abs(z - v.z) <= epsilon &&
			GMath::Abs(w - v.w) <= epsilon;
	}

	FORCEINLINE _Vector4 operator^(const _Vector4& v)const
	{
		return _Vector4(
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x,
			0.0f
		);
	}

	FORCEINLINE constexpr float LengthSquare()const
	{
		return x * x + y * y + z * z + w * w;
	}

	FORCEINLINE constexpr float LengthSquare3D()const
	{
		return x * x + y * y + z * z;
	}

	FORCEINLINE constexpr float Length()const
	{
		return GMath::Sqrt(LengthSquare());
	}

	FORCEINLINE constexpr float Length3D()const
	{
		return GMath::Sqrt(LengthSquare3D());
	}

	FORCEINLINE void Normalize()
	{
		const float inv_len = GMath::RSqrt(LengthSquare());
		x *= inv_len;
		y *= inv_len;
		z *= inv_len;
		w *= inv_len;
	}

	FORCEINLINE void Normalize3D()
	{
		const float inv_len = GMath::RSqrt(LengthSquare3D());
		x *= inv_len;
		y *= inv_len;
		z *= inv_len;
		w = 0.0f;
	}

	FORCEINLINE void SafeNormalize(float epsilon = SMALL_NUMBER)
	{
		const float len_sqr = LengthSquare();
		if (len_sqr > epsilon)
		{
			const float inv_len = GMath::RSqrt(len_sqr);
			x *= inv_len;
			y *= inv_len;
			z *= inv_len;
			w *= inv_len;
			return;
		}
		x = y = z = w = 0;
	}

	FORCEINLINE void SafeNormalize3D(float epsilon = SMALL_NUMBER)
	{
		const float len_sqr = LengthSquare3D();
		if (len_sqr > epsilon)
		{
			const float inv_len = GMath::RSqrt(len_sqr);
			x *= inv_len;
			y *= inv_len;
			z *= inv_len;
			w = 0.0f;
			return;
		}
		x = y = z = 0;
		w = 1;
	}

	FORCEINLINE static _Vector4 Normalize3D(const _Vector4& v)
	{
		const float s = GMath::RSqrt(v.LengthSquare3D());
		return _Vector4(v.x * s, v.y * s, v.z * s, 0.0f);
	}

	FORCEINLINE static _Vector4 Normalize(const _Vector4& v)
	{
		const float s = GMath::RSqrt(v.LengthSquare());
		return _Vector4(v.x * s, v.y * s, v.z * s, v.w * s);
	}

	FORCEINLINE static _Vector4 SafeNormalize(const _Vector4& v, float threshold = SMALL_NUMBER)
	{
		const float l = v.LengthSquare();
		if (l == 1.0f)
			return v;
		else if (l < SMALL_NUMBER)
			return _Vector4(0, 0, 0, 0);
		const float s = GMath::RSqrt(l);
		return _Vector4(v.x * s, v.y * s, v.z * s, v.w * s);
	}

	FORCEINLINE static _Vector4 SafeNormalize3D(const _Vector4& v, float threshold = SMALL_NUMBER)
	{
		const float l = v.LengthSquare3D();
		if (l == 1.0f)
			return v;
		else if (l < SMALL_NUMBER)
			return _Vector4(0, 0, 0, 0);
		const float s = GMath::RSqrt(l);
		return _Vector4(v.x * s, v.y * s, v.z * s, 0);
	}

	FORCEINLINE constexpr bool IsNearlyZero3(float epsilon = KINDA_SMALL_NUMBER)const
	{
		return
			GMath::Abs(x) <= epsilon &&
			GMath::Abs(y) <= epsilon &&
			GMath::Abs(z) <= epsilon;
	}

	FORCEINLINE constexpr bool IsNearlyZero(float epsilon = KINDA_SMALL_NUMBER)const
	{
		return
			GMath::Abs(x) <= epsilon &&
			GMath::Abs(y) <= epsilon &&
			GMath::Abs(z) <= epsilon &&
			GMath::Abs(w) <= epsilon;
	}

	FORCEINLINE _Vector4 Reflect3D(const _Vector4& n)const;

} _float4;

#define DECLARE_OP\
		OP(+)\
		OP(-)\
		OP(*)OP(/)

#define OP(op)\
FORCEINLINE _Vector4 operator##op(const _Vector4& A, const _Vector4& B){\
	_Vector4 rs(A);\
	rs##op##= B;\
	rs.CheckNan();\
	return rs;}

DECLARE_OP
#undef OP

#define OP(op)\
FORCEINLINE _Vector4 operator##op(const float s, const _Vector4& V){\
	_Vector4 rs( s op V.x, s op V.y, s op V.z, s op V.w );\
	rs.CheckNan(); return rs;}
DECLARE_OP
#undef OP
#undef DECLARE_OP

#define DECLARE_OP\
		OP(+)\
		OP(-)\
		OP(*)

#define OP(op)\
FORCEINLINE _Vector4 operator##op(const _Vector4& V, const float s){\
	_Vector4 rs( V.x op s, V.y op s, V.z op s, V.w op s );\
	rs.CheckNan(); return rs;}
DECLARE_OP
FORCEINLINE _Vector4 operator/(const _Vector4& V, const float s) {
	float inv_s = 1.0f / s;
	_Vector4 rs(V.x * inv_s, V.y * inv_s, V.z * inv_s, V.w * inv_s);
	rs.CheckNan(); return rs;
}
#undef OP
#undef DECLARE_OP

FORCEINLINE constexpr float Dot3(const _Vector4& v1, const _Vector4& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

FORCEINLINE constexpr float Dot(const _Vector4& v1, const _Vector4& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

FORCEINLINE _Vector4::_Vector4(float x, float y, float z, float w)
	:x(x),y(y),z(z),w(w)
{
	CheckNan();
}

FORCEINLINE _Vector4::_Vector4(const _Vector4& v)
	:x(v.x),y(v.y),z(v.z),w(v.w)
{}


FORCEINLINE _Vector4 _Vector4::operator-() const
{
	return _Vector4(-x, -y, -z, -w);
}

FORCEINLINE _Vector4 _Vector4::Reflect3D(const _Vector4& n) const
{
	return *this - (2.0f * Dot3(*this, n)) * n;
}

FORCEINLINE _Vector4::_Vector4(bool bAsPoint)
	:x(0),y(0),z(0),w((float)bAsPoint)
{
}