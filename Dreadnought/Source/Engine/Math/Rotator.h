#pragma once
#include <EngineMath.h>
struct _Rotator
{
	float Roll, Pitch, Yaw;

	static const _Rotator ZeroRotator;
	std::string ToString()const;

	FORCEINLINE void CheckNan() const
	{
#if CHECK_NAN
		if (HasNan())
		{
			*this = ZeroRotator;
		}
#endif
	}

	FORCEINLINE _Rotator() {}
	FORCEINLINE _Rotator(float rpy);
	FORCEINLINE _Rotator(float roll, float pitch, float yaw);
	FORCEINLINE _Rotator(const _Quaternion& quat);

	/*FORCEINLINE _Rotator operator+(const _Rotator& r)const;
	FORCEINLINE _Rotator operator-(const _Rotator& r)const;
	FORCEINLINE _Rotator operator*(const _Rotator& r)const;*/
};

FORCEINLINE _Rotator::_Rotator(float rpy)
	:Roll(rpy), Pitch(rpy), Yaw(rpy)
{
	CheckNan();
}

FORCEINLINE _Rotator::_Rotator(float roll, float pitch, float yaw)
	:Roll(roll), Pitch(pitch), Yaw(yaw)
{
	CheckNan();
}