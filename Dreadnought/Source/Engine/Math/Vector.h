#pragma once
#include <EngineMath.h>
typedef struct _Vector
{
	float x, y, z;
public:
	std::string ToString()const;
public:
	FORCEINLINE _Vector();
	FORCEINLINE _Vector(float f);
	FORCEINLINE _Vector(float x, float y, float z);
	FORCEINLINE _Vector(const _Vector2& v, float z);
	FORCEINLINE _Vector(const _Vector& v);
	FORCEINLINE _Vector(const _Vector4& v);
	FORCEINLINE _Vector& operator=(const _Vector& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}
	FORCEINLINE _Vector& operator=(_Vector&& other)noexcept
	{
		x = std::move(other.x);
		y = std::move(other.y);
		z = std::move(other.z);
		return *this;
	}

	FORCEINLINE void CheckNan() const
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

	FORCEINLINE float operator[](size_t Index)const
	{
#if defined(_DEBUG) || defined(DEBUG)
		assert(Index >= 0 && Index < 3);
#endif
		return (&x)[Index];
	}
	FORCEINLINE float& operator[](size_t Index)
	{
#if defined(_DEBUG) || defined(DEBUG)
		assert(Index >= 0 && Index < 3);
#endif
		return (&x)[Index];
	}
	FORCEINLINE void Set(float inX, float inY, float inZ)
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
	FORCEINLINE _Vector& operator##op(const _Vector& V){\
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
	FORCEINLINE _Vector& operator##op(const float s){\
		x op s;\
		y op s;\
		z op s;\
		CheckNan();\
		return *this;}
	DECLARE_OP
