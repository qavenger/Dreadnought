#pragma once
#include <EngineMath.h>
struct _Rotator
{
	float Roll, Pitch, Yaw;

	static const _Rotator ZeroRotator;
	std::string ToString()const;
	bool HasNan()const;
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

	float operator[](uint32 index)const;
	float& operator[](uint32 index);
	_Rotator operator+(const _Rotator& r)const;
	_Rotator operator-(const _Rotator& r)const;
	_Rotator operator*(float scale)const;
	_Rotator& operator+=(const _Rotator& r);
	_Rotator& operator-=(const _Rotator& r);
	_Rotator& operator*=(float scale);
	_Rotator& Add(float deltaRoll, float deltaPitch, float deltaYaw);
	_Rotator GetClamped()const;
	_Rotator& Clamp();
	_Vector ToVector()const;
	_Vector ToEuler()const;
	_Quaternion ToQuaternion()const;

	_Vector RotatorVector(const _Vector& v)const;
	_Vector UnrotatorVector(const _Vector& v)const;
	bool operator==(const _Rotator& r)const;
	bool operator!=(const _Rotator& r)const;

	bool IsZero()const;
	bool IsNearlyZero(float epsilon)const;
	bool Equals(const _Rotator& r, float epsilon = KINDA_SMALL_NUMBER)const;
public:
	/// <returns>Sum of the difference of three axe</returns>
	float GetManhanttanDistance(const _Rotator& rot)const;
	_Rotator GetEquivalentRotator()const;
	void MakeClosest(_Rotator& other)const;
	void Normalize();
	_Rotator GetNormalized()const;
	_Rotator GetDenormalized()const;
	void GetWindingAndRemainder(_Rotator& winding, _Rotator& remainder)const;
public:
	/// <summary>
	/// X = Roll, Y = Pitch, Z = Yaw
	/// </summary>
	static _Rotator FromEuler(const _Vector& euler);

	/// <returns>clamp the angle to 0 to 360</returns>
	static float ClampAngle(float angle);

	/// <returns>clamp the angle to -180 to 180</returns>
	static float NormalizeAngle(float angle);
public:

	/// <returns>map angle from [0, 360] degree of float to [0, 255] of uint8</returns>
	static uint8 CompressAngleToByte(float angle);

	/// <returns>map angle from [0, 255] uint8 to [0, 360] degree of float</returns>
	static float DecompressAngleFromByte(uint8 angle);

	/// <returns>map angle from [0, 360] degree of float to [0, 255] of uint8</returns>
	static uint8 CompressAngleToShort(float angle);

