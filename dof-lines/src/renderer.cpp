#include "renderer.h"


Line::Line(ofVec3f p1, ofVec3f p2)
{
	this->p1 = p1;
	this->p2 = p2;
}
ofVec3f Line::sample()
{
	return p1.getInterpolated(p2, ofRandomuf());
}
float Line::length()
{
	return p1.distance(p2);
}
ofVec3f Line::getP1()
{
	return p1;
}
ofVec3f Line::getP2()
{
	return p2;
}

ofVec3f randPointOnSphere() {
	float theta = 2 * PI * ofRandomuf();
	float phi = acos(ofRandom(-1, 1));
	return ofVec3f(
		sin(phi) * cos(theta),
		sin(phi) * sin(theta),
		cos(phi));
}

ofVec3f  randSphere(float radius) {
	float l = ofRandomuf();
	float u = ofRandom(-1, 1);
	float phi = ofRandom(0, 2 * PI);
	float R = radius * pow(l, 1 / 3);
	float _u = sqrt(1 - u * u);
	return ofVec3f(R * _u * cos(phi), R * _u * sin(phi), R * u);
}
ofVec3f randCircle(float radius) {
	float a = ofRandomuf() * 2.0 * PI;
	float r = radius * sqrt(ofRandomuf());
	float x = r * cos(a);
	float y = r * sin(a);
	return ofVec3f(x, y, 0);
	/*return p5.Vector.add(p5.Vector.mult(up,y),
						 p5.Vector.mult(right,x));*/
}


void Renderer::setup(vector<Line>& geometry)
{
	this->geometry = geometry;
	power = 0;
}
void Renderer::setPointSize(float ps)
{
	pointSize = ps;
}
void Renderer::setAlpha(float a)
{
	alpha = a;
}
void Renderer::setPower(float p)
{
	this->power = p;
}
void Renderer::setFocusCenter(ofVec3f fc)
{
	focusCenter = fc;
}
void Renderer::setShift(ofVec2f s)
{
	shift = s;
}
void Renderer::setFocusDirection(ofVec3f fd)
{
	focusDirection = fd.normalize();
}
void Renderer::setDofMode(float dm)
{
	dofMode = dm;
}
void Renderer::setAperture(float a)
{
	aperture = a;
}
void Renderer::setFocusDistance(float fd)
{
	focusDistance = fd;
}

void Renderer::setDofMin(float dm)
{
	dofMin = dm;
}
void Renderer::draw(Camera* cam)
{

	ofEnableBlendMode(ofBlendMode::OF_BLENDMODE_ALPHA);
	//ofClear(0, 0, 0, 0);
	//ofSetColor(0,0,0,12);
	//ofClearAlpha();
	//ofRect(0,0,width,height);
	ofFill();

	ofVec3f _fc = cam->projectWtoL(focusCenter);
	ofSetColor(255.f, 0, 0, 255.0f);
	if (_fc.z > 0.0f)
		ofDrawCircle(_fc.x, _fc.y, 10.0);

	ofMesh mesh;
	mesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_POINTS);
	for (int i = 0; i < geometry.size(); i++)
	{
		Line& line = geometry[i];
		glLineWidth(0.1);
		ofVec3f point = line.sample();
		//ofVec3f p = point - cam.getPos();
		//p = ofVec3f(p.dot(cam.getRight()), p.dot(cam.getUp()), p.dot(cam.getDir()));
		ofVec3f p = cam->projectWtoL(point);

		float dist = focusDirection.dot(point - focusCenter);
		float r = aperture * this->dofCurve(focusDistance, dist);
		ofVec3f w = p + randSphere(r);
		//ofVec3f w = p + randCircle(r);
		//ofVec3f w = p;
		if (w.z < 0.0)
			continue;
		ofFloatColor col = ofFloatColor(1.0f, 1.0f, 1, alpha);//this.getColor(r / a, dist);
		//ofSetColor(col,3);
		w = cam->project(w);
		/*w.x = w.x / (w.z * _w);
		w.y = w.y / (w.z * _w);*/
		int x = (w.x + shift.x + 1.0f) * cam->getWidth() / 2.0f;
		int y = (-w.y + shift.y + 1.0f) * cam->getHeight() / 2.0f;
		mesh.addVertex(ofPoint(x, y, 0));
		mesh.addColor(col);
		//ofDrawCircle(x, y,1);
	}
	glPointSize(pointSize);
	mesh.draw();
}
float Renderer::dofCurve(float d, float f)
{
	switch (dofMode)
	{
	case ThinLens:
		return dofThinLens(d, f);
		break;
	case Pow:
		return dofPow(d, f);
		break;
	case PowThinLens:
		return dofPowThinLens(d, f);
	}
	return 0.0;
}