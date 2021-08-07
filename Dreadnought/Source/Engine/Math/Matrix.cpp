#include <stdafx.h>
#include <Matrix.h>
#include <MathUtil.h>

void _Matrix::SetIdentity()
{
	M[0][0] = 1; M[0][1] = 0; M[0][2] = 0; M[0][3] = 0;
	M[1][0] = 0; M[1][1] = 1; M[1][2] = 0; M[1][3] = 0;
	M[2][0] = 0; M[2][1] = 0; M[2][2] = 1; M[2][3] = 0;
	M[3][0] = 0; M[3][1] = 0; M[3][2] = 0; M[3][3] = 1;
}

_Matrix _Matrix::GetTransposed() const
{
	return _Matrix(
		M[0][0], M[1][0], M[2][0], M[3][0],
		M[0][1], M[1][1], M[2][1], M[3][1],
		M[0][2], M[1][2], M[2][2], M[3][2],
		M[0][3], M[1][3], M[2][3], M[3][3]
	);
}

_Matrix& _Matrix::operator=(const _Matrix& m)
{
	M[0][0] = m.M[0][0];	M[0][1] = m.M[0][1];	M[0][2] = m.M[0][2];	M[0][3] = m.M[0][3];
	M[1][0] = m.M[1][0];	M[1][1] = m.M[1][1];	M[1][2] = m.M[1][2];	M[1][3] = m.M[1][3];
	M[2][0] = m.M[2][0];	M[2][1] = m.M[2][1];	M[2][2] = m.M[2][2];	M[2][3] = m.M[2][3];
	M[3][0] = m.M[3][0];	M[3][1] = m.M[3][1];	M[3][2] = m.M[3][2];	M[3][3] = m.M[3][3];
	return *this;
}

_Matrix _Matrix::operator+(const _Matrix& m) const
{
	_Matrix result;
	result.M[0][0] = M[0][0] + m.M[0][0];  result.M[1][0] = M[1][0] + m.M[1][0];
	result.M[0][1] = M[0][1] + m.M[0][1];  result.M[1][1] = M[1][1] + m.M[1][1];
	result.M[0][2] = M[0][2] + m.M[0][2];  result.M[1][2] = M[1][2] + m.M[1][2];
	result.M[0][3] = M[0][3] + m.M[0][3];  result.M[1][3] = M[1][3] + m.M[1][3];

	result.M[2][0] = M[2][0] + m.M[2][0];  result.M[3][0] = M[3][0] + m.M[3][0];
	result.M[2][1] = M[2][1] + m.M[2][1];  result.M[3][1] = M[3][1] + m.M[3][1];
	result.M[2][2] = M[2][2] + m.M[2][2];  result.M[3][2] = M[3][2] + m.M[3][2];
	result.M[2][3] = M[2][3] + m.M[2][3];  result.M[3][3] = M[3][3] + m.M[3][3];
	return result;
}

void _Matrix::operator+=(const _Matrix& m)
{
	M[0][0] = M[0][0] + m.M[0][0];  M[1][0] = M[1][0] + m.M[1][0];
	M[0][1] = M[0][1] + m.M[0][1];  M[1][1] = M[1][1] + m.M[1][1];
	M[0][2] = M[0][2] + m.M[0][2];  M[1][2] = M[1][2] + m.M[1][2];
	M[0][3] = M[0][3] + m.M[0][3];  M[1][3] = M[1][3] + m.M[1][3];

	M[2][0] = M[2][0] + m.M[2][0];  M[3][0] = M[3][0] + m.M[3][0];
	M[2][1] = M[2][1] + m.M[2][1];  M[3][1] = M[3][1] + m.M[3][1];
	M[2][2] = M[2][2] + m.M[2][2];  M[3][2] = M[3][2] + m.M[3][2];
	M[2][3] = M[2][3] + m.M[2][3];  M[3][3] = M[3][3] + m.M[3][3];
}

_Matrix _Matrix::operator*(float other) const
{
	_Matrix result;
	result.M[0][0] = M[0][0] * other;  result.M[1][0] = M[1][0] * other;
	result.M[0][1] = M[0][1] * other;  result.M[1][1] = M[1][1] * other;
	result.M[0][2] = M[0][2] * other;  result.M[1][2] = M[1][2] * other;
	result.M[0][3] = M[0][3] * other;  result.M[1][3] = M[1][3] * other;

	result.M[2][0] = M[2][0] * other;  result.M[3][0] = M[3][0] * other;
	result.M[2][1] = M[2][1] * other;  result.M[3][1] = M[3][1] * other;
	result.M[2][2] = M[2][2] * other;  result.M[3][2] = M[3][2] * other;
	result.M[2][3] = M[2][3] * other;  result.M[3][3] = M[3][3] * other;
	return result;
}

void _Matrix::operator*=(float other)
{
	M[0][0] = M[0][0] * other;  M[1][0] = M[1][0] * other;
	M[0][1] = M[0][1] * other;  M[1][1] = M[1][1] * other;
	M[0][2] = M[0][2] * other;  M[1][2] = M[1][2] * other;
	M[0][3] = M[0][3] * other;  M[1][3] = M[1][3] * other;

	M[2][0] = M[2][0] * other;  M[3][0] = M[3][0] * other;
	M[2][1] = M[2][1] * other;  M[3][1] = M[3][1] * other;
	M[2][2] = M[2][2] * other;  M[3][2] = M[3][2] * other;
	M[2][3] = M[2][3] * other;  M[3][3] = M[3][3] * other;
}

bool _Matrix::operator==(const _Matrix& m) const
{
	for (int32 x = 0; x < 4; ++x)
	{
		for (int32 y = 0; y < 4; ++y)
		{
			if (M[x][y] != m.M[x][y])
			{
				return false;
			}
		}
	}
	return true;
}

bool _Matrix::IsEquals(const _Matrix& m, float epsilon) const
{
	for (int32 x = 0; x < 4; ++x)
	{
		for (int32 y = 0; y < 4; ++y)
		{
			if (GMath::Abs(M[x][y] - m.M[x][y]) > epsilon)
			{
				return false;
			}
		}
	}
	return true;
}

bool _Matrix::operator!=(const _Matrix& m) const
{
	for (int32 x = 0; x < 4; ++x)
	{
		for (int32 y = 0; y < 4; ++y)
		{
			if (M[x][y] != m.M[x][y])
			{
				return true;
			}
		}
	}
	return false;
}

PerspectiveMatrix::PerspectiveMatrix(float FOV, float aspectRatio, float nearZ, float farZ)
{
	float xf = (nearZ) / (nearZ - farZ);
	float s, c;
	GMath::SinCos(&s, &c, FOV*0.5f);
	float x = c/s;
	M[0][0] = 0;				M[0][1] = 0;				M[0][2] = xf;				M[0][3] = 1;
	M[1][0] = x/aspectRatio;	M[1][1] = 0;				M[1][2] = 0 ;				M[1][3] = 0;
	M[2][0] = 0;				M[2][1] = x;				M[2][2] = 0 ;				M[2][3] = 0;
	M[3][0] = 0;				M[3][1] = 0;				M[3][2] = -farZ*xf;			M[3][3] = 0;
}
