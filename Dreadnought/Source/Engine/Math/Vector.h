#pragma once
#include <EngineMath.h>
#include <Vector2D.h>
typedef struct _Vector
{
	float x, y, z;
public:
	std::string ToString()const;
public:
	inline _Vector();
	inline _Vector(float f);
	inline _Vector(float x, float y, float z);
	inline _Vector(const _Vector2& v, float z);
	inline _Vector(const _Vector4& v);
	inline _Vector(bool ForceInit);
	inline void CheckNan() const
	{
#if CHECK_NAN
		if (HasNan())
		{
			*this = ZeroVector;
		}
#endif
	}

	// Vector Zero ( 0, 0, 0 )
	static const _Vector ZeroVector;
	// One Vector ( 1, 1, 1 )
	static const _Vector OneVector;
	// Up Vector ( 0, 0, 1 )
	static const _Vector UpVector;
	// Down Vector ( 0, 0, -1 )
	static const _Vector DownVector;
	// Left Vector ( 1, 0, 0 )
	static const _Vector ForwardVector;
	// Right Vector ( -1, 0, 0 )
	static const _Vector BackwardVector;
	// Left Vector ( 0, 1, 0 )
	static const _Vector LeftVector;
	// Right Vector ( 0, -1, 0 )
	static const _Vector RightVector;

	inline float operator[](size_t Index)const
	{
#if defined(_DEBUG) || defined(DEBUG)
		assert(Index >= 0 && Index < 3);
#endif
		return (&x)[Index];
	}
	inline float& operator[](size_t Index)
	{
#if defined(_DEBUG) || defined(DEBUG)
		assert(Index >= 0 && Index < 3);
#endif
		return (&x)[Index];
	}
	inline void Set(float inX, float inY, float inZ)
	{
		x = inX;
		y = inY;
		z = inZ;
	}

#define DECLARE_OP\
		OP(+=)\
		OP(-=)\
		OP(*=)\
		OP(/=)

#define OP(op) \
	inline _Vector& operator##op(const _Vector& V){\
		x op V.x;\
		y op V.y;\
		z op V.z;\
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
	inline _Vector& operator##op(const float s){\
		x op s;\
		y op s;\
		z op s;\
		CheckNan();\
		return *this;}
	DECLARE_OP
