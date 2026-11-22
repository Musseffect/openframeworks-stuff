#pragma once
#include <ofVec2f.h>
#include <ofColor.h>
#include <ofRectangle.h>
#include <ofVbo.h>
#include <ofShader.h>

class Renderer {
	GLuint quadVbo;
	GLuint quadVao;
	GLuint pointVbo;
	std::vector<float> lineBufferData;
	ofShader shader;
public:
	Renderer();
	void clear();
	void addLine(const ofVec2f& a, const ofVec2f& b, const ofColor& cA, const ofColor& cB, float sigma);
	void render(const ofRectangle& viewport);
};