#undef OP
#undef DECLARE_OP
	FORCEINLINE _Vector& operator/=(const float s){
		float inv_s = 1.0f / s;
		x *= inv_s;	y *= inv_s;	z *= inv_s;
		CheckNan();
		return *this;
	}
	

	FORCEINLINE void Negate()
	{
		x = -x;
		y = -y;
		z = -z;
	}

	FORCEINLINE constexpr float operator|(const _Vector& V)const
	{
		return x * V.x + y * V.y + z * V.z;
	}

	FORCEINLINE _Vector operator^(const _Vector& V)const
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

	FORCEINLINE _Vector operator-()const{
		return _Vector(-x, -y, -z);
	}

	FORCEINLINE constexpr float Dot(const _Vector& V)const
	{
		return x * V.x + y * V.y + z * V.z;
	}

	FORCEINLINE _Vector Cross(const _Vector& V)const
	{
		return _Vector(
			y * V.z - z * V.y,
			z * V.x - x * V.z,
			x * V.y - y * V.x
		);
	}

	FORCEINLINE void SetLength(float length)
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

	FORCEINLINE void SetLength2D(float length)
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
	FORCEINLINE _Vector GetWithLengthOf(float length)const
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
	FORCEINLINE _Vector GetWithLengthOf2D(float length)const
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

	FORCEINLINE constexpr void ClampLength(float length)
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

	FORCEINLINE constexpr void ClampLength2D(float length)
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

	FORCEINLINE _Vector GetClampedToLength(float length)const
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

	FORCEINLINE _Vector GetClampedToLength2D(float length)const
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

	FORCEINLINE _Vector Reciprocal()const
	{
		return _Vector(
			x != 0 ? 1.0f / x : BIG_NUMBER,
			y != 0 ? 1.0f / y : BIG_NUMBER,
			z != 0 ? 1.0f / z : BIG_NUMBER
		);
	}

	FORCEINLINE _Vector Reflect(const _Vector& N)const;

	FORCEINLINE constexpr float LengthSquare()const
	{
		return x * x + y * y + z * z;
	}

	FORCEINLINE constexpr float LengthSquare2D()const
	{
		return x * x + y * y;
	}

	FORCEINLINE constexpr float Length()const
	{
		return GMath::Sqrt(LengthSquare());
	}

	FORCEINLINE constexpr float Length2D()const
	{
		return GMath::Sqrt(LengthSquare2D());
	}

	FORCEINLINE constexpr bool IsZero()const
	{
		return x == 0.0f && y == 0.0f && z == 0.0f;
	}

	FORCEINLINE constexpr bool IsNearlyZero(float epsilon = 0.001)const
	{
		return GMath::Abs(x) <= epsilon && GMath::Abs(y) <= epsilon && GMath::Abs(z) <= epsilon;
	}

	FORCEINLINE void Normalize()
	{
		float inv_len = GMath::RSqrt(x * x + y * y + z * z);
		x *= inv_len, y*= inv_len, z*= inv_len;
	
	}

	FORCEINLINE _Vector SafeNormalize(float epsilon = SMALL_NUMBER)const
	{
		const float lensqr = x * x + y * y + z * z;
		if (lensqr > epsilon)
		{
			float inv_len = GMath::RSqrt(lensqr);
			return _Vector(x*inv_len, y*inv_len, z*inv_len);
		}
		return ZeroVector;
	}


	FORCEINLINE static _Vector Normalize(const _Vector& v)
	{
		const float s = GMath::RSqrt(v.LengthSquare());
		return _Vector(v.x * s, v.y * s, v.z * s);
	}

	FORCEINLINE static _Vector SafeNormalize(const _Vector& v, float threshold = SMALL_NUMBER)
	{
		const float l = v.LengthSquare();
		if (l == 1.0f)
			return v;
		else if (l < SMALL_NUMBER)
			return _Vector::ZeroVector;
		const float s = GMath::RSqrt(l);
		return _Vector(v.x * s, v.y * s, v.z * s);
	}

	FORCEINLINE constexpr void ToDirectionAndLength(_Vector& outDir, float& outLength)const
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

	FORCEINLINE constexpr float GetMinComponent()const
	{
		return GMath::Min(GMath::Min(x, y), z);
	}

	FORCEINLINE constexpr float GetMaxComponent()const
	{
		return GMath::Max(GMath::Max(x, y), z);
	}

	FORCEINLINE constexpr float GetAbsMinComponent()const
	{
		return GMath::Min(GMath::Min(GMath::Abs(x), GMath::Abs(y)), GMath::Abs(z));
	}

	FORCEINLINE constexpr float GetAbsMaxComponent()const
	{
		return GMath::Max(GMath::Max(GMath::Abs(x), GMath::Abs(y)), GMath::Abs(z));
	}

	FORCEINLINE _Vector Min(const _Vector& v)const
	{
		return _Vector(GMath::Min(x, v.x), GMath::Min(y, v.y), GMath::Min(z, v.z));
	}

	FORCEINLINE _Vector Max(const _Vector& v)const
	{
		return _Vector(GMath::Max(x, v.x), GMath::Max(y, v.y), GMath::Max(z, v.z));
	}

	FORCEINLINE _Vector Abs()const
	{
		return _Vector(GMath::Abs(x), GMath::Abs(y), GMath::Abs(z));
	}

	FORCEINLINE constexpr bool IsNormalized(float epsilon = THRESH_VECTOR_NORMALIZED)const
	{
		return (GMath::Abs(1 - LengthSquare()) < epsilon);
	}

	FORCEINLINE _Vector Sign()const
	{
		return _Vector(GMath::Sign(x), GMath::Sign(y), GMath::Sign(z));
	}

	FORCEINLINE constexpr void Clamp(const _Vector& minV, const _Vector& maxV)
	{
		x = GMath::Clamp(x, minV.x, maxV.x);
		y = GMath::Clamp(y, minV.y, maxV.y);
		z = GMath::Clamp(z, minV.z, maxV.z);
	}

	FORCEINLINE _Vector Projection()const
	{
		const float inv_z = 1.0f / z;
		return _Vector(x * inv_z, y * inv_z, 1);
	}

	FORCEINLINE constexpr void Saturate()
	{
		x = GMath::Saturate(x);
		y = GMath::Saturate(y);
		z = GMath::Saturate(z);
	}

	FORCEINLINE _Vector ProjectOnNormal(const _Vector& n)const;

	FORCEINLINE constexpr float DistanceSquaredTo(const _Vector& p)
	{
		return GMath::Square(p.x - x) + GMath::Square(p.y - y) + GMath::Square(p.z - z);
	}

	FORCEINLINE constexpr float DistanceSquaredTo2D(const _Vector& p)
	{
		return GMath::Square(p.x - x) + GMath::Square(p.y - y);
	}

	FORCEINLINE constexpr float DistanceTo(const _Vector& p)
	{
		return GMath::Sqrt(DistanceSquaredTo(p));
	}

	FORCEINLINE constexpr float DistanceTo2D(const _Vector& p)
	{
		return GMath::Sqrt(DistanceSquaredTo2D(p));
	}

	FORCEINLINE static constexpr float DistanceSquared2D(const _Vector& v1, const _Vector& v2)
	{
		return GMath::Square(v2.x - v1.x) + GMath::Square(v2.y - v1.y);
	}

	FORCEINLINE static constexpr float Distance2D(const _Vector& v1, const _Vector& v2)
	{
		return GMath::Sqrt(DistanceSquared2D(v1, v2));
	}

	FORCEINLINE static constexpr float DistanceSquared(const _Vector& v1, const _Vector& v2)
	{
		return GMath::Square(v2.x - v1.x) + GMath::Square(v2.y - v1.y) + GMath::Square(v2.z - v1.z);
	}

	FORCEINLINE static constexpr float Distance(const _Vector& v1, const _Vector& v2)
	{
		return GMath::Sqrt(DistanceSquared(v1, v2));
	}

	FORCEINLINE static bool PointsAreClose(const _Vector& p1, const _Vector& p2, float threshold = THRESH_POINTS_ARE_SAME)
	{
		return 
			GMath::Abs(p1.x - p2.x) < threshold &&
			GMath::Abs(p1.y - p2.y) < threshold &&
			GMath::Abs(p1.z - p2.z) < threshold;
	}

	FORCEINLINE float HeadingAngle()const
	{
		return GMath::Atan2(y, x);
	}

	FORCEINLINE _Vector2 UnitCartesianToSpherical()const;

	FORCEINLINE _Vector2 CartesianToSpherical()const;
	
	FORCEINLINE static _Vector CartesianFromSpherical(const _Vector2 s);
	

	FORCEINLINE static _Vector CartesianFromSphericalEst(const _Vector2 s);
	

	/// <summary>
	/// See if two unit length vector are parallel to each other
	/// </summary>
	FORCEINLINE static constexpr bool IsParallel(const _Vector& n1, const _Vector& n2, float threshold = THRESH_NORMALS_ARE_PARALLEL)
	{
		const float non = n1 | n2;
		return GMath::Abs(non) >= threshold;
	}

	/// <summary>
	/// See if two unit length vector are parallel and point to the same direction to each other
	/// </summary>
	FORCEINLINE static constexpr bool Coincident(const _Vector& n1, const _Vector& n2, float threshold = THRESH_NORMALS_ARE_PARALLEL)
	{
		const float non = n1 | n2;
		return non >= threshold;
	}

	FORCEINLINE static constexpr bool Orthogonal(const _Vector& n1, const _Vector& n2, float threshold = THRESH_NORMALS_ARE_ORTHOGONAL)
	{
		const float non = n1 | n2;
		return GMath::Abs(non) <= threshold;
	}

	FORCEINLINE static _Vector VectorPlaneProjection(const _Vector& v, const _Vector& planeNormal);

	FORCEINLINE bool HasNan()const { return GMath::IsInf(x) || GMath::IsInf(y) || GMath::IsInf(z); }

	FORCEINLINE constexpr float DistanceToPlane(const _Vector& PlaneBase, const _Vector& PlaneNormal)const;

	FORCEINLINE static float PointPlaneDistance(const _Vector& P, const _Vector& PlaneBase, const _Vector& PlaneNormal);
} _float3;