#undef OP
#undef DECLARE_OP
	inline _Vector& operator/=(const float s){
		float inv_s = 1.0f / s;
		x *= inv_s;	y *= inv_s;	z *= inv_s;
		CheckNan();
		return *this;
	}
	

	inline void Negate()
	{
		x = -x;
		y = -y;
		z = -z;
	}

	inline constexpr float operator|(const _Vector& V)const
	{
		return x * V.x + y * V.y + z * V.z;
	}

	inline _Vector operator^(const _Vector& V)const
	{
		// x			y			z
		// vx			vy			vz
		// y vz- z vy	z vx- x vz	x vy- y vx
		return _Vector(
			y * V.z - z * V.y,
			z * V.x - x * V.z,
			x * V.y - y * V.x
		);
	}

	inline _Vector operator-()const{
		return _Vector(-x, -y, -z);
	}

	inline constexpr float Dot(const _Vector& V)const
	{
		return x * V.x + y * V.y + z * V.z;
	}

	inline _Vector Cross(const _Vector& V)const
	{
		return _Vector(
			y * V.z - z * V.y,
			z * V.x - x * V.z,
			x * V.y - y * V.x
		);
	}

	inline void SetLength(float length)
	{
		if (length < KINDA_SMALL_NUMBER)
		{
			x = 0; y = 0; z = 0;
		}
		const float len_sqr = LengthSquare();
		const float s = length * GMath::RSqrt(len_sqr);
		x *= s;
		y *= s;
		z *= s;
	}

	inline void SetLength2D(float length)
	{
		if (length < KINDA_SMALL_NUMBER)
		{
			x = 0; y = 0;
		}
		const float len_sqr = LengthSquare2D();
		const float s = length * GMath::RSqrt(len_sqr);
		x *= s;
		y *= s;
	}

	/// <summary>
	/// Get a new vector with new length but pointed to the same direction as this
	/// </summary>
	/// <param name="length">target length</param>
	/// <returns>new vector</returns>
	inline _Vector GetWithLengthOf(float length)const
	{
		if (length < KINDA_SMALL_NUMBER)
		{
			return ZeroVector;
		}
		const float len_sqr = LengthSquare();
		const float s = length * GMath::RSqrt(len_sqr);
		return _Vector(
			x * s,
			y * s,
			z * s
		);
	}

	/// <summary>
	/// Get a new vector with z component as 0 with new length but pointed to the same direction as this
	/// </summary>
	/// <param name="length">target length</param>
	/// <returns>new vector</returns>
	inline _Vector GetWithLengthOf2D(float length)const
	{
		if (length < KINDA_SMALL_NUMBER)
		{
			return _Vector(0.0f, 0.0f, z);
		}
		const float len_sqr = LengthSquare2D();
		const float s = length * GMath::RSqrt(len_sqr);
		return _Vector(
			x * s,
			y * s,
			z
		);
	}

	inline constexpr void ClampLength(float length)
	{
		if (length < KINDA_SMALL_NUMBER)
		{
			x = 0; y = 0; z = 0;
		}
		const float len_sqr = LengthSquare();
		if (len_sqr > length * length)
		{
			const float s = length * GMath::RSqrt(len_sqr);
			x *= s;
			y *= s;
			z *= s;
		}
	}

	inline constexpr void ClampLength2D(float length)
	{
		if (length < KINDA_SMALL_NUMBER)
		{
			x = 0; y = 0;
		}
		const float len_sqr = LengthSquare2D();
		if (len_sqr > length * length)
		{
			const float s = length * GMath::RSqrt(len_sqr);
			x *= s;
			y *= s;
		}
	}

	inline _Vector GetClampedToLength(float length)const
	{
		if (length < KINDA_SMALL_NUMBER)
		{
			return ZeroVector;
		}
		const float len_sqr = LengthSquare();
		if (len_sqr > length * length)
		{
			const float s = length * GMath::RSqrt(len_sqr);
			return _Vector(
				x * s,
				y * s,
				z * s
			);
		}
		else
		{
			return *this;
		}
	}

	inline _Vector GetClampedToLength2D(float length)const
	{
		if (length < KINDA_SMALL_NUMBER)
		{
			return _Vector(0.0f, 0.0f, z);
		}
		const float len_sqr = LengthSquare2D();
		if (len_sqr > length * length)
		{
			const float s = length * GMath::RSqrt(len_sqr);
			return _Vector(
				x * s,
				y * s,
				z
			);
		}
		else
		{
			return *this;
		}
	}

	inline _Vector Reciprocal()const
	{
		return _Vector(
			x != 0 ? 1.0f / x : BIG_NUMBER,
			y != 0 ? 1.0f / y : BIG_NUMBER,
			z != 0 ? 1.0f / z : BIG_NUMBER
		);
	}

	inline _Vector Reflect(const _Vector& N)const;

	inline constexpr float LengthSquare()const
	{
		return x * x + y * y + z * z;
	}

	inline constexpr float LengthSquare2D()const
	{
		return x * x + y * y;
	}

	inline constexpr float Length()const
	{
		return GMath::Sqrt(LengthSquare());
	}

	inline constexpr float Length2D()const
	{
		return GMath::Sqrt(LengthSquare2D());
	}

	inline constexpr bool IsZero()const
	{
		return x == 0.0f && y == 0.0f && z == 0.0f;
	}

	inline constexpr bool IsNearlyZero(float epsilon = 0.001)const
	{
		return GMath::Abs(x) <= epsilon && GMath::Abs(y) <= epsilon && GMath::Abs(z) <= epsilon;
	}

	inline void Normalize()
	{
		float inv_len = GMath::RSqrt(x * x + y * y + z * z);
		x *= inv_len, y*= inv_len, z*= inv_len;
	
	}

	inline _Vector SafeNormalize(float epsilon = SMALL_NUMBER)const
	{
		const float lensqr = x * x + y * y + z * z;
		if (lensqr > epsilon)
		{
			float inv_len = GMath::RSqrt(lensqr);
			return _Vector(x*inv_len, y*inv_len, z*inv_len);
		}
		return ZeroVector;
	}


	inline static _Vector Normalize(const _Vector& v)
	{
		const float s = GMath::RSqrt(v.LengthSquare());
		return _Vector(v.x * s, v.y * s, v.z * s);
	}

	inline static _Vector SafeNormalize(const _Vector& v, float threshold = SMALL_NUMBER)
	{
		const float l = v.LengthSquare();
		if (l == 1.0f)
			return v;
		else if (l < SMALL_NUMBER)
			return _Vector::ZeroVector;
		const float s = GMath::RSqrt(l);
		return _Vector(v.x * s, v.y * s, v.z * s);
	}

	inline constexpr void ToDirectionAndLength(_Vector& outDir, float& outLength)const
	{
		outLength = Length();
		if (outLength > SMALL_NUMBER)
		{
			float inv_length = 1.0f / outLength;
			outDir = _Vector(x * inv_length, y * inv_length, z * inv_length);
		}
		else
		{
			outDir = ZeroVector;
		}
	}

	inline constexpr float GetMinComponent()const
	{
		return GMath::Min(GMath::Min(x, y), z);
	}

	inline constexpr float GetMaxComponent()const
	{
		return GMath::Max(GMath::Max(x, y), z);
	}

	inline constexpr float GetAbsMinComponent()const
	{
		return GMath::Min(GMath::Min(GMath::Abs(x), GMath::Abs(y)), GMath::Abs(z));
	}

	inline constexpr float GetAbsMaxComponent()const
	{
		return GMath::Max(GMath::Max(GMath::Abs(x), GMath::Abs(y)), GMath::Abs(z));
	}

	inline _Vector Min(const _Vector& v)const
	{
		return _Vector(GMath::Min(x, v.x), GMath::Min(y, v.y), GMath::Min(z, v.z));
	}

	inline _Vector Max(const _Vector& v)const
	{
		return _Vector(GMath::Max(x, v.x), GMath::Max(y, v.y), GMath::Max(z, v.z));
	}

	inline _Vector Abs()const
	{
		return _Vector(GMath::Abs(x), GMath::Abs(y), GMath::Abs(z));
	}

	inline constexpr bool IsNormalized(float epsilon = THRESH_VECTOR_NORMALIZED)const
	{
		return (GMath::Abs(1 - LengthSquare()) < epsilon);
	}

	inline _Vector Sign()const
	{
		return _Vector(GMath::Sign(x), GMath::Sign(y), GMath::Sign(z));
	}

	inline constexpr void Clamp(const _Vector& minV, const _Vector& maxV)
	{
		x = GMath::Clamp(x, minV.x, maxV.x);
		y = GMath::Clamp(y, minV.y, maxV.y);
		z = GMath::Clamp(z, minV.z, maxV.z);
	}

	inline _Vector Projection()const
	{
		const float inv_z = 1.0f / z;
		return _Vector(x * inv_z, y * inv_z, 1);
	}

	inline constexpr void Saturate()
	{
		x = GMath::Saturate(x);
		y = GMath::Saturate(y);
		z = GMath::Saturate(z);
	}

	inline _Vector ProjectOnNormal(const _Vector& n)const;

	inline constexpr float DistanceSquaredTo(const _Vector& p)
	{
		return GMath::Square(p.x - x) + GMath::Square(p.y - y) + GMath::Square(p.z - z);
	}

	inline constexpr float DistanceSquaredTo2D(const _Vector& p)
	{
		return GMath::Square(p.x - x) + GMath::Square(p.y - y);
	}

	inline constexpr float DistanceTo(const _Vector& p)
	{
		return GMath::Sqrt(DistanceSquaredTo(p));
	}

	inline constexpr float DistanceTo2D(const _Vector& p)
	{
		return GMath::Sqrt(DistanceSquaredTo2D(p));
	}

	inline static constexpr float DistanceSquared2D(const _Vector& v1, const _Vector& v2)
	{
		return GMath::Square(v2.x - v1.x) + GMath::Square(v2.y - v1.y);
	}

	inline static constexpr float Distance2D(const _Vector& v1, const _Vector& v2)
	{
		return GMath::Sqrt(DistanceSquared2D(v1, v2));
	}

	inline static constexpr float DistanceSquared(const _Vector& v1, const _Vector& v2)
	{
		return GMath::Square(v2.x - v1.x) + GMath::Square(v2.y - v1.y) + GMath::Square(v2.z - v1.z);
	}

	inline static constexpr float Distance(const _Vector& v1, const _Vector& v2)
	{
		return GMath::Sqrt(DistanceSquared(v1, v2));
	}

	inline static bool PointsAreClose(const _Vector& p1, const _Vector& p2, float threshold = THRESH_POINTS_ARE_SAME)
	{
		return 
			GMath::Abs(p1.x - p2.x) < threshold &&
			GMath::Abs(p1.y - p2.y) < threshold &&
			GMath::Abs(p1.z - p2.z) < threshold;
	}

	inline float HeadingAngle()const
	{
		return GMath::Atan2(y, x);
	}

	inline _Vector2 UnitCartesianToSpherical()const
	{
		assert(IsNormalized());
		const float theta = GMath::Acos(z);
		const float phi = GMath::Atan2(y, x);
		return _Vector2(theta, phi);
	}

	inline _Vector2 CartesianToSpherical()const
	{
		float theta = 0;
		const float phi = GMath::Atan2(y, x);
		const float ls = LengthSquare();
		if (GMath::Abs(1 - ls) <= THRESH_VECTOR_NORMALIZED)
		{
			theta = GMath::Acos(z);
		}
		else
		{
			const float s = GMath::RSqrt(ls);
			theta = GMath::Acos(z * s);
		}
		return _Vector2(theta, phi);
	}

	inline static _Vector CartesianFromSpherical(const _Vector2 s)
	{
		const float sinTheta = GMath::Sin(s.x);
		return _Vector(
			GMath::Cos(s.y) * sinTheta,
			GMath::Sin(s.y) * sinTheta,
			GMath::Cos(s.x)
		);
	}

	inline static _Vector CartesianFromSphericalEst(const _Vector2 s)
	{
		float sinTheta, cosTheta, sinPhi, cosPhi;
		GMath::SinCos(&sinTheta, &cosTheta, s.x);
		GMath::SinCos(&sinPhi, &cosPhi, s.y);
		return _Vector(
			cosPhi * sinTheta,
			sinPhi * sinTheta,
			cosTheta
		);
	}

	/// <summary>
	/// See if two unit length vector are parallel to each other
	/// </summary>
	inline static constexpr bool IsParallel(const _Vector& n1, const _Vector& n2, float threshold = THRESH_NORMALS_ARE_PARALLEL)
	{
		const float non = n1 | n2;
		return GMath::Abs(non) >= threshold;
	}

	/// <summary>
	/// See if two unit length vector are parallel and point to the same direction to each other
	/// </summary>
	inline static constexpr bool Coincident(const _Vector& n1, const _Vector& n2, float threshold = THRESH_NORMALS_ARE_PARALLEL)
	{
		const float non = n1 | n2;
		return non >= threshold;
	}

	inline static constexpr bool Orthogonal(const _Vector& n1, const _Vector& n2, float threshold = THRESH_NORMALS_ARE_ORTHOGONAL)
	{
		const float non = n1 | n2;
		return GMath::Abs(non) <= threshold;
	}

	inline static _Vector VectorPlaneProjection(const _Vector& v, const _Vector& planeNormal);

	inline bool HasNan()const { return GMath::IsInf(x) || GMath::IsInf(y) || GMath::IsInf(z); }

	inline constexpr float DistanceToPlane(const _Vector& PlaneBase, const _Vector& PlaneNormal)const;

	inline static float PointPlaneDistance(const _Vector& P, const _Vector& PlaneBase, const _Vector& PlaneNormal);
} _float3;

