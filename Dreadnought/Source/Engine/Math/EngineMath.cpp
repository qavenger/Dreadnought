#include<stdafx.h>
#include<EngineMath.h>
#include<VectorSSE.h>
#include<Vector.h>
#include<Vector2D.h>
#include<Vector4D.h>
#include<Rotator.h>
#include<Quaternion.h>
#include<Matrix.h>

const _Vector _Vector::ZeroVector(0, 0, 0);
const _Vector _Vector::OneVector(1, 1, 1);
const _Vector _Vector::UpVector(0, 0, 1);
const _Vector _Vector::DownVector(0, 0, -1);
const _Vector _Vector::ForwardVector(1, 0, 0);
const _Vector _Vector::BackwardVector(-1, 0, 0);
const _Vector _Vector::RightVector(0, 1, 0);
const _Vector _Vector::LeftVector(0, -1, 0);

const _Vector2 _Vector2::ZeroVector(0);
const _Vector2 _Vector2::OneVector(1);
const _Vector2 _Vector2::Unit45DegVector(INV_SQRT_2, INV_SQRT_2);

const _Rotator _Rotator::ZeroRotator(0, 0, 0);

const _Matrix _Matrix::Identity(true);

FORCEINLINE std::string _Vector2::ToString()const
{
	return FormatString("X=%3.3f Y=%3.3f", x, y);
}

FORCEINLINE std::string _Rotator::ToString()const
{
	return FormatString("Roll=%3.3f Pitch=%3.3f Yaw=%3.3f", Roll,Pitch, Yaw);
}

FORCEINLINE _Vector _Vector2::SphericalToUnitCartesian() const
{
	const float sinTheta = GMath::Sin(x);
	return _Vector(
		GMath::Cos(y) * sinTheta,
		GMath::Sin(y) * sinTheta,
		GMath::Cos(x)
	);
}

FORCEINLINE _Vector _Vector2::SphericalToUnitCartesianEst() const
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

FORCEINLINE _Vector4::_Vector4(const _Vector& v, float w)
	: x(v.x), y(v.y), z(v.z), w(w)
{
	CheckNan();
}

FORCEINLINE _Vector4::_Vector4(float x, const _Vector& v)
	: x(x), y(v.x), z(v.y), w(v.z)
{
	CheckNan();
}

FORCEINLINE std::string _Vector::ToString() const
{
	return FormatString("X=%3.3f Y=%3.3f Z=%3.3f", x, y, z);
}

FORCEINLINE _Vector2::_Vector2(const _Vector& v)
	: x(v.x), y(v.y)
{
	CheckNan();
}


FORCEINLINE _Vector2 _Vector::UnitCartesianToSpherical()const
{
	assert(IsNormalized());
	const float theta = GMath::Acos(z);
	const float phi = GMath::Atan2(y, x);
	return _Vector2(theta, phi);
}

FORCEINLINE _Vector2 _Vector::CartesianToSpherical()const
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

FORCEINLINE _Vector _Vector::CartesianFromSpherical(const _Vector2 s)
{
	const float sinTheta = GMath::Sin(s.x);
	return _Vector(
		GMath::Cos(s.y) * sinTheta,
		GMath::Sin(s.y) * sinTheta,
		GMath::Cos(s.x)
	);
}

FORCEINLINE _Vector _Vector::CartesianFromSphericalEst(const _Vector2 s)
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

FORCEINLINE _Vector::_Vector(const _Vector2& v, float z)
	: x(v.x), y(v.y), z(z)
{
	CheckNan();
}


FORCEINLINE _Vector::_Vector(const _Vector4& v)
	:x(v.x), y(v.y), z(v.z)
{
	CheckNan();
}

FORCEINLINE _Matrix::_Matrix(_Vector x, _Vector y, _Vector z, _Vector w)
	:
	M{
	x.x, x.y, x.z, 0.0f,
	y.x, y.y, y.z, 0.0f,
	z.x, z.y, z.z, 0.0f,
	w.x, w.y, w.z, 1.0f
	}
{
}

FORCEINLINE _Matrix::_Matrix(_Vector4 x, _Vector4 y, _Vector4 z, _Vector4 w)
	:
	M{
	x.x, x.y, x.z, x.w, 
	y.x, y.y, y.z, y.w, 
	z.x, z.y, z.z, z.w,
	w.x, w.y, w.z, w.w
	}
{
}

void _Matrix::SetIdentity()
{
	M[0][0] = 1; M[0][1] = 0; M[0][2] = 0; M[0][3] = 0;
	M[1][0] = 0; M[1][1] = 1; M[1][2] = 0; M[1][3] = 0;
	M[2][0] = 0; M[2][1] = 0; M[2][2] = 1; M[2][3] = 0;
	M[3][0] = 0; M[3][1] = 0; M[3][2] = 0; M[3][3] = 1;
}

FORCEINLINE VectorRegister SSE::VectorExp2(const VectorRegister& X)
{
	return MakeVectorRegister(GMath::Exp2(VectorGetComponent(X, 0)), GMath::Exp2(VectorGetComponent(X, 1)), GMath::Exp2(VectorGetComponent(X, 2)), GMath::Exp2(VectorGetComponent(X, 3)));
}

FORCEINLINE VectorRegister SSE::VectorLog2(const VectorRegister& X)
{
	return MakeVectorRegister(GMath::Log2(VectorGetComponent(X, 0)), GMath::Log2(VectorGetComponent(X, 1)), GMath::Log2(VectorGetComponent(X, 2)), GMath::Log2(VectorGetComponent(X, 3)));
}

FORCEINLINE _Vector4::_Vector4(const _Vector2& v, float z, float w)
	:x(v.x), y(v.y), z(z), w(w)
{
	CheckNan();
}

FORCEINLINE _Vector4::_Vector4(const _Vector2& v0, const _Vector2& v1)
	: x(v0.x), y(v0.y), z(v1.x), w(v1.y)
{
	CheckNan();
}


FORCEINLINE _Vector2::_Vector2(const _Vector4& v)
{
	x = v.x;
	y = v.y;
}
