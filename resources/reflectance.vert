#version 330

uniform mat4 P;  //projection matrix
uniform mat4 Lp;  //light projection matrix
uniform mat4 mT;
uniform mat4 mR;
uniform mat4 M;  //modelview matrix: M = C * mR * mT
uniform mat4 Lv; //light view matrix
uniform mat4 Lr;  //light rotation matrix

uniform vec4 lightPos;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 colorIn;

out vec3 smoothPos;
out vec3 smoothNorm;
out vec4 smoothColor;
out vec4 shadowPos;

vec4 positionInLightView(vec3 pos)
{
	mat4 offset =
	mat4(
		 0.5, 0.0, 0.0, 0.0,
		 0.0, 0.5, 0.0, 0.0,
		 0.0, 0.0, 0.5, 0.0,
		 0.5, 0.5, 0.5, 1.0);
	
	//TODO: move vertex, put in light view
    vec4 vertPos = mR * mT * vec4(pos,1);
    vertPos = offset * Lp * Lv * vertPos;
	
	return vertPos;
}

void main()
{
	smoothPos = pos;
	smoothNorm = colorIn*2 - 1;
	smoothColor = vec4(colorIn, 1);
	shadowPos = positionInLightView(pos);
	
	vec4 p = vec4(pos, 1);
	gl_Position = P*M*p;
}
