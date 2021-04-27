#include "ofApp.h"




//--------------------------------------------------------------
void ofApp::setup(){
	showGUI = true;

	offscreenBuffer.allocate(width,height, GL_RGBA);
	gui.setup("Parameters","settings.xml");
	globalGroup.setup("Group");
	globalGroup.add(aperture.setup("apertrue",0.1,0,2));
	globalGroup.add(focusDistance.setup("focus distance", 2, 0, 40));
	globalGroup.add(alpha.setup("alpha", 0.03, 0, 1));
	globalGroup.add(pointSize.setup("point size", 0.5, 0, 10));
	globalGroup.add(samples.setup("samples", 3, 1, 10));
	globalGroup.add(frames.setup("frames", 1000, 100, 10000));
	globalGroup.add(reset.setup("reset"));
	gui.add(&globalGroup);
	cameraGroup.setup("Camera");
	cameraGroup.add(fov.setup("fov",50,0,360));
	cameraGroup.add(projectionType.setup("projection type", 0, 0, 3));
	cameraGroup.add(eye.setup("eye", ofVec3f(0,2,1.9), ofVec3f(-10), ofVec3f(10)));
	cameraGroup.add(target.setup("target", ofVec3f(0,0,0), ofVec3f(-10), ofVec3f(10)));
	cameraGroup.add(up.setup("up", ofVec3f(0,1,0), ofVec3f(-10), ofVec3f(10)));
	cameraGroup.add(shift.setup("shift", ofVec2f(0, 0), ofVec2f(-10), ofVec2f(10)));
	cameraGroup.add(useCameraEyeFocus.setup("camera eye focus",true));
	cameraGroup.add(focusTarget.setup("focus target", ofVec3f(0, 0), ofVec3f(-10), ofVec3f(10)));
	cameraGroup.add(focusCenter.setup("focus center", ofVec3f(0, 0), ofVec3f(-10), ofVec3f(10)));
	cameraGroup.add(dofType.setup("dof type", 0,0,2));
	cameraGroup.add(dofPower.setup("dof power", 1, -10, 10));
	cameraGroup.add(dofMin.setup("dof min", 0, 0, 4));
	gui.add(&cameraGroup);
	gui.loadFromFile("settings.xml");

	ofBackground(0);
	frame = 0;

	renderer.setup(generateGeometry());
}

//--------------------------------------------------------------
void ofApp::update(){

}

void ofApp::debugDraw()
{
	/*ofSetupScreenPerspective(ofGetScreenWidth(), ofGetScreenHeight(),
		75.0f, 0.01, 100);*/
	/*ofCamera camera;
	camera.setFov(15.0);
	camera.setPosition(0, 2, 1.9);
	camera.setAspectRatio(ofGetScreenWidth() / ofGetScreenHeight());
	camera.lookAt(glm::vec3(0,0,0),glm::vec3(0,1,0));
	camera.setNearClip(0.01);
	camera.setFarClip(100);
	ofBackground(0);
	ofSetColor(255);
	camera.begin();

	vector<Line> lines = generateGeometry();

	for (int i = 0; i < lines.size(); i++)
	{
		ofDrawLine(lines[i].getP1(),lines[i].getP2());
	}
	camera.end();*/
}

