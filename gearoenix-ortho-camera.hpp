#ifndef GEAROENIX_CORE_ORTHO_CAMERA_HPP
#define GEAROENIX_CORE_ORTHO_CAMERA_HPP
#include "gearoenix-camera.hpp"
namespace gearoenix
{
	namespace math
	{
		class OrthoCamera: public Camera
		{
		private:
			float orthSize = 1.0f;
			void ratioChanged();
		public:
			OrthoCamera(const float &n, const float &f, const float &ratio);
			void scaleOrthoSize(const float &s);
		};
	}
}
#endif
