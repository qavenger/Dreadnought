#include <stdafx.h>
#include "Transform.h"

Transform::Transform()
	:Position(),Rotation(),Scale(1)
{
}

Transform::Transform(const Transform& other)
	:
	Position(other.Position),
	Rotation(other.Rotation),
	Scale(other.Scale)
{
}

Transform& Transform::operator=(const Transform& other)
{
	Position = other.Position;
	Rotation = other.Rotation;
	Scale = other.Scale;
	return *this;
}

Transform& Transform::operator=(Transform&& other)noexcept
{
	Position = std::move(other.Position);
	Rotation = std::move(other.Rotation);
	Scale = std::move(other.Scale);
	return *this;
}
