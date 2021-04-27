#include "camera.h"


Camera::Camera(ofVec3f pos, ofVec3f target, ofVec3f up, float width, float height)
{
	this->pos = pos;
	dir = (target - pos).getNormalized();
	right = dir.getCrossed(up).getNormalized();
	this->up = right.getCrossed(dir).getNormalized();
	this->width = width;
	this->height = height;
}
float Camera::getWidth()
{
	return width;
}
float Camera::getHeight()
{
	return height;
}
ofVec3f Camera::getPos()
{
	return pos;
}
ofVec3f Camera::getDir()
{
	return dir;
}
ofVec3f Camera::getRight()
{
	return right;
}
ofVec3f Camera::getUp()
{
	return up;
}
ofVec3f Camera::projectWtoL(ofVec3f vec)
{
	ofVec3f local = vec - pos;
	local = ofVec3f(local.dot(right), local.dot(up), local.dot(dir));
	return local;
}

PerspectiveCamera::PerspectiveCamera(ofVec3f pos, ofVec3f target, ofVec3f up, float width, float height, float fov) :Camera(pos, target, up, width, height)
{
	this->fov = fov;
}
ofVec3f PerspectiveCamera::project(ofVec3f lp)
{
	float _w = tan(fov / 2);
	return ofVec3f(lp.x / (lp.z * _w), lp.y / (lp.z * _w), lp.z);
}


FisheyeCamera::FisheyeCamera(ofVec3f pos, ofVec3f target, ofVec3f up, float width, float height, float fov) :Camera(pos, target, up, width, height)
{
	this->fov = fov;
}
ofVec3f FisheyeCamera::project(ofVec3f lp)
{
	float theta = atan2(lp.y, lp.x);
	float r = sqrt(lp.y*lp.y + lp.x*lp.x);
	float phi = atan2(r, lp.z) / fov;
	return ofVec3f(phi * cos(theta), phi * sin(theta), lp.z);
}

OrthoCamera::OrthoCamera(ofVec3f pos, ofVec3f target, ofVec3f up, float width, float height) :Camera(pos, target, up, width, height)
{
}
ofVec3f OrthoCamera::project(ofVec3f lp)
{
	return lp / ofVec3f(width, height, 1.0f);
}

ReversePerspectiveCamera::ReversePerspectiveCamera(ofVec3f pos, ofVec3f target, ofVec3f up, float width, float height, float fov) :Camera(pos, target, up, width, height)
{
	this->fov = fov;
}
ofVec3f ReversePerspectiveCamera::project(ofVec3f lp)
{
	float _w = tan(fov / 2) * 5;
	return ofVec3f(lp.x*lp.z / _w, lp.y*lp.z / _w, lp.z);
}