#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"

class ofApp : public ofBaseApp{

		ofImage image;
		ofImage difference;
		ofxDatGuiButton* loadButton;
		ofxDatGuiButton* reloadButton;
		ofxDatGuiSlider* minVarianceSlider;
		ofxDatGuiSlider* maxDepthSlider;
		ofxDatGuiSlider* minWidthSlider;
		ofxDatGuiSlider* minHeightSlider;
		ofxDatGuiSlider* maxRatioSlider;
		ofxDatGuiDropdown* showDropdown;
		ofxDatGuiDropdown* objFunctionDropdown;
		
		ofFbo offscreenBuffer;
		ofxDatGui* gui;
		bool isLoaded;
	public:
		void setup();
		void update();
		void draw();
		void processImage();
		void onLoadEvent(ofxDatGuiButtonEvent event);
		void onReloadEvent(ofxDatGuiButtonEvent event);

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
