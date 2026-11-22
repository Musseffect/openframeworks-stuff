#include "Renderer.h"
#include <string>
#include <ofLog.h>
#include <glm\glm.hpp>


Renderer::Renderer() {
	shader.setupShaderFromFile(GL_VERTEX_SHADER, "./shaders/lineRendering.vert");
	shader.setupShaderFromFile(GL_FRAGMENT_SHADER, "./shaders/lineRendering.frag");
	if (!shader.linkProgram())
		ofLog(OF_LOG_ERROR, "Cannot initialize line shader");

	glGenBuffers(1, &pointVbo);
	float vertices[] = {
		-1, 1,
		-1, -1,
		1, 1,
		1, -1
	};
	glGenBuffers(1, &quadVbo);
	glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &quadVao);
	glBindVertexArray(quadVao);
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
}
void Renderer::clear() {
	lineBufferData.clear();
}
void Renderer::addLine(const ofVec2f& a, const ofVec2f& b, const ofColor& cA, const ofColor& cB, float sigma) {
	float length = (a - b).length();
	lineBufferData.push_back(a.x);
	lineBufferData.push_back(a.y);
	lineBufferData.push_back(std::atan2(b.y - a.y, b.x - a.x));
	lineBufferData.push_back(length);
	lineBufferData.push_back(cA.r);
	lineBufferData.push_back(cA.g);
	lineBufferData.push_back(cA.b);
	lineBufferData.push_back(sigma);
	lineBufferData.push_back(cB.r);
	lineBufferData.push_back(cB.g);
	lineBufferData.push_back(cB.b);
	lineBufferData.push_back(0.0f); // dummy
}
void Renderer::render(const ofRectangle& viewport) {

	if (lineBufferData.size() > 11) {
		glUseProgram(shader.getProgram());
		glm::mat3 viewportMatrix(
			2.0f / viewport.getWidth(), 0.0, -1.0f,
			0.0, 2.0f / viewport.getHeight(), -1.0f,
			0.0f, 0.0f, 1.0);
		shader.setUniformMatrix3f("transform", viewportMatrix, 1);
		glBindVertexArray(quadVao);
		glBindBuffer(GL_ARRAY_BUFFER, pointVbo);
		glBufferData(GL_ARRAY_BUFFER, 4 * lineBufferData.size(), lineBufferData.data(), GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, false, 12 * 4, (void*)0);
		glVertexAttribDivisor(1, 1);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, false, 12 * 4, (void*)(4 * 4));
		glVertexAttribDivisor(2, 1);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, false, 12 * 4, (void*)(8 * 4));
		glVertexAttribDivisor(3, 1);

		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, lineBufferData.size() / 12);
		glUseProgram(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}