void ofApp::drawLines()
{
	Camera *cam;
	switch ((int)projectionType)
	{
	case 0:
		cam = new PerspectiveCamera((ofVec3f)eye, (ofVec3f)target, (ofVec3f)up, width, height, glm::radians<float>(fov));
		break;
	case 1:
		cam = new OrthoCamera((ofVec3f)eye, (ofVec3f)target, (ofVec3f)up, width, height);
		break;
	case 2:
		cam = new FisheyeCamera((ofVec3f)eye, (ofVec3f)target, (ofVec3f)up, width, height, glm::radians<float>(fov));
		break;
	case 3:
		cam = new ReversePerspectiveCamera((ofVec3f)eye, (ofVec3f)target, (ofVec3f)up,width,height, glm::radians<float>(fov));
		break;
	default:
		return;
	}

	renderer.setAlpha(alpha);
	renderer.setDofMode(dofType);
	if (useCameraEyeFocus)
	{
		renderer.setFocusCenter((ofVec3f)eye);
		renderer.setFocusDirection(((ofVec3f)target - (ofVec3f)eye).normalize());
	}
	else
	{
		renderer.setFocusCenter((ofVec3f)focusCenter);
		renderer.setFocusDirection(((ofVec3f)focusTarget-(ofVec3f)focusCenter).normalize());
	}
	renderer.setAperture(aperture);
	renderer.setFocusDistance(focusDistance);
	renderer.setPointSize(pointSize);
	renderer.setPower(dofPower);
	renderer.setDofMin(dofMin);
	renderer.setShift((ofVec2f)shift);
	renderer.draw(cam);
	delete cam;

	/*ofVec3f focusCenter = ofVec3f(sqrt(2), 0, -sqrt(2));
	ofVec3f focusDirection = ofVec3f(-1, 0, 1).normalize();

	ofEnableBlendMode(ofBlendMode::OF_BLENDMODE_ALPHA);

	//ofSetColor(0,0,0,12);
	//ofClearAlpha();
	//ofRect(0,0,width,height);
	ofFill();
	DOFCamera cam((ofVec3f)eye, (ofVec3f)target, (ofVec3f)up);
	cam.setProjection(projectionType);
	cam.setFOV(fov);


	ofVec3f _fc = cam.projectWtoL(focusCenter);
	ofSetColor(255.f, 0, 0, 255.0f);
	if(_fc.z>0.0f)
		ofDrawCircle(_fc.x, _fc.y, 10.0);

	ofMesh mesh;
	mesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_POINTS);
	for (int i = 0; i < lines.size(); i++)
	{
		Line& line = lines[i];
		glLineWidth(0.1);
		ofVec3f point = line.sample();
		ofVec3f p = point - cam.getPos();
		p = ofVec3f(p.dot(cam.getRight()),p.dot(cam.getUp()),p.dot(cam.getDir()));
		
		if (p.z < 0.0)
			continue;
		float dist = focusDirection.dot(point-focusCenter);
		float r = aperture * cam.dof(focusDistance,dist);
		//ofVec3f w = p+randSphere(r);
		ofVec3f w = p+randCircle(r);
		//ofVec3f w = p;
		if (w.z < 0.0)
			continue;
		ofFloatColor col = ofFloatColor(1.0f,1.0f,1,alpha);//this.getColor(r / a, dist);
		//ofSetColor(col,3);
		w = cam.project(w);
		int x = (w.x + 1.0f) * width / 2.0f;
		int y = (w.y + 1.0f) * height / 2.0f;
		mesh.addVertex(ofPoint(x, y, 0));
		mesh.addColor(col);
		//ofDrawCircle(x, y,1);
	}
	glPointSize(pointSize);
	mesh.draw();*/
}
void ofApp::drawAnim(float time)
{
	float angle = glm::two_pi<float>()*time;
	ofVec3f eye(sin(angle),0,cos(angle));
	ofVec3f target(0,0,0);
	ofVec3f up(0,1,0);

	Camera *cam;
	cam = new PerspectiveCamera(eye,target,up,width,height,glm::radians(75.0f));

	float alpha = 2;
	float aperture = 0.1;
	float focusDistance = 3.0;
	int dofMode = 0;
	float dofMin = 0.0;
	ofVec3f focusCenter = ofVec3f(0,0,-3);
	ofVec3f focusDirection = ofVec3f(0,0,1);
	float pointSize = 1.0f;
	float power = 1.0;
	ofVec2f shift = ofVec2f(0,0);

	renderer.setAlpha(alpha);
	renderer.setDofMode(dofMode);
	renderer.setAperture(aperture);
	renderer.setFocusDistance(focusDistance);
	renderer.setFocusCenter(focusCenter);
	renderer.setFocusDirection(focusDirection);
	renderer.setPointSize(pointSize);
	renderer.setDofMin(dofMin);
	renderer.setPower(power);
	renderer.setShift(shift);
	renderer.draw(cam);
	delete cam;
}

