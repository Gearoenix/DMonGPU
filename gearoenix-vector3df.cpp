#include "gearoenix-vector3df.hpp"
#include "gearoenix-log.hpp"
#include "gearoenix-shortcuts.hpp"
#include <cmath>

#define PRINTER_PRECISION

gearoenix::math::Vector3Df::Vector3Df()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

gearoenix::math::Vector3Df::Vector3Df(const float &f)
{
	x = f;
	y = f;
	z = f;
}

gearoenix::math::Vector3Df::Vector3Df(const float &x, const float &y, const float &z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void gearoenix::math::Vector3Df::operator+=(const Vector3Df &f)
{
	x += f.x;
	y += f.y;
	z += f.z;
}

void gearoenix::math::Vector3Df::operator-=(const Vector3Df &f)
{
	x -= f.x;
	y -= f.y;
	z -= f.z;
}

void gearoenix::math::Vector3Df::operator*=(const Vector3Df &f)
{
	x *= f.x;
	y *= f.y;
	z *= f.z;
}

void gearoenix::math::Vector3Df::operator/=(const Vector3Df &f)
{
	x /= f.x;
	y /= f.y;
	z /= f.z;
}

void gearoenix::math::Vector3Df::operator+=(const float &f)
{
	x += f;
	y += f;
	z += f;
}

void gearoenix::math::Vector3Df::operator-=(const float &f)
{
	x -= f;
	y -= f;
	z -= f;
}

void gearoenix::math::Vector3Df::operator*=(const float &f)
{
	x *= f;
	y *= f;
	z *= f;
}

void gearoenix::math::Vector3Df::operator/=(const float &f)
{
	x /= f;
	y /= f;
	z /= f;
}

bool gearoenix::math::Vector3Df::operator==(const Vector3Df &f) const
{
	return (x == f.x) && (y == f.y) && (z == f.z);
}

void gearoenix::math::Vector3Df::normalize()
{
	*this /= length();
}

float gearoenix::math::Vector3Df::length() const
{
	return float(std::sqrt(x * x + y * y + z * z));
}

float gearoenix::math::Vector3Df::length2() const
{
	return x * x + y * y + z * z;
}

float gearoenix::math::Vector3Df::dot(const Vector3Df &f) const
{
	return f.x * x + f.y * y + f.z * z;
}

gearoenix::math::Vector3Df gearoenix::math::Vector3Df::operator+(const Vector3Df &f) const
{
	Vector3Df r;
	r.x = x + f.x;
	r.y = y + f.y;
	r.z = z + f.z;
	return r;
}

gearoenix::math::Vector3Df gearoenix::math::Vector3Df::operator-(const Vector3Df &f) const
{
	Vector3Df r;
	r.x = x - f.x;
	r.y = y - f.y;
	r.z = z - f.z;
	return r;
}

gearoenix::math::Vector3Df gearoenix::math::Vector3Df::operator*(const Vector3Df &f) const
{
	Vector3Df r;
	r.x = x * f.x;
	r.y = y * f.y;
	r.z = z * f.z;
	return r;
}

gearoenix::math::Vector3Df gearoenix::math::Vector3Df::operator/(const Vector3Df &f) const
{
	Vector3Df r;
	r.x = x / f.x;
	r.y = y / f.y;
	r.z = z / f.z;
	return r;
}

gearoenix::math::Vector3Df gearoenix::math::Vector3Df::cross(const Vector3Df &f) const
{
	Vector3Df r;
	r.x = y * f.z - z * f.y;
	r.y = z * f.x - x * f.z;
	r.z = x * f.y - y * f.x;
	return r;

}

gearoenix::math::Vector3Df gearoenix::math::Vector3Df::operator-() const
{
	return Vector3Df(-x, -y, -z);
}

const float &gearoenix::math::Vector3Df::operator[](const unsigned int &index) const
{
	switch (index)
	{
	case 0:
		return x;
		break;
	case 1:
		return y;
		break;
	case 2:
		return z;
		break;
	default:
		gce("Index out of range in Vector3D index operator.");
		break;
	}
}

float &gearoenix::math::Vector3Df::operator[](const unsigned int &index)
{
	switch (index)
	{
	case 0:
		return x;
		break;
	case 1:
		return y;
		break;
	case 2:
		return z;
		break;
	default:
		gce("Index out of range in Vector3D index operator.");
		break;
	}
}

gearoenix::math::Vector3Df gearoenix::math::Vector3Df::operator+(const float &f) const
{
	return Vector3Df(x + f, y + f, z + f);
}

gearoenix::math::Vector3Df gearoenix::math::Vector3Df::operator-(const float &f) const
{
	return Vector3Df(x - f, y - f, z - f);
}

gearoenix::math::Vector3Df gearoenix::math::Vector3Df::operator*(const float &f) const
{
	return Vector3Df(x * f, y * f, z * f);
}

gearoenix::math::Vector3Df gearoenix::math::Vector3Df::operator/(const float &f) const
{
	return Vector3Df(x / f, y / f, z / f);
}

std::ostream& gearoenix::math::operator<<(std::ostream& os, const gearoenix::math::Vector3Df& obj)
{
	os << "Vector 3D float(x: " << obj.x << ", y: " << obj.y << ", z: " << obj.z << ")";
	return os;
}