#define DECLARE_OP\
		OP(+)\
		OP(-)\
		OP(*)OP(/)

#define OP(op)\
inline _Vector operator##op(const _Vector& A, const _Vector& B){\
	_Vector rs(A);\
	rs##op##= B;\
	rs.CheckNan();\
	return rs;}

DECLARE_OP
#undef OP

#define OP(op)\
inline _Vector operator##op(const float s, const _Vector& V){\
	_Vector rs( s op V.x, s op V.y, s op V.z );\
	rs.CheckNan(); return rs;}
DECLARE_OP
#undef OP
#undef DECLARE_OP

#define DECLARE_OP\
		OP(+)\
		OP(-)\
		OP(*)

#define OP(op)\
inline _Vector operator##op(const _Vector& V, const float s){\
	_Vector rs( V.x op s, V.y op s, V.z op s );\
	rs.CheckNan(); return rs;}
DECLARE_OP
inline _Vector operator/(const _Vector& V, const float s) {
	float inv_s = 1.0f / s;
	_Vector rs(V.x * inv_s, V.y * inv_s, V.z * inv_s);
	rs.CheckNan(); return rs;
}
#undef OP
#undef DECLARE_OP

inline _Vector2::_Vector2(const _Vector& v)
	: x(v.x), y(v.y)
{
	CheckNan();
}
const _Vector _Vector::ZeroVector(0, 0, 0);
const _Vector _Vector::OneVector(1, 1, 1);
const _Vector _Vector::UpVector(0, 0, 1);
const _Vector _Vector::DownVector(0, 0, -1);
const _Vector _Vector::ForwardVector(1, 0, 0);
const _Vector _Vector::BackwardVector(-1, 0, 0);
const _Vector _Vector::RightVector(0, 1, 0);
const _Vector _Vector::LeftVector(0, -1, 0);