	/// <returns>map angle from [0, 255] uint8 to [0, 360] degree of float</returns>
	static float DecompressAngleFromShort(uint16 angle);
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

FORCEINLINE float _Rotator::operator[](uint32 index) const
{
	assert(index >= 0 && index < 3);
	return ((const float*)(this))[index];
}

FORCEINLINE float& _Rotator::operator[](uint32 index)
{
	assert(index >= 0 && index < 3);
	return ((float*)(this))[index];
}

FORCEINLINE _Rotator _Rotator::operator+(const _Rotator& r) const
{
	return _Rotator(Roll+r.Roll, Pitch+r.Pitch, Yaw+r.Yaw);
}

FORCEINLINE _Rotator _Rotator::operator-(const _Rotator& r) const
{
	return _Rotator(Roll - r.Roll, Pitch - r.Pitch, Yaw - r.Yaw);
}

FORCEINLINE _Rotator _Rotator::operator*(float scale) const
{
	return _Rotator(Roll * scale, Pitch * scale, Yaw * scale);
}

FORCEINLINE _Rotator& _Rotator::operator+=(const _Rotator& r)
{
	Roll	+= r.Roll;
	Pitch	+= r.Pitch;
	Yaw		+= r.Yaw;
	CheckNan();
	return *this;
}

FORCEINLINE _Rotator& _Rotator::operator-=(const _Rotator& r)
{
	Roll	-= r.Roll;
	Pitch	-= r.Pitch;
	Yaw		-= r.Yaw;
	CheckNan();
	return *this;
}

FORCEINLINE _Rotator& _Rotator::operator*=(float scale)
{
	Roll	*= scale;
	Pitch	*= scale;
	Yaw		*= scale;
	CheckNan();
	return *this;
}

FORCEINLINE _Rotator& _Rotator::Add(float deltaRoll, float deltaPitch, float deltaYaw)
{
	Roll += deltaRoll; Pitch += deltaPitch; Yaw += deltaYaw;
	CheckNan();
	return *this;
}

FORCEINLINE _Rotator _Rotator::GetClamped() const
{
	return _Rotator(ClampAngle(Roll), ClampAngle(Pitch), ClampAngle(Yaw));
}

FORCEINLINE _Rotator& _Rotator::Clamp()
{
	Roll = ClampAngle(Roll);
	Pitch = ClampAngle(Pitch);
	Yaw = ClampAngle(Yaw);
	return *this;
}

FORCEINLINE bool _Rotator::operator==(const _Rotator& r) const
{
	return Roll == r.Roll && Pitch == r.Pitch && Yaw == r.Yaw;
}

FORCEINLINE bool _Rotator::operator!=(const _Rotator& r) const
{
	return !(Roll == r.Roll && Pitch == r.Pitch && Yaw == r.Yaw);
}

FORCEINLINE bool _Rotator::IsZero() const
{
	return ClampAngle(Roll) == 0.0f && ClampAngle(Pitch) == 0.0f && ClampAngle(Yaw) == 0.0f;
}

FORCEINLINE float _Rotator::ClampAngle(float angle)
{
	const float div = angle / 360.0f;
	const float quotient = GMath::Abs(div) < FLOAT_NON_FRACTIONAL ? GMath::Trunc(div) : div;
	float intPortion = 360 * quotient;
	if (GMath::Abs(intPortion) > GMath::Abs(angle)) intPortion = angle;
	const float rs = angle - intPortion;
	return GMath::Min(rs, 360.0f);
}

FORCEINLINE float _Rotator::NormalizeAngle(float angle)
{
	angle = ClampAngle(angle);
	if (angle > 180.0f) angle -= 360;
	return angle;
}

FORCEINLINE uint8 _Rotator::CompressAngleToByte(float angle)
{
	return GMath::RoundToInt(angle * (256.0f / 360.0f)) & 0xff;
}

FORCEINLINE float _Rotator::DecompressAngleFromByte(uint8 angle)
{
	return angle * (360.0f / 256.0f);
}

inline uint8 _Rotator::CompressAngleToShort(float angle)
{
	return GMath::RoundToInt(angle * (65536.0f / 360.0f)) & 0xffff;
}

inline float _Rotator::DecompressAngleFromShort(uint16 angle)
{
	return angle * (360.0f / 65536.0f);
}
 

FORCEINLINE bool _Rotator::IsNearlyZero(float epsilon) const
{
	using namespace SSE;
	const VectorRegister regA = SSE::VectorLoadFloat3_W0(this);
	const VectorRegister norm = VectorNormalizeRotator(regA);
	const VectorRegister absNorm = VectorAbs(norm);

	return !VectorAnyGreaterThan(absNorm, VectorLoadFloat1(&epsilon));
}

inline bool _Rotator::Equals(const _Rotator& r, float epsilon) const
{
	using namespace SSE;
	const VectorRegister regA = SSE::VectorLoadFloat3_W0(this);
	const VectorRegister regB = SSE::VectorLoadFloat3_W0(&r);
	const VectorRegister normDet = VectorNormalizeRotator(VectorSubtract(regA, regB));
	const VectorRegister absNormDet = VectorAbs(normDet);

	return !VectorAnyGreaterThan(absNormDet, VectorLoadFloat1(&epsilon));
}



FORCEINLINE _Rotator _Rotator::GetNormalized() const
{
	_Rotator rot(*this);
	rot.Normalize();
	return rot;
}

FORCEINLINE _Rotator _Rotator::GetDenormalized() const
{
	return _Rotator(ClampAngle(Roll), ClampAngle(Pitch), ClampAngle(Yaw));
}

FORCEINLINE void _Rotator::GetWindingAndRemainder(_Rotator& winding, _Rotator& remainder) const
{
	// roll
	remainder.Roll = NormalizeAngle(Roll);
	winding.Roll = Roll - remainder.Roll;
	// pitch
	remainder.Pitch = NormalizeAngle(Pitch);
	winding.Pitch = Pitch - remainder.Pitch;
	// yaw
	remainder.Yaw = NormalizeAngle(Yaw);
	winding.Yaw = Yaw - remainder.Yaw;
}


FORCEINLINE float _Rotator::GetManhanttanDistance(const _Rotator& rot) const
{
	return GMath::Abs(Roll - rot.Roll) + GMath::Abs(Pitch - rot.Pitch) + GMath::Abs(Yaw - rot.Yaw);
}

FORCEINLINE _Rotator _Rotator::GetEquivalentRotator() const
{
	return _Rotator(Roll + 180.0f, 180.0f - Pitch, Yaw + 180.0f);
}

FORCEINLINE void _Rotator::MakeClosest(_Rotator& other) const
{
	_Rotator c = other.GetEquivalentRotator();
	float f = GetManhanttanDistance(other);
	float s = GetManhanttanDistance(c);
	if (s < f) other = c;
}

