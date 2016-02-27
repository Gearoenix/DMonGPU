#include "gearoenix-camera.hpp"

static const gearoenix::math::Vector3Df globalX(1.0f, 0.0f, 0.0f);
static const gearoenix::math::Vector3Df globalY(0.0f, 1.0f, 0.0f);
static const gearoenix::math::Vector3Df globalZ(0.0f, 0.0f, 1.0f);

gearoenix::math::Camera::Camera(const float &n, const float &f, const float &ratio): ratio(ratio)
{
	cameraNear = n;
	cameraFar = f;
	localX = Vector3Df(1.0f, 0.0f, 0.0f);
	localY = Vector3Df(0.0f, 1.0f, 0.0f);
	localZ = Vector3Df(0.0f, 0.0f, 1.0f);
	location = Vector3Df(0.0f, 0.0f, 10.0f);
	rsvM.makeIdentity();
	Matrix4x4f trans;
	trans.makeTranslate(-location);
	viewM = rsvM * trans;
}

const gearoenix::math::Matrix4x4f &gearoenix::math::Camera::getVPM() const
{
	return vpM;
}

const gearoenix::math::Vector3Df &gearoenix::math::Camera::getLocalX() const
{
	return localX;
}

void gearoenix::math::Camera::rotateGlobalyYAround(const float &degree, const Vector3Df &point)
{
	Vector3Df dis = location - point;
	Matrix4x4f trans;
	Matrix4x4f invtrans;
	trans.makeRotation(globalY, degree);
	invtrans.makeRotation(globalY, -degree);
	localX = invtrans * localX;
	localY = invtrans * localY;
	localZ = invtrans * localZ;
	dis = invtrans * dis;
	location = dis + point;
	rsvM = rsvM * trans;
	Matrix4x4f transM;
	transM.makeTranslate(-location);
	viewM = rsvM * transM;
	vpM = projectionM * viewM;
}

void gearoenix::math::Camera::rotateGlobalyZAround(const float &degree, const Vector3Df &point)
{
	Vector3Df dis = location - point;
	Matrix4x4f trans;
	Matrix4x4f invtrans;
	trans.makeRotation(globalZ, degree);
	invtrans.makeRotation(globalZ, -degree);
	localX = invtrans * localX;
	localY = invtrans * localY;
	localZ = invtrans * localZ;
	dis = invtrans * dis;
	location = dis + point;
	rsvM = rsvM * trans;
	Matrix4x4f transM;
	transM.makeTranslate(-location);
	viewM = rsvM * transM;
	vpM = projectionM * viewM;
}

void gearoenix::math::Camera::rotateLocalyXAround(const float &degree, const Vector3Df &point)
{
	Vector3Df dis = location - point;
	Matrix4x4f trans;
	Matrix4x4f invtrans;
	trans.makeRotation(localX, degree);
	invtrans.makeRotation(localX, -degree);
	localY = invtrans * localY;
	localZ = invtrans * localZ;
	dis = invtrans * dis;
	location = dis + point;
	rsvM = rsvM * trans;
	Matrix4x4f transM;
	transM.makeTranslate(-location);
	viewM = rsvM * transM;
	vpM = projectionM * viewM;
}

void gearoenix::math::Camera::moveForward(const float &dis)
{
	location += localZ * dis;
	Matrix4x4f transM;
	transM.makeTranslate(-location);
	viewM = rsvM * transM;
	vpM = projectionM * viewM;
}

void gearoenix::math::Camera::move(const Vector3Df &v)
{
	location += v;
	Matrix4x4f trans;
	trans.makeTranslate(-location);
	viewM = rsvM * trans;
	vpM = projectionM * viewM;
}

void gearoenix::math::Camera::setViewport(const float &w, const float &h)
{
	ratio = w / h;
	ratioChanged();
}

const gearoenix::math::Vector3Df &gearoenix::math::Camera::getLocation() const
{
	return location;
}

const float &gearoenix::math::Camera::getRatio() const
{
	return ratio;
}

float &gearoenix::math::Camera::getRatio()
{
	return ratio;
}