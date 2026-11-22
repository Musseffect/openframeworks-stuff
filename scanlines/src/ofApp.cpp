#include "ofApp.h"

constexpr int Resolution = 1024;

void ofApp::onLoadEvent(ofxDatGuiButtonEvent event)
{
	isLoaded = false;
	reloadButton->setEnabled(false);
	ofFileDialogResult dialogResult = ofSystemLoadDialog("Choose image", false, ofFilePath::getUserHomeDir());
	if (dialogResult.bSuccess)
	{
		if (!image.load(dialogResult.filePath))
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
}
//--------------------------------------------------------------
void ofApp::setup(){

	gui = new ofxDatGui(100, 100);
	gui->addHeader(":: Drag ::");
	gui->addFooter();
	gui->addFRM();
	loadButton = gui->addButton("load");
	reloadButton = gui->addButton("reload");
	toneMappingDropdown = gui->addDropdown("tone mapping", std::vector<std::string>({ "Exponential", "Reinhard" }));
	toneMappingDropdown->select(0);
	loadButton->onButtonEvent(this, &ofApp::onLoadEvent);
	reloadButton->onButtonEvent(this, &ofApp::onReloadEvent);
	reloadButton->setEnabled(false);

	sigmaSlider = gui->addSlider("sigma", 0.f, 100.0f, 10.0f);
	sigmaSlider->setPrecision(3);
	gammaSlider = gui->addSlider("gamma", 0.f, 5.f, 2.2f);
	gammaSlider->setPrecision(2);
	biasSlider = gui->addSlider("bias", 0.f, 1.f, 0.1f);
	biasSlider->setPrecision(2);
	gainSlider = gui->addSlider("gain", -10.f, 10.f, 0.f);
	gainSlider->setPrecision(2);

	for (int i = 0; i < 2; ++i)
	{
		offscreenBuffers[i].allocate(Resolution, Resolution, GL_RGB32F_ARB);
		offscreenBuffers[i].clearColorBuffer(ofFloatColor(0.0f, 0.0f, 0.0f));
	}
	toneMappingShaders[static_cast<int>(ToneMapping::Exponential)].setupShaderFromFile(GL_VERTEX_SHADER, "./shaders/screenRender.vert");
	toneMappingShaders[static_cast<int>(ToneMapping::Exponential)].setupShaderFromFile(GL_FRAGMENT_SHADER, "./shaders/exponentialToneMapping.frag");
	toneMappingShaders[static_cast<int>(ToneMapping::Exponential)].linkProgram();
	toneMappingShaders[static_cast<int>(ToneMapping::Reinhard)].setupShaderFromFile(GL_VERTEX_SHADER, "./shaders/screenRender.vert");
	toneMappingShaders[static_cast<int>(ToneMapping::Reinhard)].setupShaderFromFile(GL_FRAGMENT_SHADER, "./shaders/reinhardToneMapping.frag");
	toneMappingShaders[static_cast<int>(ToneMapping::Reinhard)].linkProgram();
	float vertices[] = {
		-1, 1,
		-1, -1,
		1, 1,
		1, -1
	};
	glGenVertexArrays(1, &quadVao);
	glBindVertexArray(quadVao);
	glGenBuffers(1, &quadVbo);
	glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	int size = 2;
	GLint type = GL_FLOAT;
	bool normalize = false;
	int stride = 0;
	int offset = 0;
	glVertexAttribPointer(
		0, size, type, normalize, stride, (void*)offset);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	for (int y = 0; y < 1; y += 10) {
		float cy = float(y) / Resolution * 2.f - 1.f;
		cy = 0.0f;
		float dx = 5 / Resolution * 2.f;
		for (int x = 0; x < Resolution; x += 5) {
			float cx = float(x) / float(Resolution) * 2.f - 1.f;
			ofVec2f p1 = ofVec2f(cx, cy);
			ofVec2f p2 = ofVec2f(cx + dx, cy);
			float c1 = ofNoise(p1 * 0.5 * 10 + 0.5) * 10.0f;
			float c2 = ofNoise(p2 * 0.5 * 10 + 0.5) * 10.0f;
			//p1.y = c1;
			//p2.y = c2;
			//c1 = 1.0f;
			//c2 = 1.0f;
			renderer.addLine(p1, p2, ofColor(c1, c1, c1), ofColor(c2, c2, c2), 10.0f / Resolution);
		}
	}
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
	offscreenBuffers[currentBuffer].bind();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glViewport(0, 0, Resolution, Resolution);
	float sigma = sigmaSlider->getValue() / Resolution;
	/*renderer.clear();
	for (int i = 0; i < 10; ++i)
	{
		const float t = static_cast<float>(time.getAsSeconds());
		float phase = i * 0.1 * 6.28;
		float nextPhase = (i + 1) * 0.1 * 6.28;
		float x = std::sin(t * 6.28 + phase);
		float y = std::cos(t * 6.28 + phase);
		float nextX = std::sin(t * 6.28 + nextPhase);
		float nextY = std::cos(t * 6.28 + nextPhase);
		renderer.addLine(ofVec2f(x, y), ofVec2f(nextX, nextY), ofColor(1.0, 1.0, 1.0), ofColor(1.0, 1.0, 1.0), sigma);
	}*/
	//renderer.addLine(ofVec2f(-1.0, 0.0), ofVec2f(1.0, 0.0), ofColor(1.0, 1.0, 1.0), ofColor(1.0, 1.0, 1.0), sigma);
	ofRectangle viewport;
	viewport.setFromCenter(ofVec2f(0.0, 0.0), 2.0, 2.0);
	renderer.render(viewport);
	offscreenBuffers[currentBuffer].unbind();
	int toneMappingIdx = toneMappingDropdown->getSelected()->getIndex();
	auto& toneMappingShader = toneMappingShaders[toneMappingIdx];
	glUseProgram(toneMappingShader.getProgram());
	glActiveTexture(GL_TEXTURE0);
	const auto& texData = offscreenBuffers[currentBuffer].getTexture().getTextureData();
	//ofLog(OF_LOG_NOTICE, std::to_string(offscreenBuffers[currentBuffer].getTexture().getTextureData().textureID));
	glBindTexture(texData.textureTarget, texData.textureID);

	//ofLog(OF_LOG_NOTICE, std::to_string(glGetError()));
	toneMappingShader.setUniform1i("backbuffer", 0);
	toneMappingShader.setUniform1f("gain", std::pow(2, gainSlider->getValue()));
	toneMappingShader.setUniform1f("bias", biasSlider->getValue());
	toneMappingShader.setUniform1f("gamma", gammaSlider->getValue());
	float backbufferRatio = 1;
	const float screenWidth = ofGetWindowWidth();
	const float screenHeight = ofGetWindowHeight();
	float screenRatio = screenHeight / screenWidth;
	//scale backbuffer image so it would fit in canvas
	ofVec2f scale;
	if (screenRatio > backbufferRatio)
		scale = ofVec2f(1.0, backbufferRatio / screenRatio);
	else
		scale = ofVec2f(screenRatio / backbufferRatio, 1.0);
	toneMappingShader.setUniform2f("scale", scale);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screenWidth, screenHeight);
	glBindVertexArray(quadVao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);
	currentBuffer = 1 - currentBuffer;
	gui->draw();
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
