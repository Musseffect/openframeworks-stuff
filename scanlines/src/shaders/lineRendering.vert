#version 330

#define ROT(a) mat2(cos(a),-sin(a),sin(a),cos(a))

layout(location = 0)in vec2 rectPosition;
layout(location = 1)in vec4 center;//center angle distance
layout(location = 2)in vec4 colorA_sigma;//colorA_sigma
layout(location = 3)in vec3 colorB;
uniform mat3 transform;

out vec3 fColor;
out float fSigma;
out float fDist;
out vec2 fPosition;

void main() {
	fColor = mix(colorA_sigma.xyz, colorB.xyz, rectPosition.x * 0.5 + 0.5);
	fSigma = colorA_sigma.w;
	fDist = center.w;
	//local coordinates
	fPosition = rectPosition * vec2(fSigma*4. + fDist * 0.5, 4. * fSigma) + vec2(fDist * 0.5, 0.0);
	//local to global to ndc coordinates
	gl_Position = vec4(transform * vec3(ROT(-center.z) * fPosition + center.xy, 1.), 1.);
}