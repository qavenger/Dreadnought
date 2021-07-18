#pragma once
#include<EngineMath.h>
struct _Matrix
{
	_declspec(align(16)) float M[4][4];

	_declspec(align(16)) static const _Matrix Identity;

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

	void SetIdentity();


};