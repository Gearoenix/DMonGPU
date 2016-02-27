#ifndef GEAROENIX_CORE_CAMERA_HPP
#define GEAROENIX_CORE_CAMERA_HPP
#include "gearoenix-matrix4x4f.hpp"
#include "gearoenix-vector3df.hpp"
namespace gearoenix
{
	namespace math
	{
		class Camera
		{
		protected:
			Vector3Df localX;
			Vector3Df localY;
			Vector3Df localZ;
			Vector3Df location;
			Matrix4x4f rsvM;
			Matrix4x4f viewM;
			Matrix4x4f projectionM;
			Matrix4x4f vpM;
			float ratio;
			float cameraNear;
			float cameraFar;
			Camera(const float &n, const float &f, const float &ratio);
			virtual void ratioChanged() = 0;
		public:
			const Vector3Df &getLocalX() const;
			const Matrix4x4f &getVPM() const;
			void rotateGlobalyYAround(const float &degree, const Vector3Df &point);
			void rotateGlobalyZAround(const float &degree, const Vector3Df &point);
			void rotateLocalyXAround(const float &degree, const Vector3Df &point);
			void moveForward(const float &dis);
			void move(const Vector3Df &v);
			void setViewport(const float &w, const float &h);
			const Vector3Df &getLocation() const;
			const float &getRatio() const;
			float &getRatio();
			static Camera *current;
		};
	}
}
#endif
