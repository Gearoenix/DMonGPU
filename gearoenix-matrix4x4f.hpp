#ifndef GEAROENIX_CORE_MATRIX4X4
#define GEAROENIX_CORE_MATRIX4X4
#include "gearoenix-vector3df.hpp"
namespace gearoenix
{
	namespace math
	{
		struct Matrix4x4f
		{
			float m[4][4];
			Matrix4x4f();
			Matrix4x4f(const Matrix4x4f &o);
			Matrix4x4f operator*(const Matrix4x4f &a) const;
			Vector3Df operator*(const Vector3Df &v) const;
			void operator=(const Matrix4x4f &a);
			void makeIdentity();
			void makeTranslate(const Vector3Df &v);
			void makeRotation(const Vector3Df &v, const float &degree);
			void makePerspective(const float &ratio, const float &fieldOfView, const float &near, const float &far);
			void makeOrtho(const float &ratio, const float &n, const float &f, const float &size = 1.0f);
			void place(const Vector3Df &v);
			void scaleX(const float &x);
			void scaleY(const float &y);
			void scaleZ(const float &z);
			void getLocation(Vector3Df &loc) const;
		};
	}
}
#endif
