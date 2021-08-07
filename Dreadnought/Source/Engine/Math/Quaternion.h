#pragma once
struct _Quaternion
{
	float x, y, z, w;

	static const _Quaternion Identity;
	FORCEINLINE bool HasNan()const;

	FORCEINLINE void CheckNan() const
	{
#if CHECK_NAN
		if (HasNan())
		{
			*this = Identity;
		}
#endif
	}
};