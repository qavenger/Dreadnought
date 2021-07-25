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
	M{	m00,m01,m02,m03,
		m10,m11,m12,m13,
		m20,m21,m22,m23,
		m30,m31,m32,m33	}
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
	return _Vector( M[0][index], M[1][index], M[2][index] );
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
		0.0f   , 0.0f	, 0.0f	 , M[3][3]

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

TransformMatrix::TransformMatrix(const _Vector& xAxis, const _Vector& yAxis, const _Vector& zAxis, const _Vector& origin)
	:
	_Matrix(
		xAxis.x, xAxis.y, xAxis.z, 0.0f,
		yAxis.x, yAxis.y, yAxis.z, 0.0f,
		zAxis.x, zAxis.y, zAxis.z, 0.0f,
		origin.x, origin.y, origin.z, 1.0f
	)
{
}

LookFromMatrix::LookFromMatrix(const _Vector& eyePos, const _Vector& lookDir, const _Vector& upVector)
{

}
