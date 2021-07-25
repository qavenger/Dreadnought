#pragma once
#include<EngineMath.h>
struct _Matrix
{
	_declspec(align(16)) float M[4][4];

	_declspec(align(16)) static const _Matrix Identity;

	std::string ToString()const;
	uint32 Hash()const;
	_Matrix() {}
	explicit _Matrix(bool)
	{
		M[0][0] = M[1][1] = M[2][2] = M[3][3] = 1;
		M[0][1] = M[0][2] = M[0][3] = 0;
		M[1][0] = M[1][2] = M[1][3] = 0;
		M[2][0] = M[2][1] = M[2][3] = 0;
		M[3][0] = M[3][1] = M[3][2] = 0;
	}
	_Matrix(const _Vector& x, const _Vector& y, const _Vector& z, const _Vector& w);
	_Matrix(const _Vector4& x, const _Vector4& y, const _Vector4& z, const _Vector4& w);
	_Matrix(float m00, float m01, float m02, float m03,
						float m10, float m11, float m12, float m13,
						float m20, float m21, float m22, float m23,
						float m30, float m31, float m32, float m33);

	void SetIdentity();

	_Matrix operator*(const _Matrix& m)const;
	void operator*=(const _Matrix& m);
	_Matrix operator+(const _Matrix& m)const;
	void operator+=(const _Matrix& m);
	_Matrix operator*(float other)const;
	void operator*=(float other);
	bool operator==(const _Matrix& m)const;
	bool IsEquals(const _Matrix& m, float epsilon = KINDA_SMALL_NUMBER)const;
	bool operator!=(const _Matrix& m)const;
	_Matrix& operator=(const _Matrix& m);
	_Vector4 TransformVector4(const _Vector4& v)const;
	_Vector4 TransformPosition(const _Vector& v)const;
	_Vector4 TransformVector(const _Vector& v)const;
	_Vector InverseTransformPosition(const _Vector& v)const;
	_Vector4 InverseTransformVector(const _Vector& v)const;
	_Matrix GetTransposed()const;
	_Matrix GetTransposeAdjoint()const;

	float Determinant()const;
	float RotDeterminant()const;

	_Matrix GetInverseFast()const;
	_Matrix GetInverse()const;

	_Vector GetScaledAxis(uint32 index)const;
	void GetScaledAxis(_Vector& X, _Vector& Y, _Vector& Z )const;
	_Vector GetUnitAxis(uint32 index)const;
	void GetUnitAxis(_Vector& X, _Vector& Y, _Vector& Z)const;

	void SetAxis(uint32 index, const _Vector& axis);
	void SetOrigin(const _Vector& origin);
	void SetAxes(_Vector* XAxis, _Vector* YAxis, _Vector* ZAxis, _Vector* Origin);

	_Vector GetColumn(uint32 index)const;
	void SetColumn(uint32 index, const _Vector& v);

	void RemoveScaling(float epsilon = SMALL_NUMBER);

	_Matrix GetMatrixNoScale(float epsilon = SMALL_NUMBER)const;
	_Vector ExtractScaling(float epsilon = SMALL_NUMBER);
	_Vector GetScaling(float epsilon = SMALL_NUMBER)const;

	_Matrix GetMatrixNoTranslation()const;
	_Matrix GetTranslatedMatrix(const _Vector& translation)const;

	bool ContainNan()const;
	void ScaleTranslation(const _Vector& scale);

	_Vector GetOrigin()const;

	_Rotator ToRotator()const;
};

struct TransformMatrix : public _Matrix
{
	TransformMatrix(const _Vector& xAxis, const _Vector& yAxis, const _Vector& zAxis, const _Vector& origin);
};

struct LookFromMatrix : public _Matrix
{
	LookFromMatrix(const _Vector& eyePos, const _Vector& lookDir, const _Vector& upVector);
};