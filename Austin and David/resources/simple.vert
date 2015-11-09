#version 330

//TODO: receive a uniform value
uniform float timeVal;
uniform float myConstant;

in vec2 pos;
in vec3 color;
in float vertexSize;


out vec4 smoothColor;

void main()
{
//	gl_Position = vec4(pos, 0, 1);
//	smoothColor = vec4(color, 1);
    
    
    
	//TODO: make an animation!
	//could use the frag shader also
    
    smoothColor = vec4(color.r * (0.5 * sin(timeVal) + 0.5),
                       color.g * (0.5 * sin(timeVal*2) + 0.5),
                       color.b * (0.5 * sin(timeVal*3.1415926) + 0.5), 1);

    
    gl_Position = vec4(pos * (0.5 * sin(timeVal*2) + myConstant/1000),0 ,1);
    
    
    
//    gl_Position = vec4(pos * (0.5 * sin(constant/1000) + 0.2),0 ,1);
//    gl_Position = vec4(pos + vertexSize,0 ,1);

}
