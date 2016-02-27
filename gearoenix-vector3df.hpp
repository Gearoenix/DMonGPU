#ifndef GEAROENIX_CORE_VECTOR3D
#define GEAROENIX_CORE_VECTOR3D
#include <ostream>
namespace gearoenix
{
	namespace math
	{
		struct Vector3Df
		{
			float x, y, z;
			Vector3Df();
			Vector3Df(const float &v);
			Vector3Df(const float &x, const float &y, const float &z);
			void operator+=(const Vector3Df &v);
			void operator-=(const Vector3Df &v);
			void operator*=(const Vector3Df &v);
			void operator/=(const Vector3Df &v);
			void operator+=(const float &v);
			void operator-=(const float &v);
			void operator*=(const float &v);
			void operator/=(const float &v);
			bool operator==(const Vector3Df &f) const;
			Vector3Df operator+(const Vector3Df &v) const;
			Vector3Df operator-(const Vector3Df &v) const;
			Vector3Df operator*(const Vector3Df &v) const;
			Vector3Df operator/(const Vector3Df &v) const;
			Vector3Df operator+(const float &v) const;
			Vector3Df operator-(const float &v) const;
			Vector3Df operator*(const float &v) const;
			Vector3Df operator/(const float &v) const;
			Vector3Df operator-() const;
			const float &operator[](const unsigned int &index) const;
			float &operator[](const unsigned int &index);
			void normalize();
			float length() const;
			float length2() const;
			float dot(const Vector3Df &v) const;
			Vector3Df cross(const Vector3Df &v) const;
		};
		std::ostream& operator<<(std::ostream& os, const Vector3Df& obj);
	}
}
#endif