inline float _Vector::PointPlaneDistance(const _Vector& P, const _Vector& PlaneBase, const _Vector& PlaneNormal)
{
	return (P - PlaneBase) | PlaneNormal;
}

inline _Vector ClampVector(const _Vector& v, const _Vector& minV, const _Vector& maxV)
{
	return _Vector(
		GMath::Clamp(v.x, minV.x, maxV.x),
		GMath::Clamp(v.y, minV.y, maxV.y),
		GMath::Clamp(v.z, minV.z, maxV.z)
	);
}

inline _Vector Sign(const _Vector& v)
{
	return _Vector(GMath::Sign(v.x), GMath::Sign(v.y), GMath::Sign(v.z));
}

inline _Vector Saturate(const _Vector& v)
{
	return _Vector(GMath::Saturate(v.x), GMath::Saturate(v.y), GMath::Saturate(v.z));
}

inline constexpr float Dot(const _Vector& A, const _Vector& B)
{
	return A | B;
}

inline _Vector Cross(const _Vector& A, const _Vector& B)
{
	return A ^ B;
}

inline _Vector::_Vector()
{
}

inline _Vector::_Vector(float f)
	:
	x(f), y(f), z(f)
{
	CheckNan();
}

inline _Vector::_Vector(float x, float y, float z)
	:x(x), y(y), z(z)
{
	CheckNan();
}
inline _Vector::_Vector(const _Vector2& v, float z)
	: x(v.x), y(v.y), z(z)
{
	CheckNan();
}

