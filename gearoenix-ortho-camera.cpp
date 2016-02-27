#include "gearoenix-ortho-camera.hpp"

void gearoenix::math::OrthoCamera::ratioChanged()
{
	projectionM.makeOrtho(ratio, cameraNear, cameraFar, orthSize);
	vpM = projectionM * viewM;
}

gearoenix::math::OrthoCamera::OrthoCamera(const float & n, const float & f, const float & ratio): Camera(n, f, ratio)
{
	ratioChanged();
}

void gearoenix::math::OrthoCamera::scaleOrthoSize(const float &s)
{
	orthSize *= s;
	ratioChanged();
}
