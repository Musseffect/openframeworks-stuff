#pragma once
#include "ofMain.h"
#include "camera.h"

struct Line
{
	ofVec3f p1;
	ofVec3f p2;
	Line(ofVec3f p1, ofVec3f p2);
	ofVec3f sample();
	float length();
	ofVec3f getP1();
	ofVec3f getP2();
};

class Renderer
{
	vector<Line> geometry;
private:
	float smoothAbs(float x)
	{
		return sqrt(x*x + dofMin * dofMin);
	}
	float dofThinLens(float d, float f)
	{
		return smoothAbs(d-f)/(2.0f*f*d);
	}
	float dofPow(float d, float f)
	{
		return pow(smoothAbs(d-f),power);
	}
	float dofPowThinLens(float d, float f)
	{
		return pow(smoothAbs(d - f), power)/(2.0f*f*d);
	}
	float power;
	ofVec3f focusCenter;
	ofVec3f focusDirection;
	float aperture;
	float focusDistance;
	ofVec2f shift;
	float alpha;
	float pointSize;
	int dofMode;
	float dofMin;
public:
	enum DofMode
	{
		ThinLens = 0,
		Pow = 1,
		PowThinLens = 2
	};
	void setAperture(float a);
	void setFocusDistance(float fd);
	void setup(vector<Line>& geometry);
	void setPointSize(float ps);
	void setAlpha(float a);
	void setPower(float p);
	void setDofMin(float dm);
	void setFocusCenter(ofVec3f fc);
	void setShift(ofVec2f s);
	void setFocusDirection(ofVec3f fd);
	void setDofMode(float dm);
	void draw(Camera* cam);
	float dofCurve(float d, float f);
};