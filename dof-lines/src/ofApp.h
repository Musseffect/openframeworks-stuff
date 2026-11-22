#pragma once

#include "ofMain.h"
#pragma push
#pragma warning(disable: 4018)
#include "ofxGui.h"
#pragma pop
#include "renderer.h"
#include "camera.h"


class DOFCamera
{
	ofVec3f pos;
	ofVec3f dir;
	ofVec3f right;
	ofVec3f up;
	int projection;
	float fov;
public:
	DOFCamera(ofVec3f pos, ofVec3f target, ofVec3f up)
	{
		this->pos = pos;
		dir = (target - pos).getNormalized();
		right = dir.getCrossed(up).getNormalized();
		this->up = right.getCrossed(dir).getNormalized();
		projection = 0;
		fov = glm::radians(75.0);
	}
	void setFOV(float fov)
	{
		this->fov = glm::radians(fov);
	}
	void setProjection(int i)
	{
		projection = i;
	}
	ofVec3f getPos()
	{
		return pos;
	}
	ofVec3f getDir()
	{
		return dir;
	}
	ofVec3f getRight()
	{
		return right;
	}
	ofVec3f getUp()
	{
		return up;
	}
	ofVec3f reversePerspective(ofVec3f vec)
	{
		//float fov = glm::radians(70.0);
		float _w = tan(fov / 2) * 5;
		return ofVec3f(vec.x*vec.z / _w, vec.y*vec.z / _w, vec.z);
	}
	ofVec3f perspective(ofVec3f vec)
	{
		//float fov = glm::radians(50.0);
		float _w = tan(fov / 2);
		return ofVec3f(vec.x / (vec.z * _w), vec.y / (vec.z * _w), vec.z);
	}
	ofVec3f ortho(ofVec3f vec)
	{
		return vec;
	}
	ofVec3f fisheye(ofVec3f vec)
	{
		//float fov = glm::radians(25.0);
		float theta = atan2(vec.y, vec.x);
		float r = sqrt(vec.y*vec.y + vec.x*vec.x);
		float phi = atan2(r, vec.z) / fov;
		return ofVec3f(phi * cos(theta), phi * sin(theta), vec.z);
	}
	ofVec3f project(ofVec3f vec)
	{
		switch (projection)
		{
		case 0:
			return perspective(vec);
		case 1:
			return ortho(vec);
		case 2:
			return fisheye(vec);
		case 3:
			return reversePerspective(vec);
		}
		return vec;
	}
	ofVec3f projectWtoL(ofVec3f vec)
	{
		ofVec3f local = vec - pos;
		local = ofVec3f(local.dot(right), local.dot(up), local.dot(dir));
		return project(local);
	}
	float dof(float focusDistance, float distance)
	{
		return abs(focusDistance - distance) / focusDistance * distance;
	}
};



class ofApp : public ofBaseApp {
	const int width = 1080;
	const int height = 1080;
	ofFbo offscreenBuffer;
	ofxPanel gui;
	ofxGuiGroup globalGroup;
	ofxGuiGroup cameraGroup;
	ofxFloatSlider aperture;
	ofxFloatSlider focusDistance;
	ofxFloatSlider fov;

	ofxIntSlider projectionType;

	ofxFloatSlider pointSize;
	ofxFloatSlider alpha;
	ofxIntSlider samples;
	ofxIntSlider frames;
	ofxVec3Slider eye;
	ofxVec3Slider target;
	ofxVec3Slider up;
	ofxVec2Slider shift;
	ofxButton reset;

	ofxIntSlider dofType;
	ofxFloatSlider dofMin;
	ofxFloatSlider dofPower;

	ofxToggle useCameraEyeFocus;
	ofxVec3Slider focusTarget;
	ofxVec3Slider focusCenter;

	Renderer renderer;
	int frame;
	bool showGUI;
public:
	void setup();
	void update();
	void draw();


	static vector<Line> generateGeometry();

	void debugDraw();
	void drawLines();

	void drawAnim(float time);
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	void exit();
};
