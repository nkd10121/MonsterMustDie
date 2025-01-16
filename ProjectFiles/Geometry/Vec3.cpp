#include "Vec3.h"
#include <cassert>

Vec3::Vec3() :
	x(0.0f),
	y(0.0f),
	z(0.0f)
{
}

Vec3::Vec3(float inX, float inY, float inZ) :
	x(inX),
	y(inY),
	z(inZ)
{
}

Vec3::Vec3(DxLib::VECTOR vec) :
	x(vec.x),
	y(vec.y),
	z(vec.z)
{
}

Vec3 Vec3::Right()
{
	return Vec3(1.0f, 0.0f, 0.0f);
}

Vec3 Vec3::Left()
{
	return Vec3(-1.0f, 0.0f, 0.0f);
}

Vec3 Vec3::Up()
{
	return Vec3(0.0f, 1.0f, 0.0f);
}

Vec3 Vec3::Down()
{
	return Vec3(0.0f, -1.0f, 0.0f);
}

Vec3 Vec3::Front()
{
	return Vec3(0.0f, 0.0f, 1.0f);
}

Vec3 Vec3::Back()
{
	return Vec3(0.0f, 0.0f, -1.0f);
}

Vec3 Vec3::operator-() const
{
	return Vec3(-x, -y, -z);
}

Vec3 Vec3::operator+(const Vec3& val) const
{
	return Vec3(x + val.x, y + val.y, z + val.z);
}

void Vec3::operator+=(const Vec3& val)
{
	x += val.x;
	y += val.y;
	z += val.z;
}

Vec3 Vec3::operator-(const Vec3& val) const
{
	return Vec3(x - val.x, y - val.y, z - val.z);
}

void Vec3::operator-=(const Vec3& val)
{
	x -= val.x;
	y -= val.y;
	z -= val.z;
}

Vec3 Vec3::operator*(float scale) const
{
	return Vec3(x * scale, y * scale, z * scale);
}

void Vec3::operator*=(float scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
}

Vec3 Vec3::operator/(float scale) const
{
	if (scale == 0.0f)
	{
		assert(0 && "0除算を検知");
		return Vec3();
	}

	return Vec3(x / scale, y / scale, z / scale);
}

void Vec3::operator/=(float scale)
{
	if (SqLength() > 0.0f)
	{
		x /= scale;
		y /= scale;
		z /= scale;
	}
	else
	{
		assert(0 && "0除算を検知");
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
}

bool Vec3::operator==(const Vec3& val) const
{
	return x == val.x && y == val.y && z == val.z;
}

bool Vec3::operator!=(const Vec3& val) const
{
	return x != val.x || y != val.y || z != val.z;
}

float Vec3::Length() const
{
	float ret;
	ret = sqrt(x * x + y * y + z * z);
	return ret;
}

float Vec3::SqLength() const
{
	float ret;
	ret = x * x + y * y + z * z;
	return ret;
}

Vec3 Vec3::Normalize() const
{
	Vec3 ret;
	float size = this->Length();
	if (size == 0.0f)
	{
		//assert(0 && "0除算を検知");
		return ret;
	}
	ret.x = this->x / size;
	ret.y = this->y / size;
	ret.z = this->z / size;
	return ret;
}

VECTOR Vec3::ToVECTOR() const
{
	DxLib::VECTOR ret = VGet(0.0f, 0.0f, 0.0f);
	ret.x = this->x;
	ret.y = this->y;
	ret.z = this->z;
	return ret;
}

float Dot(const Vec3& item1, const Vec3& item2)
{
	float ret;
	ret = item1.x * item2.x + item1.y * item2.y + item1.z * item2.z;
	return ret;
}

Vec3 Cross(const Vec3& item1, const Vec3& item2)
{
	Vec3 ret;
	ret.x = item1.y * item2.z - item1.z * item2.y;
	ret.y = item1.z * item2.x - item1.x * item2.z;
	ret.z = item1.x * item2.y - item1.y * item2.x;
	return ret;
}

Vec3 Abs(const Vec3& item)
{
	Vec3 ret = item;

	if (ret.x < 0.0f)
	{
		ret.x = -ret.x;
	}

	if (ret.y < 0.0f)
	{
		ret.y = -ret.y;
	}

	if (ret.z < 0.0f)
	{
		ret.z = -ret.z;
	}

	return ret;
}
