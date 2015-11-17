#version 330

uniform vec4 Ka;
uniform vec4 Kd;
uniform vec4 Ks;
uniform vec4 specAlpha;

uniform mat4 P;
uniform mat4 Lp;
uniform mat4 mT;
uniform mat4 mR;
uniform mat4 M;
uniform mat4 Lv;
uniform mat4 Lr;

uniform mat4 lightPos;
uniform mat4 velocity;

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
    
    return vec4(pos, 1);
}

void main()
{
    smoothPos = pos;
    smoothNorm = colorIn * 2 - 1;
    smoothColor = vec4(colorIn, 1);
    shadowPos = positionInLightView(pos);
    
    mat4 temp =
    mat4(
         1.0, 0.0, 0.0, 0.5,
         0.0, 1.0, 0.0, 0.5,
         0.0, 0.0, 1.0, 0.5,
         0.0, 0.0, 0.0, 1.0);
    
    vec4 p = vec4(pos, 1);
    gl_Position = temp*P*M*p;
}