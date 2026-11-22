#version 330
#define PI 3.1415926535898
precision highp float;

in vec3 fColor;
in float fSigma;
in float fDist;
in vec2 fPosition;

out vec4 color;

//Error function
float erf(float x)
{
	float s = sign(x), a = abs(x);
	x = 1.0 + (0.278393 + (0.230389 + 0.078108 * (a * a)) * a) * a;
	x *= x;
	return s - s / (x * x);
}
//Simplified formula
float intensity(vec2 p, float d, float sigma)
{
	float f = 1.0 / (sqrt(2.)*sigma);
	return (erf(p.x*f) - erf((p.x - d)*f))
		*exp(-p.y*p.y*f*f) / (2.*d);
}
float pointIntensity(vec2 pos, float sigma) {
	float f = 1.0 / sigma;
	return exp(-dot(pos, pos)*f*f*0.5)*f / sqrt(2.*PI);
}
void main() {
	color.xyz = fColor * (fDist > 0.0001 ? intensity(fPosition, fDist, fSigma) : pointIntensity(fPosition, fSigma));
	color.w = 1.0;
}