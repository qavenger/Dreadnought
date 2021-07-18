#pragma once
#include <MathUtil.h>
class Transform
{
public:
	Transform();
	Transform(const Transform& other);
	Transform& operator=(const Transform& other);
	Transform& operator=(Transform&& other)noexcept;
private:
	_Vector Position;
	_Rotator Rotation;
	_Vector Scale;
};