//--------------------------------------------------------------
void ofApp::draw(){

	


#ifdef DRAWANIM
	const int frames = 4000;
	const int animFrames = 48;
	int animFrame = frame / frames;

	for (int i = 0; i < samples; i++)
	{
		float time = (animFrame + ofRandom()) / animFrames;
		drawAnim(time);
	}
	if ((frame+1)% frames)
	{
		saveimage(animFrame);
		ofBackground(0, 255);
	}
	return;
#endif
	/*debugDraw();
	return;*/

	if (reset)
	{
		frame = 0;
	}
	if (frame < frames)
	{
		offscreenBuffer.begin();
		if(frame==0)
		{
			ofBackground(0,0);
		}
		for (int i = 0; i < samples; i++)
		{
			drawLines();
		}
		offscreenBuffer.end();
		frame++;
	}
	ofSetColor(255);
	ofDrawBitmapString("Frame :" + ofToString(frame), 10, 15);
	ofDrawBitmapString("Save screen - d", 10, 30);
	ofDrawBitmapString("Save image - s", 10, 45);
	ofDrawBitmapString("Hide gui - h", 10, 60);
	{
		int screenW = ofGetWindowWidth();
		int screenH = ofGetWindowHeight();
		int correctWidth;
		int correctHeight;
		int correctXPos;
		int correctYPos;
		int height = offscreenBuffer.getHeight();
		int width = offscreenBuffer.getWidth();
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
		ofDisableBlendMode();
		offscreenBuffer.draw(correctXPos, correctYPos, correctWidth, correctHeight);
	}
	if(showGUI)
		gui.draw();
}

vector<Line> ofApp::generateGeometry()
{
	vector<Line> lines;
	ofSeedRandom(177);
	int w = 161;
	int h = 101;
	int samplesX = 4;
	int samplesY = 4;
	vector<ofVec3f> nodes;
	for (int j = 0; j < h; j++) {
		for (int i = 0; i < w; i++) {
			float x = (i * 2.0f + 1.0f - w) / (w - 1.0f);
			float y = (j * 2.0f + 1.0f - h) / (h - 1.0f) * 0.7;
			float z = (sin(x * 10.0 + cos(y * 3.0f) * 2.0f) + 1.0f) * (1.0f + sin(y * 6.0f + 1.2)) * exp(-5.1 * (x * x));
			ofVec3f n = ofVec3f(x, z * 0.15, y);
			nodes.push_back(n);
			if (j != 0 && i % samplesX == 0) {
				int index1 = i + j * w;
				int index2 = i + (j - 1) * w;
				lines.push_back(Line(nodes[index1], nodes[index2]));
			}
			if (i != 0 && j % samplesY == 0) {
				int index1 = i + j * w;
				int index2 = (i - 1) + j * w;
				lines.push_back(Line(nodes[index1], nodes[index2]));
			}
		}
	}
	return lines;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 's')
	{
		ofPixels pixels;
		pixels.allocate(int(offscreenBuffer.getWidth()), int(offscreenBuffer.getHeight()), GL_RGBA);
		offscreenBuffer.readToPixels(pixels);
		ofSaveImage(pixels, "screenshots/" + ofGetTimestampString() + "screenshot.png", OF_IMAGE_QUALITY_BEST);
	}
	if (key == 'h')//hide gui
	{
		showGUI = !showGUI;
	}
	if (key == 'd')//save screen
	{
		ofImage screenshot;
		screenshot.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
		screenshot.save(boost::filesystem::path("screenshots/" + ofGetTimestampString() + "screenshot.png"), ofImageQualityType::OF_IMAGE_QUALITY_BEST);
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

void ofApp::exit()
{
	gui.saveToFile("settings.xml");
}
