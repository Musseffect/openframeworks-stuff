#include "ofApp.h"
#include "treeMapper.h"

void ofApp::onLoadEvent(ofxDatGuiButtonEvent event)
{

	int screenW = ofGetWindowWidth();
	int screenH = ofGetWindowHeight();
	isLoaded = false;
	ofFileDialogResult dialogResult = ofSystemLoadDialog("Choose image", false, ofFilePath::getUserHomeDir());
	if (dialogResult.bSuccess)
	{
		try
		{
			image.load(dialogResult.filePath);
		}
		catch (...)
		{
			isLoaded = false;
			MessageBoxA(NULL, "Problem encountered during the processing of loaded image.", "Error", 0);
			std::cout << "Problem encountered during the processing of loaded image" << std::endl;
			return;
		}

		offscreenBuffer.allocate(screenW, screenH, GL_RGBA);
		processImage();
		isLoaded = true;
	}

}

void ofApp::processImage()
{
	treeMapper mapper(image.getWidth(), image.getHeight(), image);
	offscreenBuffer.begin();
	mapper.draw(offscreenBuffer.getWidth() / float(image.getWidth()),offscreenBuffer.getHeight()/float(image.getHeight()));
	offscreenBuffer.end();
}


//--------------------------------------------------------------
void ofApp::setup(){
	gui = new ofxDatGui(100, 100);
	gui->addHeader(":: Drag ::");
	gui->addFooter();
	gui->addFRM();
	loadButton = gui->addButton("load");

	loadButton->onButtonEvent(this, &ofApp::onLoadEvent);



}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	if (isLoaded)
		offscreenBuffer.draw(0,0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
