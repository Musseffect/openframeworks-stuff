#pragma once
#include "ofMain.h"
#include "ofShader.h"
#include "Renderer.h"
#include "ofxDatGui.h"

class ofApp : public ofBaseApp{
	enum class ToneMapping {
		Exponential = 0,
		Reinhard
	};
	Renderer renderer;
	ofTime time;
	ofShader toneMappingShaders[2];
	GLuint quadVbo;
	GLuint quadVao;

	ofFbo offscreenBuffers[2];
	int currentBuffer = 0;
	ofImage image;
	ofxDatGuiButton* loadButton;
	ofxDatGuiButton* reloadButton;
	ofxDatGuiSlider* gainSlider;
	ofxDatGuiSlider* biasSlider;
	ofxDatGuiSlider* gammaSlider;
	ofxDatGuiSlider* sigmaSlider;
	ofxDatGuiDropdown* toneMappingDropdown;

	bool isLoaded = false;

	ofxDatGui* gui;
	void onLoadEvent(ofxDatGuiButtonEvent event);
	void onReloadEvent(ofxDatGuiButtonEvent event);
	void processImage();
	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

};
