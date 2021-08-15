#include<stdafx.h>
#include<EngineMath.h>
#include<VectorSSE.h>
#include<Vector.h>
#include<Vector2D.h>
#include<Vector4D.h>
#include<Rotator.h>
#include<Quaternion.h>
#include<Matrix.h>


FORCEINLINE float GMath::FMod(float X, float Y)
{
	const float AbsY = fabs(Y);
	if (AbsY <= SMALL_NUMBER)
	{
		return 0;
	}
	const float div = X / Y;
	const float quotient = fabs(div) < FLOAT_NON_FRACTIONAL ? truncf(div) : div;
	float intPortion = Y * quotient;
	if (fabsf(intPortion) > fabs(X))
	{
		intPortion = X;
	}
	const float rs = X - intPortion;
	return Clamp(rs, -AbsY, AbsY);
}

FORCEINLINE int32 GMath::TruncToInt(float f)
{
	return SSE::TruncToInt(f);
}

FORCEINLINE int32 GMath::RoundToInt(float f)
{
	return SSE::RoundToInt(f);
}

FORCEINLINE int32 GMath::FloorToInt(float f)
{
	return SSE::FloorToInt(f);
}

FORCEINLINE int32 GMath::CeilToInt(float f)
{
	return SSE::CeilToInt(f);
}

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
const _Quaternion _Quaternion::Identity{ 0, 0, 0, 1 };
const _Matrix _Matrix::Identity(true);

FORCEINLINE std::string _Vector2::ToString()const
{
	return FormatString("X=%3.3f Y=%3.3f", x, y);
}

FORCEINLINE std::string _Rotator::ToString()const
{
	return FormatString("Roll=%3.3f Pitch=%3.3f Yaw=%3.3f", Roll, Pitch, Yaw);
}

std::string _Quaternion::ToString() const
{
	return FormatString("X=%.9f Y=%.9f Z=%.9f W=%.9f", x, y, z, w);
}