FORCEINLINE _Vector::_Vector(const _Vector& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

#define DECLARE_OP\
		OP(+)\
		OP(-)\
		OP(*)OP(/)

#define OP(op)\
FORCEINLINE _Vector operator##op(const _Vector& A, const _Vector& B){\
	_Vector rs(A);\
	rs##op##= B;\
	rs.CheckNan();\
	return rs;}

DECLARE_OP
#undef OP

#define OP(op)\
FORCEINLINE _Vector operator##op(const float s, const _Vector& V){\
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
FORCEINLINE _Vector operator##op(const _Vector& V, const float s){\
	_Vector rs( V.x op s, V.y op s, V.z op s );\
	rs.CheckNan(); return rs;}
DECLARE_OP
FORCEINLINE _Vector operator/(const _Vector& V, const float s) {
	float inv_s = 1.0f / s;
	_Vector rs(V.x * inv_s, V.y * inv_s, V.z * inv_s);
	rs.CheckNan(); return rs;
}
#undef OP
#undef DECLARE_OP


FORCEINLINE float _Vector::PointPlaneDistance(const _Vector& P, const _Vector& PlaneBase, const _Vector& PlaneNormal)
{
	return (P - PlaneBase) | PlaneNormal;
}

FORCEINLINE _Vector ClampVector(const _Vector& v, const _Vector& minV, const _Vector& maxV)
{
	return _Vector(
		GMath::Clamp(v.x, minV.x, maxV.x),
		GMath::Clamp(v.y, minV.y, maxV.y),
		GMath::Clamp(v.z, minV.z, maxV.z)
	);
}

FORCEINLINE _Vector Sign(const _Vector& v)
{
	return _Vector(GMath::Sign(v.x), GMath::Sign(v.y), GMath::Sign(v.z));
}

FORCEINLINE _Vector Saturate(const _Vector& v)
{
	return _Vector(GMath::Saturate(v.x), GMath::Saturate(v.y), GMath::Saturate(v.z));
}

FORCEINLINE constexpr float Dot(const _Vector& A, const _Vector& B)
{
	return A | B;
}

FORCEINLINE _Vector Cross(const _Vector& A, const _Vector& B)
{
	return A ^ B;
}

FORCEINLINE _Vector::_Vector()
{
}

FORCEINLINE _Vector::_Vector(float f)
	:
	x(f), y(f), z(f)
{
	CheckNan();
}

FORCEINLINE _Vector::_Vector(float x, float y, float z)
	:x(x), y(y), z(z)
{
	CheckNan();
}



FORCEINLINE _Vector _Vector::Reflect(const _Vector& N)const
{
	return *this - N * (2.0f * (*this | N));	
}

FORCEINLINE _Vector Reflect(const _Vector& V, const _Vector& N)
{
	return V - N * (2 * (V | N));
}

FORCEINLINE _Vector _Vector::ProjectOnNormal(const _Vector& n) const
{
	return n *(*this | n);
}

FORCEINLINE _Vector _Vector::VectorPlaneProjection(const _Vector& v, const _Vector& planeNormal)
{
	return v - v.ProjectOnNormal(planeNormal);
}

FORCEINLINE constexpr float _Vector::DistanceToPlane(const _Vector& PlaneBase, const _Vector& PlaneNormal) const
{
	return ((*this) - PlaneBase) | PlaneNormal;
}

