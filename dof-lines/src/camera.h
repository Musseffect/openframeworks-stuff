#pragma once
#include "ofMain.h"

class Camera
{
protected:
	ofVec3f pos;
	ofVec3f dir;
	ofVec3f right;
	ofVec3f up;
	float width;
	float height;
public:
	Camera(ofVec3f pos, ofVec3f target, ofVec3f up, float width, float height);
	float getWidth();
	float getHeight();
	ofVec3f getPos();
	ofVec3f getDir();
	ofVec3f getRight();
	ofVec3f getUp();
	ofVec3f projectWtoL(ofVec3f vec);
	virtual ofVec3f project(ofVec3f lp) = 0;
};

class PerspectiveCamera :public Camera
{
	float fov;
public:
	PerspectiveCamera(ofVec3f pos, ofVec3f target, ofVec3f up, float width, float height, float fov);
	virtual ofVec3f project(ofVec3f lp) override;
};

class FisheyeCamera :public Camera
{
	float fov;
public:
	FisheyeCamera(ofVec3f pos, ofVec3f target, ofVec3f up, float width, float height, float fov);
	virtual ofVec3f project(ofVec3f lp) override;
};

class OrthoCamera :public Camera
{
public:
	OrthoCamera(ofVec3f pos, ofVec3f target, ofVec3f up, float width, float height);
	virtual ofVec3f project(ofVec3f lp) override;
};
class ReversePerspectiveCamera :public Camera
{
	float fov;
public:
	ReversePerspectiveCamera(ofVec3f pos, ofVec3f target, ofVec3f up, float width, float height, float fov);
	virtual ofVec3f project(ofVec3f lp) override;
};