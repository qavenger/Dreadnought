#pragma once
__declspec(align(16)) struct _Quaternion
{
	float x, y, z, w;

	std::string ToString()const;

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

	FORCEINLINE _Quaternion() {}
	FORCEINLINE _Quaternion(float inX, float inY, float inZ, float inW);
	FORCEINLINE _Quaternion(const VectorRegister& v);
	explicit _Quaternion(const _Matrix& M);
	explicit _Quaternion(const _Rotator& R);
	_Quaternion(const _Vector& axis, float angle);

	FORCEINLINE _Quaternion operator+(const _Quaternion& q)const;
	FORCEINLINE _Quaternion operator+=(const _Quaternion& q);

	FORCEINLINE _Quaternion operator-(const _Quaternion& q)const;
	FORCEINLINE _Quaternion operator-=(const _Quaternion& q);

	FORCEINLINE bool Equals(const _Quaternion& q, float epsilon = KINDA_SMALL_NUMBER)const;
	FORCEINLINE bool IsIdentity(float epsilon = KINDA_SMALL_NUMBER)const;

	FORCEINLINE _Quaternion operator*(const _Quaternion& q)const;
	FORCEINLINE _Quaternion operator*=(const _Quaternion& q);

	_Vector RotateVector(const _Vector& v)const;
	_Vector UnrotateVector(const _Vector& v)const;
	void ToSwingTwist(const _Vector& inTwistAxis, _Quaternion& outSwing, _Quaternion& outTwist)const;
	float GetTwistAngle(const _Vector& twistAxis)const;
	FORCEINLINE bool IsNormalized()const;
	FORCEINLINE _Quaternion Inverse()const;
	FORCEINLINE void Normalize(float epsilon = SMALL_NUMBER);
	FORCEINLINE _Quaternion GetNormalized(float epsilon = SMALL_NUMBER)const;
	_Vector operator*(const _Vector& v)const;
	_Matrix operator*(const _Matrix& m)const;

	FORCEINLINE _Quaternion operator*=(const float scale);
	FORCEINLINE _Quaternion operator*(const float scale)const;

	FORCEINLINE _Quaternion operator/=(const float scale);
	FORCEINLINE _Quaternion operator/(const float scale)const;

	float operator|(const _Quaternion& q)const;

	bool Identical(const _Quaternion& q)const;
	bool operator==(const _Quaternion& q)const;
	bool operator!=(const _Quaternion& q)const;
public:
	static _Quaternion FromEuler(const _Vector& euler);

	void SetShortestArcWith(const _Quaternion& q);

	_Quaternion Log()const;
	_Quaternion Exp()const;

	FORCEINLINE _Vector GetAxisX()const;
	FORCEINLINE _Vector GetAxisY()const;
	FORCEINLINE _Vector GetAxisZ()const;
	FORCEINLINE _Vector GetForwardVector()const;
	FORCEINLINE _Vector GetRightVector()const;
	FORCEINLINE _Vector GetUpVector()const;
	FORCEINLINE _Vector ToVector()const;
	FORCEINLINE _Vector GetRotationAxis()const;
	FORCEINLINE float AngularDistance(const _Quaternion& q)const;
	FORCEINLINE float SizeSquared()const;
	FORCEINLINE float Size()const;
	FORCEINLINE float GetAngle()const;
	void ToAxisAngle(_Vector& axis, float& angle)const;
	_Rotator ToRotator()const;

public:
	FORCEINLINE static _Quaternion FindBetween(const _Vector& v1, const _Vector& v2)
	{
		return FindBetweenVectors(v1, v2);
	}

	static _Quaternion FindBetweenNormals(const _Vector& v1, const _Vector& v2);
	static _Quaternion FindBetweenVectors(const _Vector& v1, const _Vector& v2);

	FORCEINLINE static float Error(const _Quaternion& q1, const _Quaternion& q2);
	FORCEINLINE static float ErrorAutoNormalize(const _Quaternion& q1, const _Quaternion& q2);
	FORCEINLINE static _Quaternion FastLerp(const _Quaternion& A, const _Quaternion& B, const float Alpha);
	FORCEINLINE static _Quaternion FastBilerp(const _Quaternion& p00, const _Quaternion& p01, const _Quaternion& p10, const _Quaternion& p11, const float FracX, const float FracY);
	static _Quaternion Slerp_NoNormalization(const _Quaternion& q1, const _Quaternion& q2, float slerp);
	FORCEINLINE static _Quaternion Slerp(const _Quaternion& q1, const _Quaternion& q2, float slerp)
	{
		return _Quaternion::Slerp_NoNormalization(q1, q2, slerp).GetNormalized();
	}

	static _Quaternion SlerpFullPath_NoNormalization(const _Quaternion& q1, const _Quaternion& q2, float slerp);

	FORCEINLINE static _Quaternion SlerpFullPath(const _Quaternion& q1, const _Quaternion& q2, float slerp)
	{
		return _Quaternion::SlerpFullPath_NoNormalization(q1, q2, slerp).GetNormalized();
	}

	static _Quaternion Squad(const _Quaternion& q1, const _Quaternion& t1, const _Quaternion& q2, const _Quaternion& t2, float alpha);
	static _Quaternion SquadFullPath(const _Quaternion& q1, const _Quaternion& t1, const _Quaternion& q2, const _Quaternion& t2, float alpha);
	static void CalcTangents(const _Quaternion& prevP, const _Quaternion& p, const _Quaternion& nextP, float tension, _Quaternion& outTan);
};


FORCEINLINE _Quaternion::_Quaternion(float inX, float inY, float inZ, float inW)
	:x(inX),y(inY),z(inZ),w(inW)
{
	CheckNan();
}

FORCEINLINE float _Quaternion::SizeSquared() const
{
	return x * x + y * y + z * z + w + w;
}

FORCEINLINE float _Quaternion::Size() const
{
	return GMath::Sqrt(SizeSquared());
}

FORCEINLINE float _Quaternion::GetAngle() const
{
	return 2* GMath::Acos(w);
}

FORCEINLINE float _Quaternion::operator|(const _Quaternion& q) const
{
	return x * q.x + y * q.y + z * q.z + w * q.w;
}

FORCEINLINE bool _Quaternion::Identical(const _Quaternion& q) const
{
	return x == q.x && y == q.y && z == q.z && w == q.w;
}

