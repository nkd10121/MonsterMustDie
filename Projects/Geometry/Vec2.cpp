#include "Vec2.h"
#include <cassert>

Vec2::Vec2() :
	x(0.0f),
	y(0.0f)
{
}

Vec2::Vec2(float inX, float inY) :
	x(inX),
	y(inY)
{
}

Vec2 Vec2::Right()
{
	return Vec2(1.0f,0.0f);
}

Vec2 Vec2::Left()
{
	return Vec2(-1.0f,0.0f);
}

Vec2 Vec2::Up()
{
	return Vec2(0.0f,1.0f);
}

Vec2 Vec2::Down()
{
	return Vec2(0.0f,-1.0f);
}

Vec2 Vec2::Zero()
{
	return Vec2(0.0f,0.0f);
}

Vec2 Vec2::operator-() const
{
	return Vec2(-x, -y);
}

Vec2 Vec2::operator+(const Vec2& val) const
{
	return Vec2(x + val.x, y + val.y);
}

void Vec2::operator+=(const Vec2& val)
{
	x += val.x;
	y += val.y;
}

Vec2 Vec2::operator-(const Vec2& val) const
{
	return Vec2(x - val.x, y - val.y);
}

void Vec2::operator-=(const Vec2& val)
{
	x -= val.x;
	y -= val.y;
}

Vec2 Vec2::operator*(float scale) const
{
	return Vec2(x * scale, y * scale);
}

void Vec2::operator*=(float scale)
{
	x *= scale;
	y *= scale;
}

Vec2 Vec2::operator/(float scale) const
{
	if (scale == 0.0f)
	{
		assert(0 && "0除算を検知");
		return Vec2();
	}

	return Vec2(x / scale, y / scale);
}

void Vec2::operator/=(float scale)
{
	if (SqLength() > 0.0f)
	{
		x /= scale;
		y /= scale;
	}
	else
	{
		assert(0 && "0除算を検知");
		x = 0.0f;
		y = 0.0f;
	}
}

bool Vec2::operator==(const Vec2& val) const
{
	return x == val.x && y == val.y;
}

bool Vec2::operator!=(const Vec2& val) const
{
	return x != val.x || y != val.y;
}

float Vec2::Length() const
{
	float ret;
	ret = sqrt(x * x + y * y);
	return ret;
}

float Vec2::SqLength() const
{
	float ret;
	ret = x * x + y * y;
	return ret;
}

Vec2 Vec2::Normalize() const
{
	Vec2 ret;
	float size = this->Length();
	if (size == 0.0f)
	{
		//assert(0 && "0除算を検知");
		return ret;
	}
	ret.x = this->x / size;
	ret.y = this->y / size;
	return ret;
}