bool _Rotator::HasNan() const
{
	return GMath::IsInf(Roll) || GMath::IsInf(Pitch) || GMath::IsInf(Yaw);
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

FORCEINLINE std::string _Matrix::ToString() const
{
	std::string rs;
	rs += FormatString("[%g %g %g %g] ", M[0][0], M[0][1], M[0][2], M[0][3]);
	rs += FormatString("[%g %g %g %g] ", M[1][0], M[1][1], M[1][2], M[1][3]);
	rs += FormatString("[%g %g %g %g] ", M[2][0], M[2][1], M[2][2], M[2][3]);
	rs += FormatString("[%g %g %g %g] ", M[3][0], M[3][1], M[3][2], M[3][3]);
	return rs;
}

FORCEINLINE uint32 _Matrix::Hash() const
{
	uint32 rs = 0;
	const uint32* ptr = (uint32*)this;
	for (uint32 i = 0; i < 16; ++i)
	{
		rs ^= ptr[i] + i;
	}
	return rs;
}

_Matrix::_Matrix(const _Vector& x, const _Vector& y, const _Vector& z, const _Vector& w)
	:
	M{
	x.x, x.y, x.z, 0.0f,
	y.x, y.y, y.z, 0.0f,
	z.x, z.y, z.z, 0.0f,
	w.x, w.y, w.z, 1.0f
} {}

_Matrix::_Matrix(const _Vector4& x, const _Vector4& y, const _Vector4& z, const _Vector4& w)
	:
	M{
	x.x, x.y, x.z, x.w,
	y.x, y.y, y.z, y.w,
	z.x, z.y, z.z, z.w,
	w.x, w.y, w.z, w.w
}
{
}

_Matrix::_Matrix(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
	:
	M{ m00,m01,m02,m03,
		m10,m11,m12,m13,
		m20,m21,m22,m23,
		m30,m31,m32,m33 }
{
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

void _Matrix::operator*=(const _Matrix& m)
{
	SSE::MatrixMultiply(this, this, &m);
}

_Matrix _Matrix::operator*(const _Matrix& m) const
{
	_Matrix result;
	SSE::MatrixMultiply(&result, this, &m);
	return result;
}

_Vector4 _Matrix::TransformVector4(const _Vector4& v) const
{
	_Vector4 result;
	VectorRegister vP = VectorLoadAligned(&v);
	VectorRegister vR = SSE::VectorTransformVector(vP, this);
	VectorStoreAligned(vR, &result);
	return result;
}

_Vector4 _Matrix::TransformPosition(const _Vector& v) const
{
	return TransformVector4(_Vector4(v, 1.0f));
}

_Vector4 _Matrix::TransformVector(const _Vector& v) const
{
	return TransformVector4(_Vector4(v, 0.0f));
}

_Vector _Matrix::InverseTransformPosition(const _Vector& v) const
{
	_Matrix inv = GetInverseFast();
	return inv.TransformPosition(v);
}

_Vector4 _Matrix::InverseTransformVector(const _Vector& v) const
{
	_Matrix inv = GetInverseFast();
	return inv.TransformVector(v);
}


_Matrix _Matrix::GetTransposeAdjoint() const
{
	_Matrix rs;
	rs.M[0][0] = M[1][1] * M[2][2] - M[1][2] * M[2][1];
	rs.M[0][1] = M[1][2] * M[2][0] - M[1][0] * M[2][2];
	rs.M[0][2] = M[1][0] * M[2][1] - M[1][1] * M[2][0];
	rs.M[0][3] = 0.0f;

	rs.M[1][0] = M[2][1] * M[0][2] - M[2][2] * M[0][1];
	rs.M[1][1] = M[2][2] * M[0][0] - M[2][0] * M[0][2];
	rs.M[1][2] = M[2][0] * M[0][1] - M[2][1] * M[0][0];
	rs.M[1][3] = 0.0f;

	rs.M[2][0] = M[0][1] * M[1][2] - M[0][2] * M[1][1];
	rs.M[2][1] = M[0][2] * M[1][0] - M[0][0] * M[1][2];
	rs.M[2][2] = M[0][0] * M[1][1] - M[0][1] * M[1][0];
	rs.M[2][3] = 0.0f;

	rs.M[3][0] = 0.0f;
	rs.M[3][1] = 0.0f;
	rs.M[3][2] = 0.0f;
	rs.M[3][3] = 1.0f;

	return rs;
}

float _Matrix::Determinant() const
{
	return  M[0][0] * (
		M[1][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
		M[2][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) +
		M[3][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2])
		) -
		M[1][0] * (
			M[0][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
			M[2][1] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
			M[3][1] * (M[0][2] * M[2][3] - M[0][3] * M[2][2])
			) +
		M[2][0] * (
			M[0][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) -
			M[1][1] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
			M[3][1] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
			) -
		M[3][0] * (
			M[0][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2]) -
			M[1][1] * (M[0][2] * M[2][3] - M[0][3] * M[2][2]) +
			M[2][1] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
			);
}

FORCEINLINE float _Matrix::RotDeterminant() const
{
	return
		M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1]) -
		M[1][0] * (M[0][1] * M[2][2] - M[0][2] * M[2][1]) +
		M[2][0] * (M[0][1] * M[1][2] - M[0][2] * M[1][1]);
}

_Matrix _Matrix::GetInverseFast() const
{
#if defined(_DEBUG) || defined(DEBUG)
	if (GetScaledAxis(0).IsNearlyZero() &&
		GetScaledAxis(1).IsNearlyZero() &&
		GetScaledAxis(2).IsNearlyZero()
		)
	{
		assert(false);
	}
	else
	{
		const float det = Determinant();
		if (det == 0.0f || !GMath::IsFinite(det))
		{
			assert(false);
		}
	}
#endif
	_Matrix rs;
	SSE::MatrixInverse(&rs, this);
	return rs;
}

_Matrix _Matrix::GetInverse() const
{
	_Matrix rs;
	if (GetScaledAxis(0).IsNearlyZero() &&
		GetScaledAxis(1).IsNearlyZero() &&
		GetScaledAxis(2).IsNearlyZero()
		)
	{
		return Identity;
	}
	else
	{
		const float det = Determinant();
		if (det == 0.0f || !GMath::IsFinite(det))
		{
			return Identity;
		}
		else
		{
			SSE::MatrixInverse(&rs, this);
		}
	}
	return rs;
}

_Vector _Matrix::GetScaledAxis(uint32 index) const
{
	assert(index >= 0 && index < 3);
	return _Vector(M[index][0], M[index][1], M[index][2]);
}

void _Matrix::GetScaledAxis(_Vector& X, _Vector& Y, _Vector& Z) const
{
	X.x = M[0][0]; X.y = M[0][1]; X.z = M[0][2];
	Y.x = M[1][0]; Y.y = M[1][1]; Y.z = M[1][2];
	Z.x = M[2][0]; Z.y = M[2][1]; Z.z = M[2][2];
}

_Vector _Matrix::GetUnitAxis(uint32 index) const
{
	assert(index >= 0 && index < 3);
	return _Vector(M[index][0], M[index][1], M[index][2]).SafeNormalize();
}

void _Matrix::GetUnitAxis(_Vector& X, _Vector& Y, _Vector& Z) const
{
	GetScaledAxis(X, Y, Z);
	X.Normalize();
	Y.Normalize();
	Z.Normalize();
}

void _Matrix::SetAxis(uint32 index, const _Vector& axis)
{
	assert(index >= 0 && index < 3);
	M[index][0] = axis.x;
	M[index][1] = axis.y;
	M[index][2] = axis.z;
}

void _Matrix::SetOrigin(const _Vector& origin)
{
	M[3][0] = origin.x;
	M[3][1] = origin.y;
	M[3][2] = origin.z;
}

void _Matrix::SetAxes(_Vector* XAxis, _Vector* YAxis, _Vector* ZAxis, _Vector* Origin)
{
	if (XAxis)
	{
		M[0][0] = XAxis->x;
		M[0][1] = XAxis->y;
		M[0][2] = XAxis->z;
	}
	if (YAxis)
	{
		M[1][0] = YAxis->x;
		M[1][1] = YAxis->y;
		M[1][2] = YAxis->z;
	}
	if (ZAxis)
	{
		M[2][0] = ZAxis->x;
		M[2][1] = ZAxis->y;
		M[2][2] = ZAxis->z;
	}
	if (Origin)
	{
		M[3][0] = Origin->x;
		M[3][1] = Origin->y;
		M[3][2] = Origin->z;
	}
}

_Vector _Matrix::GetColumn(uint32 index) const
{
	assert(index >= 0 && index < 4);
	return _Vector(M[0][index], M[1][index], M[2][index]);
}

void _Matrix::SetColumn(uint32 index, const _Vector& v)
{
	assert(index >= 0 && index < 4);
	M[0][index] = v.x;
	M[1][index] = v.y;
	M[2][index] = v.z;
}

void _Matrix::RemoveScaling(float epsilon)
{
	const float sqrSumX = GMath::Square(M[0][0]) + GMath::Square(M[0][1]) + GMath::Square(M[0][2]);
	const float sqrSumY = GMath::Square(M[1][0]) + GMath::Square(M[1][1]) + GMath::Square(M[1][2]);
	const float sqrSumZ = GMath::Square(M[2][0]) + GMath::Square(M[2][1]) + GMath::Square(M[2][2]);
	const float scaleX = GMath::Select(sqrSumX - epsilon, GMath::RSqrt(sqrSumX), 1.0f);
	const float scaleY = GMath::Select(sqrSumY - epsilon, GMath::RSqrt(sqrSumY), 1.0f);
	const float scaleZ = GMath::Select(sqrSumZ - epsilon, GMath::RSqrt(sqrSumZ), 1.0f);
	M[0][0] *= scaleX;
	M[0][1] *= scaleX;
	M[0][2] *= scaleX;

	M[1][0] *= scaleY;
	M[1][1] *= scaleY;
	M[1][2] *= scaleY;

	M[2][0] *= scaleZ;
	M[2][1] *= scaleZ;
	M[2][2] *= scaleZ;
}

_Matrix _Matrix::GetMatrixNoScale(float epsilon) const
{
	_Matrix rs = *this;
	rs.RemoveScaling(epsilon);
	return rs;
}

_Vector _Matrix::ExtractScaling(float epsilon)
{
	_Vector rs(0, 0, 0);
	const float sqrSumX = GMath::Square(M[0][0]) + GMath::Square(M[0][1]) + GMath::Square(M[0][2]);
	const float sqrSumY = GMath::Square(M[1][0]) + GMath::Square(M[1][1]) + GMath::Square(M[1][2]);
	const float sqrSumZ = GMath::Square(M[2][0]) + GMath::Square(M[2][1]) + GMath::Square(M[2][2]);
	if (sqrSumX > epsilon)
	{
		float scaleX = GMath::Sqrt(sqrSumX);
		rs.x = scaleX;
		scaleX = 1.0f / scaleX;
		M[0][0] *= scaleX;
		M[0][1] *= scaleX;
		M[0][2] *= scaleX;
	}

	if (sqrSumY > epsilon)
	{
		float scaleY = GMath::Sqrt(sqrSumY);
		rs.y = scaleY;
		scaleY = 1.0f / scaleY;
		M[1][0] *= scaleY;
		M[1][1] *= scaleY;
		M[1][2] *= scaleY;
	}

	if (sqrSumZ > epsilon)
	{
		float scaleZ = GMath::Sqrt(sqrSumZ);
		rs.y = scaleZ;
		scaleZ = 1.0f / scaleZ;
		M[2][0] *= scaleZ;
		M[2][1] *= scaleZ;
		M[2][2] *= scaleZ;
	}

	return rs;
}

_Vector _Matrix::GetScaling(float epsilon) const
{
	_Vector rs(1);
	const float sqrSumX = GMath::Square(M[0][0]) + GMath::Square(M[0][1]) + GMath::Square(M[0][2]);
	const float sqrSumY = GMath::Square(M[1][0]) + GMath::Square(M[1][1]) + GMath::Square(M[1][2]);
	const float sqrSumZ = GMath::Square(M[2][0]) + GMath::Square(M[2][1]) + GMath::Square(M[2][2]);

	if (sqrSumX > epsilon)rs.x = sqrSumX; else rs.x = 0;
	if (sqrSumY > epsilon)rs.y = sqrSumY; else rs.y = 0;
	if (sqrSumZ > epsilon)rs.z = sqrSumZ; else rs.z = 0;

	return rs;
}

_Matrix _Matrix::GetMatrixNoTranslation() const
{
	return _Matrix(
		M[0][0], M[0][1], M[0][2], M[0][3],
		M[1][0], M[1][1], M[1][2], M[1][3],
		M[2][0], M[2][1], M[2][2], M[2][3],
		0.0f, 0.0f, 0.0f, M[3][3]

	);
}

_Matrix _Matrix::GetTranslatedMatrix(const _Vector& translation) const
{
	_Matrix rs(*this);
	rs.M[3][0] += translation.x;
	rs.M[3][1] += translation.y;
	rs.M[3][2] += translation.z;
	return rs;
}

bool _Matrix::ContainNan() const
{
	for (uint32 x = 0; x < 4; ++x)
	{
		for (uint32 y = 0; y < 4; ++y)
		{
			if (!GMath::IsFinite(M[x][y]))
			{
				return true;
			}
		}
	}
	return false;
}

void _Matrix::ScaleTranslation(const _Vector& scale)
{
	M[3][0] *= scale.x;
	M[3][1] *= scale.y;
	M[3][2] *= scale.z;
}

_Vector _Matrix::GetOrigin() const
{
	return _Vector(M[3][0], M[3][1], M[3][2]);
}

_Rotator _Matrix::ToRotator() const
{
	_Vector XAxis, YAxis, ZAxis;
	GetScaledAxis(XAxis, YAxis, ZAxis);
	_Rotator rs = _Rotator(
		0,
		GMath::Atan2(XAxis.z, GMath::Sqrt(GMath::Sqrt(XAxis.x) + GMath::Sqrt(XAxis.y))) * GMath::Rad2Deg,
		GMath::Atan2(XAxis.y, XAxis.x) * GMath::Rad2Deg
	);
	//TODO: Calc roll
	rs.CheckNan();
	return rs;
}

FORCEINLINE TransformMatrix::TransformMatrix(const _Rotator& rot, const _Vector& scale, const _Vector& origin)
{
	using namespace SSE;
	const VectorRegister angles = MakeVectorRegister(rot.Roll, rot.Pitch, rot.Yaw, 0.0f);
	const VectorRegister halfAngles = VectorMultiply(angles, SSEMathConstant::DEG_TO_RAD);

	union { VectorRegister v; float f[4]; }sinAngles, cosAngles;
	VectorSinCos(&sinAngles.v, &cosAngles.v, &halfAngles);
	const float sr = sinAngles.f[0];
	const float sp = sinAngles.f[1];
	const float sy = sinAngles.f[2];
	const float cr = cosAngles.f[0];
	const float cp = cosAngles.f[1];
	const float cy = cosAngles.f[2];

	M[0][0] = scale.z * cy * cp;
	M[0][1] = scale.z * sy * cp;
	M[0][2] = scale.z * sp;
	M[0][3] = 0.0f;

	M[1][0] = scale.z * sr * sp * cy - cr * sy;
	M[1][1] = scale.z * sr * sp * sy + cr * cy;
	M[1][2] = -scale.z * sr * cp;
	M[1][3] = 0.0f;

	M[2][0] = scale.z * -(cr * sp * cy + sr * sy);
	M[2][1] = scale.z * cy * sr - cr * sp * sy;
	M[2][2] = scale.z * cr * cp;
	M[2][3] = 0.0f;

	M[3][0] = origin.x;
	M[3][1] = origin.y;
	M[3][2] = origin.z;
	M[3][3] = 1.0f;
}

FORCEINLINE TransformMatrix::TransformMatrix(const _Rotator& rot, const _Vector& origin)
{
	using namespace SSE;
	const VectorRegister angles = MakeVectorRegister(rot.Roll, rot.Pitch, rot.Yaw, 0.0f);
	const VectorRegister halfAngles = VectorMultiply(angles, SSEMathConstant::DEG_TO_RAD);

	union { VectorRegister v; float f[4]; }sinAngles, cosAngles;
	VectorSinCos(&sinAngles.v, &cosAngles.v, &halfAngles);
	const float sr = sinAngles.f[0];
	const float sp = sinAngles.f[1];
	const float sy = sinAngles.f[2];
	const float cr = cosAngles.f[0];
	const float cp = cosAngles.f[1];
	const float cy = cosAngles.f[2];

	M[0][0] = cy * cp;
	M[0][1] = sy * cp;
	M[0][2] = sp;
	M[0][3] = 0.0f;

	M[1][0] = sr * sp * cy - cr * sy;
	M[1][1] = sr * sp * sy + cr * cy;
	M[1][2] = -sr * cp;
	M[1][3] = 0.0f;

	M[2][0] = -(cr * sp * cy + sr * sy);
	M[2][1] = cy * sr - cr * sp * sy;
	M[2][2] = cr * cp;
	M[2][3] = 0.0f;

	M[3][0] = origin.x;
	M[3][1] = origin.y;
	M[3][2] = origin.z;
	M[3][3] = 1.0f;

}

_Matrix TransformMatrix::Make(const _Rotator& rotation, const _Vector& scale, const _Vector& origin)
{
	return TransformMatrix(rotation, scale, origin);
}

_Matrix TransformMatrix::Make(const _Rotator& rotation, const _Vector& origin)
{
	return TransformMatrix(rotation, origin);
}

RotationMatrix::RotationMatrix(const _Rotator& rot)
	:
	TransformMatrix(rot, _Vector::ZeroVector)
{

}

_Matrix RotationMatrix::Make(const _Rotator& rot)
{
	return RotationMatrix(rot);
}


QuatTransformMatrix::QuatTransformMatrix(const _Quaternion& q, const _Vector& scale, const _Vector& origin)
{
#if defined(_DEBUG) || defined(DEBUG)
	//TODO: check quaternion is normalized
#endif
	const float x2 = q.x + q.x; const float y2 = q.y + q.y; const float z2 = q.z + q.z;
	const float xx = q.x * x2; const float xy = q.x * y2; const float xz = q.x * z2;
	const float yy = q.y * y2; const float yz = q.y * z2; const float zz = q.z * z2;
	const float wx = q.w * x2; const float wy = q.w * y2; const float wz = q.w * z2;

	M[0][0] = 1.0f - (yy + zz);		M[1][0] = xy - wz;			M[2][0] = xz + wy;			M[3][0] = origin.x;
	M[0][1] = xy + wz;				M[1][1] = 1.0f - (xx + zz);	M[2][1] = yz + wx;			M[3][1] = origin.y;
	M[0][2] = xz - wy;				M[1][2] = yz + wx;			M[2][2] = 1.0f - (xx + yy);	M[3][2] = origin.z;
	M[0][3] = 0.0f;					M[1][3] = 0.0f;				M[2][3] = 0.0f;				M[3][3] = 1.0f;
}


QuatTransformMatrix::QuatTransformMatrix(const _Quaternion& q, const _Vector& origin)
{

}



LookFromMatrix::LookFromMatrix(const _Vector& eyePos, const _Vector& lookDir, const _Vector& upVector)
{
	_Vector forward = lookDir.SafeNormalize();
	_Vector right = Cross(upVector, forward).SafeNormalize();
	_Vector up = Cross(forward, right);

	M[0][0] = forward.x;		 M[0][1] = right.x;			M[0][2] = up.x;			M[0][3] = 0;
	M[1][0] = forward.y;		 M[1][1] = right.y;			M[1][2] = up.y;			M[1][3] = 0;
	M[2][0] = forward.z;		 M[2][1] = right.z;			M[2][2] = up.z;			M[2][3] = 0;
	M[3][0] = forward | -eyePos; M[3][1] = right | -eyePos;	M[3][2] = up | -eyePos;	M[3][3] = 1;
}

FORCEINLINE _Vector _Rotator::ToVector() const
{
#if CHECK_NAN
	if (GMath::Abs(Roll) > FLOAT_NON_FRACTIONAL ||
		GMath::Abs(Pitch) > FLOAT_NON_FRACTIONAL ||
		GMath::Abs(Yaw) > FLOAT_NON_FRACTIONAL
		)
	{
	}
#endif
	const float pitchNoWinding = GMath::FMod(Pitch, 360);
	const float yawNoWinding = GMath::FMod(Yaw, 360);
	float cp, sp, cy, sy;
	GMath::SinCos(&sp, &cp, GMath::DegreeToRadians(pitchNoWinding));
	GMath::SinCos(&sy, &cy, GMath::DegreeToRadians(yawNoWinding));
	_Vector v(cp * cy, cp * sy, sp);
#if CHECK_NAN
	if (v.HasNan())
	{
		v = _Vector::ForwardVector;
	}
#endif

	return v;
}

_Vector _Rotator::ToEuler() const
{
	return _Vector(Roll, Pitch, Yaw);
}

_Quaternion _Rotator::ToQuaternion() const
{
	CheckNan();
	using namespace SSE;
	const uint32 neg = uint32(1 << 31);
	const uint32 pos = uint32(0);
	const VectorRegister angles = MakeVectorRegister(Roll, Pitch, Yaw, 0.0f);
	const VectorRegister anglesNoWinding = VectorMod(angles, SSEMathConstant::Float360);
	const VectorRegister halfAngles = VectorMultiply(anglesNoWinding, SSEMathConstant::DEG_TO_RAD_HALF);
	VectorRegister sinAngles, cosAngles;
	VectorSinCos(&sinAngles, &cosAngles, &halfAngles);

	const VectorRegister sr = VectorReplicate(sinAngles, 0);
	const VectorRegister cr = VectorReplicate(cosAngles, 0);

	const VectorRegister sysycycyTmp = VectorShuffle(sinAngles, cosAngles, 2, 2, 2, 2);
	const VectorRegister spspcpcp = VectorShuffle(sinAngles, cosAngles, 1, 1, 1, 1);
	const VectorRegister sycysycy = VectorShuffle(sysycycyTmp, sysycycyTmp, 0, 2, 0, 2);
	const VectorRegister cpcpspsp = VectorShuffle(cosAngles, sinAngles, 1, 1, 1, 1);
	const VectorRegister cysycysy = VectorShuffle(sysycycyTmp, sysycycyTmp, 2, 0, 2, 0);

	const VectorRegister signBitsLeft = MakeVectorRegister(pos, neg, pos, pos);
	const VectorRegister signBitsRight = MakeVectorRegister(neg, neg, neg, pos);
	const VectorRegister leftTerm = VectorBitwiseXor(signBitsLeft, VectorMultiply(cr, VectorMultiply(spspcpcp, sycysycy)));
	const VectorRegister rightTerm = VectorBitwiseXor(signBitsRight, VectorMultiply(sr, VectorMultiply(cpcpspsp, cysycysy)));
	_Quaternion quat;
	const VectorRegister rs = VectorAdd(leftTerm, rightTerm);
	VectorStoreAligned(rs, &quat);
	if (quat.HasNan())
	{
		quat = _Quaternion::Identity;
	}
	return quat;
}


bool _Quaternion::HasNan() const
{
	return !(GMath::IsFinite(x) && GMath::IsFinite(y) && GMath::IsFinite(z) && GMath::IsFinite(w));
}


_Vector _Rotator::RotatorVector(const _Vector& v) const
{
	return RotationMatrix(*this).TransformVector(v);
}

_Vector _Rotator::UnrotatorVector(const _Vector& v) const
{
	return RotationMatrix(*this).GetTransposed().TransformVector(v);
}

_Rotator _Rotator::FromEuler(const _Vector& euler)
{
	return _Rotator(euler.x, euler.y, euler.z);
}

FORCEINLINE void _Rotator::Normalize()
{
	using namespace SSE;
	VectorRegister r = VectorLoadFloat3_W0(this);
	r = VectorNormalizeRotator(r);
	VectorStoreFloat3(r, this);
	CheckNan();
}

/// <returns>lerped angle between a and b, takes the shortest path</returns>
template<typename U>
FORCEINLINE constexpr _Rotator GMath::Lerp(const _Rotator& A, const _Rotator& B, const U& Alpha)
{
	return A + (B-A).GetNormalized()*Alpha;
}

/// <returns>lerped angle between a and b, allows interpolation over 180 degrees</returns>
template<typename U>
FORCEINLINE constexpr _Rotator GMath::LerpRange(const _Rotator& A, const _Rotator& B, const U& Alpha)
{
	return (A + (B-A)*Alpha).GetNormalized();
}


FORCEINLINE _Quaternion::_Quaternion(const VectorRegister& v)
{
	VectorStoreAligned(v, this);
	CheckNan();
}

FORCEINLINE _Quaternion::_Quaternion(const _Matrix& M)
{
	if (M.GetScaledAxis(0).IsNearlyZero() || M.GetScaledAxis(1).IsNearlyZero() || M.GetScaledAxis(2).IsNearlyZero())
	{
		*this = _Quaternion::Identity;
		return;
	}

#if defined(_DEBUG) || defined(DEBUG)
	if( (GMath::Abs(1 - M.GetScaledAxis(0).LengthSquare()) <= KINDA_SMALL_NUMBER) &&
		(GMath::Abs(1 - M.GetScaledAxis(1).LengthSquare()) <= KINDA_SMALL_NUMBER) &&
		(GMath::Abs(1 - M.GetScaledAxis(2).LengthSquare()) <= KINDA_SMALL_NUMBER)
		)
	{
		*this = _Quaternion::Identity;
		return;
	}
#endif

	float s;
	const float tr = M.M[0][0] + M.M[1][1] + M.M[2][2];
	if (tr > 0.0f)
	{
		float invS = GMath::RSqrt(tr + 1);
		w = 0.5f * (1 / invS);
		s = 0.5f * invS;
		x = (M.M[1][2] - M.M[2][1]) * s;
		y = (M.M[2][0] - M.M[0][2]) * s;
		z = (M.M[0][1] - M.M[1][0]) * s;
	}
	else
	{
		int32 i = 0;
		if (M.M[1][1] > M.M[0][0]) i = 1;
		if (M.M[2][2] > M.M[i][i]) i = 2;
		static const int32 nxt[3] = { 1,2,0 };
		const int32 j = nxt[i];
		const int32 k = nxt[j];
		s = M.M[i][i] - M.M[j][j] - M.M[k][k] + 1.0f;
		float invS = GMath::RSqrt(s);
		float qt[4];
		qt[i] = 0.5f * (1 / invS);
		s = 0.5f * invS;
		qt[3] = (M.M[j][k] - M.M[k][j]) * s;
		qt[j] = (M.M[i][j] - M.M[j][i]) * s;
		qt[k] = (M.M[i][k] - M.M[k][i]) * s;
		x = qt[0];
		y = qt[1];
		z = qt[2];
		w = qt[3];
	}
	CheckNan();
}

FORCEINLINE _Quaternion::_Quaternion(const _Rotator& R)
{
	*this = R.ToQuaternion();
	CheckNan();
}

FORCEINLINE _Quaternion::_Quaternion(const _Vector& axis, float angle)
{
	const float half_angle = 0.5f * angle;
	float s, c;
	GMath::SinCos(&s, &c, half_angle);

	x = s * axis.x;
	y = s * axis.y;
	z = s * axis.z;
	w = c;
	CheckNan();
}

FORCEINLINE _Quaternion _Quaternion::operator+(const _Quaternion& q) const
{
	VectorRegister a = VectorLoadAligned(this);
	VectorRegister b = VectorLoadAligned(&q);
	return _Quaternion(VectorAdd(a,b));
}

FORCEINLINE _Quaternion _Quaternion::operator+=(const _Quaternion& q)
{
	VectorRegister a = VectorLoadAligned(this);
	VectorRegister b = VectorLoadAligned(&q);
	VectorStoreAligned(VectorAdd(a, b), this);
	CheckNan();
	return *this;
}

FORCEINLINE _Quaternion _Quaternion::operator-(const _Quaternion& q) const
{
	VectorRegister a = VectorLoadAligned(this);
	VectorRegister b = VectorLoadAligned(&q);
	return _Quaternion(VectorSubtract(a, b));
}

FORCEINLINE _Quaternion _Quaternion::operator-=(const _Quaternion& q)
{
	VectorRegister a = VectorLoadAligned(this);
	VectorRegister b = VectorLoadAligned(&q);
	VectorStoreAligned(VectorSubtract(a, b), this);
	CheckNan();
	return *this;
}

FORCEINLINE bool _Quaternion::Equals(const _Quaternion& q, float epsilon) const
{
	using namespace SSE;
	const VectorRegister epsilonV = VectorLoadFloat1(&epsilon);
	const VectorRegister a = VectorLoadAligned(this);
	const VectorRegister b = VectorLoadAligned(&q);
	const VectorRegister rotSub = VectorAbs(VectorSubtract(a, b));
	const VectorRegister rotAdd = VectorAbs(VectorAdd(a, b));
	return !VectorAnyGreaterThan(rotSub, epsilonV) || !VectorAnyGreaterThan(rotAdd, epsilonV);
}

FORCEINLINE bool _Quaternion::IsIdentity(float epsilon) const
{
	return Equals(_Quaternion::Identity, epsilon);
}

FORCEINLINE _Quaternion _Quaternion::operator*(const _Quaternion& q) const
{
	_Quaternion rs;
	SSE::VectorQuaternionMultiply(&rs, this, &q);
	rs.CheckNan();
	return rs;
}

FORCEINLINE _Quaternion _Quaternion::operator*=(const _Quaternion& q)
{
	VectorRegister a = VectorLoadAligned(this);
	VectorRegister b = VectorLoadAligned(&q);
	VectorRegister rs;
	SSE::VectorQuaternionMultiply(&rs, &a, &b);
	VectorStoreAligned(rs, this);
	CheckNan();
	return *this;
}

FORCEINLINE _Vector _Quaternion::RotateVector(const _Vector& v) const
{
	const _Vector q(x, y, z);
	const _Vector t = 2 * (q ^ v);
	const _Vector rs = v + (w * t) + (q ^ t);
	return rs;
}

FORCEINLINE _Vector _Quaternion::UnrotateVector(const _Vector& v) const
{
	const _Vector q(-x, -y, -z);
	const _Vector t = 2 * (q ^ v);
	const _Vector rs = v + (w * t) + (q ^ t);
	return rs;
}

void _Quaternion::ToSwingTwist(const _Vector& inTwistAxis, _Quaternion& outSwing, _Quaternion& outTwist) const
{
	_Vector proj = (inTwistAxis | _Vector(x, y, z)) * inTwistAxis;
	outTwist = _Quaternion(proj.x, proj.y, proj.z, w);
	if (outTwist.SizeSquared() == 0.0f)
	{
		outTwist = _Quaternion::Identity;
	}
	else
	{
		outTwist.Normalize();
	}
	outSwing = *this * outTwist.Inverse();
}

float _Quaternion::GetTwistAngle(const _Vector& twistAxis) const
{
	float xyz = twistAxis | _Vector(x, y, z);
	return GMath::UnwindRadians(2.0f * GMath::Atan2(xyz, w));
}

FORCEINLINE _Quaternion _Quaternion::Inverse() const
{
	assert(IsNormalized());
	return _Quaternion(VectorQuaternionInverse(VectorLoadAligned(this)));
}

void _Quaternion::Normalize(float epsilon)
{
	using namespace SSE;
	const VectorRegister v = VectorLoadAligned(this);
	const VectorRegister sqrSum = VectorDot4(v, v);
	const VectorRegister nonZeroMask = VectorCompareGE(sqrSum, VectorLoadFloat1(&epsilon));
	const VectorRegister invLength = VectorReciprocalSqrtAccurate(sqrSum);
	const VectorRegister nv = VectorMultiply(invLength, v);
	VectorRegister rs = VectorSelect(nonZeroMask, nv, SSEMathConstant::Float0001);
	VectorStoreAligned(rs, this);
}

_Quaternion _Quaternion::GetNormalized(float epsilon) const
{
	_Quaternion rs(*this);
	rs.Normalize();
	return rs;
}

bool _Quaternion::IsNormalized() const
{
	using namespace SSE;
	VectorRegister a = VectorLoadAligned(this);
	VectorRegister t = VectorAbs(VectorSubtract(VectorOne(), VectorDot4(a, a)));
	return !VectorAnyGreaterThan(t, SSEMathConstant::ThreshQuatNormalized);
}

FORCEINLINE _Vector _Quaternion::operator*(const _Vector& v) const
{
	return RotateVector(v);
}

FORCEINLINE _Matrix _Quaternion::operator*(const _Matrix& m) const
{
	using namespace SSE;
	_Matrix rs;
	_Quaternion vt, vr;
	_Quaternion inv = Inverse();
	for (int32 i = 0; i < 4; ++i)
	{
		_Quaternion vq(m.M[i][0], m.M[i][1], m.M[i][2], m.M[i][3]);
		VectorQuaternionMultiply(&vt, this, &vq);
		VectorQuaternionMultiply(&vr, &vt, &inv);
		rs.M[i][0] = vr.x;
		rs.M[i][1] = vr.y;
		rs.M[i][2] = vr.z;
		rs.M[i][3] = vr.w;
	}
	return rs;
}

_Quaternion _Quaternion::operator*=(const float scale)
{
	VectorRegister a = VectorLoadAligned(this);
	VectorRegister b = VectorSetFloat1(scale);
	VectorStoreAligned(VectorMultiply(a, b), this);
	CheckNan();
	return *this;
}

_Quaternion _Quaternion::operator*(const float scale) const
{
	VectorRegister a = VectorLoadAligned(this);
	VectorRegister b = VectorSetFloat1(scale);
	return _Quaternion(VectorMultiply(a, b));
}

_Quaternion _Quaternion::operator/=(const float scale)
{
	VectorRegister a = VectorLoadAligned(this);
	VectorRegister b = VectorSetFloat1(scale);
	VectorStoreAligned(VectorDivide(a, b), this);
	CheckNan();
	return *this;
}

_Quaternion _Quaternion::operator/(const float scale) const
{
	VectorRegister a = VectorLoadAligned(this);
	VectorRegister b = VectorSetFloat1(scale);
	return _Quaternion(VectorDivide(a, b));
}


void _Quaternion::SetShortestArcWith(const _Quaternion& q)
{
	const float dotRs = (q | *this);
	const float bias = GMath::Select(dotRs, 1.0f, -1.0f);

	x *= bias;
	y *= bias;
	z *= bias;
	w *= bias;
}

_Quaternion _Quaternion::Log() const
{
	_Quaternion rs;
	rs.w = 0.0f;
	if (GMath::Abs(w) < 1.0f)
	{
		const float angle = GMath::Acos(w);
		const float sinAngle = GMath::Sin(angle);

		if (GMath::Abs(sinAngle) >= SMALL_NUMBER)
		{
			const float scale = angle / sinAngle;
			rs.x = scale * x;
			rs.y = scale * y;
			rs.z = scale * z;
			return rs;
		}
	}

	rs.x = x;
	rs.y = y;
	rs.z = z;
	return rs;
}

_Quaternion _Quaternion::Exp() const
{
	const float angle = GMath::Sqrt(x * x + y * y + z * z);
	const float sinAngle = GMath::Sin(angle);
	_Quaternion rs;
	rs.w = GMath::Cos(angle);
	if (GMath::Abs(sinAngle) >= SMALL_NUMBER)
	{
		const float scale = sinAngle / angle;
		rs.x = scale * x;
		rs.y = scale * y;
		rs.z = scale * z;
	}
	else
	{
		rs.x = x;
		rs.y = y;
		rs.z = z;
	}
	return rs;
}

FORCEINLINE _Vector _Quaternion::GetAxisX() const
{
	return RotateVector(_Vector(1, 0, 0));
}

FORCEINLINE _Vector _Quaternion::GetAxisY() const
{
	return RotateVector(_Vector(0, 1, 0));
}

FORCEINLINE _Vector _Quaternion::GetAxisZ() const
{
	return RotateVector(_Vector(0, 0, 1));
}

FORCEINLINE _Vector _Quaternion::GetForwardVector() const
{
	return GetAxisX();
}

FORCEINLINE _Vector _Quaternion::GetRightVector() const
{
	return GetAxisY();
}

FORCEINLINE _Vector _Quaternion::GetUpVector() const
{
	return GetAxisZ();
}

FORCEINLINE _Vector _Quaternion::ToVector() const
{
	return GetAxisX();
}

FORCEINLINE _Vector _Quaternion::GetRotationAxis() const
{
	using namespace SSE;
	_Vector rs;
	VectorRegister a = VectorLoadAligned(this);
	VectorRegister b = VectorNormalizeSafe(VectorSet_W0(a), SSEMathConstant::Float1000);
	VectorStoreFloat3(b, &rs);
	return rs;
}

FORCEINLINE float _Quaternion::AngularDistance(const _Quaternion& q) const
{
	float p = x * q.x + y * q.y + z * q.z + w * q.w;
	return GMath::Acos( 2*p*p - 1 );
}

FORCEINLINE void _Quaternion::ToAxisAngle(_Vector& axis, float& angle) const
{
	axis = GetRotationAxis();
	angle = GetAngle();
}

_Rotator _Quaternion::ToRotator() const
{
	CheckNan();
	const float st = z * x - w * y;
	const float yawY = 2.0f * (w * z + x * y);
	const float yawX = (1.0f - 2.0f * (y * y + z * z));

	const float threshold_st = 0.4999995f;
	_Rotator rs;
	if (st < -threshold_st)
	{
		rs.Pitch = -90.0f;
		rs.Yaw = GMath::Atan2(yawY, yawX) * GMath::Rad2Deg;
		rs.Roll = _Rotator::NormalizeAngle(-rs.Yaw - (2 * GMath::Atan2(x, w) * GMath::Rad2Deg));
	}
	else if (st > threshold_st)
	{
		rs.Pitch = 90.0f;
		rs.Yaw = GMath::Atan2(yawY, yawX) * GMath::Rad2Deg;
		rs.Roll = _Rotator::NormalizeAngle(rs.Yaw - (2 * GMath::Atan2(x, w) * GMath::Rad2Deg));
	}
	else
	{
		rs.Pitch = GMath::FastASin(2*st) * GMath::Rad2Deg;
		rs.Yaw = GMath::Atan2(yawY, yawX) * GMath::Rad2Deg;
		rs.Roll = GMath::Atan2(-2 * (w * x + y * z), (1 - 2 * (x * x + y + y))) * GMath::Rad2Deg;
	}
	return rs;
}

FORCEINLINE _Quaternion FindBetween_Helper(const _Vector& A, const _Vector& B, float NormAB)
{
	float w = NormAB + (A | B);
	_Quaternion rs;
	if (w >= 1e-6f * NormAB)
	{
		rs = _Quaternion(
			A.y * B.z - A.z * B.y,
			A.z * B.x - A.x * B.z,
			A.x * B.y - A.y * B.x,
			w);
	}
	else
	{
		w = 0;
		rs = GMath::Abs(A.x) > GMath::Abs(A.y) ?
			_Quaternion(-A.z, 0.0f, A.x, w) :
			_Quaternion(0.0f, -A.z, A.y, w);
	}
	rs.Normalize();
	return rs;
}

FORCEINLINE _Quaternion _Quaternion::FindBetweenNormals(const _Vector& v1, const _Vector& v2)
{
	return FindBetween_Helper(v1, v2, 1.0f);
}

FORCEINLINE _Quaternion _Quaternion::FindBetweenVectors(const _Vector& v1, const _Vector& v2)
{
	return FindBetween_Helper(
		v1, 
		v2, 
		GMath::Sqrt(v1.LengthSquare() * v2.LengthSquare())
	);
}

FORCEINLINE float _Quaternion::Error(const _Quaternion& q1, const _Quaternion& q2)
{
	const float cosom = GMath::Abs(q1 | q2);
	return GMath::Abs(cosom) < 0.9999999f ? GMath::Acos(cosom) * INV_PI : 0.0f;
}

FORCEINLINE float _Quaternion::ErrorAutoNormalize(const _Quaternion& q1, const _Quaternion& q2)
{
	_Quaternion a = q1;
	a.Normalize();
	_Quaternion b = q2;
	b.Normalize();
	return _Quaternion::Error(a,b);
}

_Quaternion _Quaternion::FastLerp(const _Quaternion& A, const _Quaternion& B, const float Alpha)
{
	const float dotRs = A | B;
	const float bias = GMath::Select(dotRs, 1.0f, -1.0f);
	return (B * Alpha) + (A * (bias * (1.0f - Alpha)));
}

_Quaternion _Quaternion::FastBilerp(const _Quaternion& p00, const _Quaternion& p01, const _Quaternion& p10, const _Quaternion& p11, const float FracX, const float FracY)
{
	return _Quaternion::FastLerp(
		_Quaternion::FastLerp(p00, p01, FracX),
		_Quaternion::FastLerp(p10, p11, FracX),
		FracY
	);
}

_Quaternion _Quaternion::Slerp_NoNormalization(const _Quaternion& q1, const _Quaternion& q2, float slerp)
{
	const float rawCosom = q1 | q2;
	const float cosom = GMath::Select(rawCosom, rawCosom, -rawCosom);
	float scale0, scale1;
	if (cosom < 0.9999f)
	{
		const float omega = GMath::Acos(cosom);
		const float invSin = 1.0f / GMath::Sin(omega);
		scale0 = GMath::Sin((1.0f - slerp) * omega) * invSin;
		scale1 = GMath::Sin(slerp * omega) * invSin;
	}
	else
	{
		scale0 = 1.0f - slerp;
		scale1 = slerp;
	}

	scale1 = GMath::Select(rawCosom, scale1, -scale1);
	_Quaternion rs;
	rs.x = scale0 * q1.x + scale1 * q2.x;
	rs.y = scale0 * q1.y + scale1 * q2.y;
	rs.z = scale0 * q1.z + scale1 * q2.z;
	rs.w = scale0 * q1.w + scale1 * q2.w;
	return rs;
}

_Quaternion _Quaternion::SlerpFullPath_NoNormalization(const _Quaternion& q1, const _Quaternion& q2, float slerp)
{
	const float cosAngle = GMath::Clamp(q1 | q2, -1.0f, 1.0f);
	const float angle = GMath::Acos(cosAngle);
	if (GMath::Abs(angle) < KINDA_SMALL_NUMBER)
	{
		return q1;
	}
	const float sinAngle = GMath::Sin(angle);
	const float invSinAngle = 1.0f / sinAngle;

	const float scale0 = GMath::Sin((1.0 - slerp) * angle) * invSinAngle;
	const float scale1 = GMath::Sin(slerp * angle) * invSinAngle;
	return q1*scale0 + q2*scale1;
}

_Quaternion _Quaternion::Squad(const _Quaternion& q1, const _Quaternion& t1, const _Quaternion& q2, const _Quaternion& t2, float alpha)
{
	const _Quaternion a = _Quaternion::Slerp_NoNormalization(q1, q2, alpha);
	const _Quaternion b = _Quaternion::SlerpFullPath_NoNormalization(t1, t2, alpha);
	return _Quaternion::SlerpFullPath(a, b, 2.0f * alpha * (1.0f-alpha));
}

_Quaternion _Quaternion::SquadFullPath(const _Quaternion& q1, const _Quaternion& t1, const _Quaternion& q2, const _Quaternion& t2, float alpha)
{
	const _Quaternion a = _Quaternion::SlerpFullPath_NoNormalization(q1, q2, alpha);
	const _Quaternion b = _Quaternion::SlerpFullPath_NoNormalization(t1, t2, alpha);
	return _Quaternion::SlerpFullPath(a, b, 2.0f * alpha * (1.0f - alpha));
}

void _Quaternion::CalcTangents(const _Quaternion& prevP, const _Quaternion& p, const _Quaternion& nextP, float tension, _Quaternion& outTan)
{
	const _Quaternion invP = p.Inverse();
	const _Quaternion part1 = (invP * prevP).Log();
	const _Quaternion part2 = (invP * nextP).Log();
	const _Quaternion preExp = (part1 + part2) * -0.5f;
	outTan = p * preExp.Exp();
}

FORCEINLINE bool _Quaternion::operator==(const _Quaternion& q) const
{
	const VectorRegister a = VectorLoadAligned(this);
	const VectorRegister b = VectorLoadAligned(&q);
	return VectorMaskBits(VectorCompareEQ(a,b)) == 0x0f;
}

FORCEINLINE bool _Quaternion::operator!=(const _Quaternion& q) const
{
	const VectorRegister a = VectorLoadAligned(this);
	const VectorRegister b = VectorLoadAligned(&q);
	return VectorMaskBits(VectorCompareNE(a, b)) != 0x00;
}

_Quaternion _Quaternion::FromEuler(const _Vector& euler)
{
	return _Rotator::FromEuler(euler).ToQuaternion();
}