inline _Vector::_Vector(const _Vector4& v)
	:x(v.x),y(v.y),z(v.z)
{
	CheckNan();
}

inline _Vector::_Vector(bool ForceInit)
	:x(0),y(0),z(0)
{
}

inline _Vector _Vector::Reflect(const _Vector& N)const
{
	return *this - N * (2.0f * (*this | N));	
}

inline _Vector Reflect(const _Vector& V, const _Vector& N)
{
	return V - N * (2 * (V | N));
}

inline _Vector _Vector::ProjectOnNormal(const _Vector& n) const
{
	return n *(*this | n);
}

inline _Vector _Vector::VectorPlaneProjection(const _Vector& v, const _Vector& planeNormal)
{
	return v - v.ProjectOnNormal(planeNormal);
}

inline constexpr float _Vector::DistanceToPlane(const _Vector& PlaneBase, const _Vector& PlaneNormal) const
{
	return ((*this) - PlaneBase) | PlaneNormal;
}


inline _Vector _Vector2::SphericalToUnitCartesian() const
{
	const float sinTheta = GMath::Sin(x);
	return _Vector(
		GMath::Cos(y) * sinTheta,
		GMath::Sin(y) * sinTheta,
		GMath::Cos(x)
	);
}

inline _Vector _Vector2::SphericalToUnitCartesianEst() const
{
	float sinTheta, cosTheta, sinPhi, cosPhi;
	GMath::SinCos(&sinTheta, &cosTheta, x);
	GMath::SinCos(&sinPhi, &cosPhi, y);
	return _Vector(
		cosPhi * sinTheta,
		sinPhi * sinTheta,
		cosTheta
	);
}

inline _Vector4::_Vector4(const _Vector& v, float w)
	: x(v.x), y(v.y), z(v.z), w(w)
{
	CheckNan();
}

inline _Vector4::_Vector4(float x, const _Vector& v)
	:x(x), y(v.x), z(v.y), w(v.z)
{
	CheckNan();
}

inline std::string _Vector::ToString() const
{
	return FormatString("X=%3.3f Y=%3.3f Z=%3.3f", x, y, z);
}