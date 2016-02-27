#include "gearoenix-matrix4x4f.hpp"
#include <cmath>

gearoenix::math::Matrix4x4f::Matrix4x4f()
{
}

gearoenix::math::Matrix4x4f::Matrix4x4f(const gearoenix::math::Matrix4x4f &o)
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			m[i][j] = o.m[i][j];
}

gearoenix::math::Matrix4x4f gearoenix::math::Matrix4x4f::operator*(const Matrix4x4f &a) const
{
	gearoenix::math::Matrix4x4f r;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			r.m[i][j] = 0.0f;
			for (int k = 0; k < 4; ++k)
			{
				r.m[i][j] += a.m[i][k] * m[k][j];
			}
		}
	}
	return r;
}

gearoenix::math::Vector3Df gearoenix::math::Matrix4x4f::operator*(const Vector3Df &v) const
{
	gearoenix::math::Vector3Df r;
	for (int i = 0; i < 3; ++i)
	{
		r[i] = m[3][i];
		for (int j = 0; j < 3; ++j)
		{
			r[i] += m[j][i] * v[j];
		}
	}
	return r;
}

void gearoenix::math::Matrix4x4f::operator=(const Matrix4x4f & a)
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			m[i][j] = a.m[i][j];
}

void gearoenix::math::Matrix4x4f::makeIdentity()
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			if (i == j)
			{
				m[i][j] = 1.0f;
			}
			else
			{
				m[i][j] = 0.0f;
			}
}

void gearoenix::math::Matrix4x4f::makeTranslate(const Vector3Df &v)
{
	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0f;
	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
	m[2][3] = 0.0f;
	m[3][0] = v.x;
	m[3][1] = v.y;
	m[3][2] = v.z;
	m[3][3] = 1.0f;
}

void gearoenix::math::Matrix4x4f::makeRotation(const Vector3Df &v, const float &degree)
{
	auto sinus = std::sin(degree);
	auto cosinus = std::cos(degree);
	auto oneminuscos = 1.0 - cosinus;
	Vector3Df w = v;
	w.normalize();
	auto wx2 = w.x * w.x;
	auto wxy = w.x * w.y;
	auto wxz = w.x * w.z;
	auto wy2 = w.y * w.y;
	auto wyz = w.y * w.z;
	auto wz2 = w.z * w.z;
	auto wxyonemincos = wxy * oneminuscos;
	auto wxzonemincos = wxz * oneminuscos;
	auto wyzonemincos = wyz * oneminuscos;
	auto wxsin = w.x * sinus;
	auto wysin = w.y * sinus;
	auto wzsin = w.z * sinus;
	m[0][0] = float(cosinus + (wx2 * oneminuscos));
	m[0][1] = float(wzsin + wxyonemincos);
	m[0][2] = float(wxzonemincos - wysin);
	m[0][3] = 0.0f;
	m[1][0] = float(wxyonemincos - wzsin);
	m[1][1] = float(cosinus + (wy2 * oneminuscos));
	m[1][2] = float(wxsin + wyzonemincos);
	m[1][3] = 0.0f;
	m[2][0] = float(wysin + wxzonemincos);
	m[2][1] = float(wyzonemincos - wxsin);
	m[2][2] = float(cosinus + (wz2 * oneminuscos));
	m[2][3] = 0.0f;
	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;
}

void gearoenix::math::Matrix4x4f::makePerspective(const float &ratio, const float &fieldOfView, const float &near, const float &far)
{
	float width = float(near / std::tan(fieldOfView / 2.0));
	float height = width / ratio;
	m[0][0] = float((2.0 * near) / width);
	m[0][1] = float(0.0);
	m[0][2] = float(0.0);
	m[0][3] = float(0.0);
	m[1][0] = float(0.0);
	m[1][1] = float((2.0 * near) / height);
	m[1][2] = float(0.0);
	m[1][3] = float(0.0);
	m[2][0] = float(0.0);
	m[2][1] = float(0.0);
	m[2][2] = float((far + near) / (near - far));
	m[2][3] = float(-1.0);
	m[3][0] = float(0.0);
	m[3][1] = float(0.0);
	m[3][2] = float((2.0 * far * near) / (near - far));
	m[3][3] = float(0.0);
}

void gearoenix::math::Matrix4x4f::makeOrtho(const float &ratio, const float &n, const float &f, const float &size)
{
	m[0][0] = 2.0f / (ratio * size);
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0f;
	m[1][0] = 0.0f;
	m[1][1] = 2.0f / (size);
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 2.0f / (n - f);
	m[2][3] = 0.0f;
	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = (f + n) / (n - f);
	m[3][3] = 1.0f;
}

void gearoenix::math::Matrix4x4f::place(const Vector3Df &v)
{
	m[3][0] = v.x;
	m[3][1] = v.y;
	m[3][2] = v.z;
}

void gearoenix::math::Matrix4x4f::scaleX(const float &x)
{
	m[0][0] *= x;
	m[0][1] *= x;
	m[0][2] *= x;
}

void gearoenix::math::Matrix4x4f::scaleY(const float &y)
{
	m[1][0] *= y;
	m[1][1] *= y;
	m[1][2] *= y;
}

void gearoenix::math::Matrix4x4f::scaleZ(const float &z)
{
	m[2][0] *= z;
	m[2][1] *= z;
	m[2][2] *= z;
}

void gearoenix::math::Matrix4x4f::getLocation(Vector3Df &loc) const
{
	loc.x = m[3][0];
	loc.y = m[3][1];
	loc.z = m[3][2];
}