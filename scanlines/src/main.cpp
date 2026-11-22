#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofDisableArbTex();
	ofGLFWWindowSettings settings;
	settings.setGLVersion(3, 3); //we define the OpenGL version we want to use
	settings.setSize(1024, 768);
	ofCreateWindow(settings);
	ofRunApp(new ofApp());

}
