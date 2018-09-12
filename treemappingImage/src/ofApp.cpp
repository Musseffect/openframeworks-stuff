#include "ofApp.h"
#include "treeMapper.h"

void ofApp::onLoadEvent(ofxDatGuiButtonEvent event)
{
	isLoaded = false;
	reloadButton->setEnabled(false);
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

		processImage();
		isLoaded = true;
		reloadButton->setEnabled(true);
	}
}

void ofApp::onReloadEvent(ofxDatGuiButtonEvent event)
{
	reloadButton->setEnabled(false);
	isLoaded = false;
	processImage();
	isLoaded = true;
	reloadButton->setEnabled(true);
}


void ofApp::processImage()
{
	int screenW = ofGetWindowWidth();
	int screenH = ofGetWindowHeight();
	int width = image.getWidth();
	int height = image.getHeight();
	offscreenBuffer.allocate(width, height, GL_RGBA);
	treeMapper mapper(width,height, image, float(minVarianceSlider->getValue()),
		maxDepthSlider->getValue(),minWidthSlider->getValue(),minHeightSlider->getValue(),maxRatioSlider->getValue(),
		objFunctionDropdown->getSelected()->getIndex());
	offscreenBuffer.begin(); 
	mapper.draw(1.f,1.f,0.f,0.f);

	offscreenBuffer.end();
	ofPixels pixels;
	pixels.allocate(width, height, GL_RGBA);
	difference.allocate(width, height, ofImageType::OF_IMAGE_GRAYSCALE);
	offscreenBuffer.readToPixels(pixels);
	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
		{
			ofColor pixel=pixels.getColor(i,j);
			ofColor imagePixel = image.getColor(i,j);
			glm::vec3 diff=(glm::vec3(pixel.r,pixel.g,pixel.b)-glm::vec3(imagePixel.r,imagePixel.g,imagePixel.b));
			ofColor color;
			color.set((abs(diff.x)+abs(diff.y)+abs(diff.z))/3.f,255.f);
			difference.setColor(i, j , color);
		}
	difference.update();
}


//--------------------------------------------------------------
void ofApp::setup(){
	gui = new ofxDatGui(100, 100);
	gui->addHeader(":: Drag ::");
	gui->addFooter();
	gui->addFRM();
	loadButton = gui->addButton("load");
	reloadButton = gui->addButton("reload");
	showDropdown = gui->addDropdown("show", std::vector<std::string>({"result","original","difference"}));
	showDropdown->select(0);
	objFunctionDropdown = gui->addDropdown("objective function", std::vector<std::string>({"minMax","minSum","minProportionalSum","minMult","minProportinalMult","maxMeanDifference"}));
	objFunctionDropdown->select(0);
	loadButton->onButtonEvent(this, &ofApp::onLoadEvent);
	reloadButton->onButtonEvent(this,&ofApp::onReloadEvent);
	reloadButton->setEnabled(false);

	minVarianceSlider = gui->addSlider("min variance",0.f,10000.f,500.f);
	maxDepthSlider = gui->addSlider("max tree depth", 0.f, 100.f, 12.f);
	maxDepthSlider->setPrecision(0);
	minWidthSlider = gui->addSlider("min width", 1.f, 500.f, 14.f);
	minWidthSlider->setPrecision(0);
	minHeightSlider = gui->addSlider("min height", 1.f, 500.f, 14.f);
	minHeightSlider->setPrecision(0);
	maxRatioSlider = gui->addSlider("max ratio", 0.f, 10.f, 1.f);


}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	if (isLoaded)
	{
		ofxDatGuiDropdownOption*option = showDropdown->getSelected();
		int state = option->getIndex();
		int screenW = ofGetWindowWidth();
		int screenH = ofGetWindowHeight();
		int correctWidth;
		int correctHeight;
		int correctXPos;
		int correctYPos;
		int height = image.getHeight();
		int width = image.getWidth();
		float screenRatio = float(screenW) / float(screenH);
		float imageRatio = float(width) / float(height);
		if (imageRatio > screenRatio)
		{
			correctWidth = screenW;// float(baseImage.getWidth());
			correctHeight = float(height*screenW) / (width);
			correctXPos = 0;
			correctYPos = (screenH - correctHeight) / 2;
		}
		else
		{
			correctWidth = float(width*screenH) / (height);
			correctHeight = screenH;
			correctXPos = (screenW - correctWidth) / 2;
			correctYPos = 0;
		}
		switch (state)
		{
			case 0:
			{	
				offscreenBuffer.draw(correctXPos, correctYPos, correctWidth, correctHeight);
				break;
			}
			case 1:
			{
				image.draw(correctXPos, correctYPos, correctWidth, correctHeight);
				break;
			}
			case 2:
			{
				difference.draw(correctXPos, correctYPos, correctWidth, correctHeight);
				break;
			}
		}
	}
	gui->draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'h')//hide gui
	{
		gui->setVisible(!gui->getVisible());
	}
	if (key == 's')//save offscreenBuffer
	{
		ofPixels pixels;
		pixels.allocate(int(offscreenBuffer.getWidth()),int(offscreenBuffer.getHeight()), GL_RGBA);
		offscreenBuffer.readToPixels(pixels);
		ofSaveImage(pixels, "screenshots/" + ofGetTimestampString() + "screenshot.png",OF_IMAGE_QUALITY_BEST);
	}
	if (key == 'd')//save screen
	{
		ofImage screenshot;
		screenshot.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
		screenshot.save(boost::filesystem::path("screenshots/"+ofGetTimestampString() + "screenshot.png"),ofImageQualityType::OF_IMAGE_QUALITY_BEST);
	}
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
