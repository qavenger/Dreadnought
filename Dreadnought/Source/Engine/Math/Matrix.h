#pragma once
#include<EngineMath.h>
struct _Matrix
{
	_declspec(align(16)) float M[4][4];

	_declspec(align(16)) static const _Matrix Identity;

	std::string ToString()const;
	uint32 Hash()const;
	FORCEINLINE _Matrix() {}
	explicit FORCEINLINE _Matrix(bool)
	{
		M[0][0] = M[1][1] = M[2][2] = M[3][3] = 1;
		M[0][1] = M[0][2] = M[0][3] = 0;
		M[1][0] = M[1][2] = M[1][3] = 0;
		M[2][0] = M[2][1] = M[2][3] = 0;
		M[3][0] = M[3][1] = M[3][2] = 0;
	}
	FORCEINLINE _Matrix(_Vector x, _Vector y, _Vector z, _Vector w);
	FORCEINLINE _Matrix(_Vector4 x, _Vector4 y, _Vector4 z, _Vector4 w);
	FORCEINLINE _Matrix(float m00, float m01, float m02, float m03,
						float m10, float m11, float m12, float m13,
						float m20, float m21, float m22, float m23,
						float m30, float m31, float m32, float m33);

	void SetIdentity();

	FORCEINLINE _Matrix operator*(const _Matrix& m)const;
	FORCEINLINE void operator*=(const _Matrix& m);
	FORCEINLINE _Matrix operator+(const _Matrix& m)const;
	FORCEINLINE void operator+=(const _Matrix& m);
	FORCEINLINE _Matrix operator*(float other)const;
	FORCEINLINE void operator*=(float other);
	FORCEINLINE bool operator==(_Matrix m)const;
	FORCEINLINE bool IsEquals(_Matrix m, float epsilon = KINDA_SMALL_NUMBER)const;
	FORCEINLINE bool operator!=(_Matrix m)const;

	FORCEINLINE _Vector4 TransformVector4(const _Vector4& v)const;
	FORCEINLINE _Vector4 TransformPosition(const _Vector& v)const;
	FORCEINLINE _Vector4 TransformVector(const _Vector& v)const;
	FORCEINLINE _Vector InverseTransformPosition(const _Vector& v)const;
	FORCEINLINE _Vector4 InverseTransformVector(const _Vector& v)const;
	FORCEINLINE _Matrix GetTransposed()const;
	FORCEINLINE _Matrix GetTransposeAdjoint()const;

	FORCEINLINE float Determinant()const;
	FORCEINLINE float RotDeterminant()const;

	FORCEINLINE _Matrix GetInverseFast()const;
	FORCEINLINE _Matrix GetInverse()const;

	FORCEINLINE _Vector GetScaledAxis(uint32 index)const;
	FORCEINLINE _Vector GetScaledAxis(_Vector& X, _Vector& Y, _Vector& Z )const;
	FORCEINLINE _Vector GetUnitAxis(uint32 index)const;
	FORCEINLINE _Vector GetUnitAxis(_Vector& X, _Vector& Y, _Vector& Z)const;

	FORCEINLINE void SetAxis(uint32 index, const _Vector& axis);
	FORCEINLINE void SetOrigin(const _Vector& origin);
	FORCEINLINE void SetAxes(_Vector* XAxis, _Vector* YAxis, _Vector* ZAxis, _Vector* Origin);

	FORCEINLINE _Vector GetColumn(uint32 index)const;
	FORCEINLINE void SetColumn(uint32 index, const _Vector& v);

	FORCEINLINE void RemoveScaling(float epsilon = SMALL_NUMBER);

	FORCEINLINE _Matrix GetMatrixNoScale(float epsilon = SMALL_NUMBER)const;
	FORCEINLINE _Vector ExtractScaling(float epsilon = SMALL_NUMBER);
	FORCEINLINE _Vector GetScaling(float epsilon = SMALL_NUMBER)const;

	FORCEINLINE _Matrix GetMatrixNoTranslation()const;
	FORCEINLINE _Matrix GetTranslatedMatrix(const _Vector& translation)const;

	FORCEINLINE bool ContainNan()const;
	FORCEINLINE void ScaleTranslation(const _Vector& scale);

	FORCEINLINE _Vector GetOrigin()const;

	FORCEINLINE _Rotator ToRotator()const;
};

struct TransformMatrix : public _Matrix
{
	TransformMatrix(const _Vector& xAxis, const _Vector& yAxis, const _Vector& zAxis, const _Vector& origin);
};

struct LookFromMatrix : public _Matrix
{
	LookFromMatrix(const _Vector& eyePos, const _Vector& lookDir, const _Vector& upVector);
};