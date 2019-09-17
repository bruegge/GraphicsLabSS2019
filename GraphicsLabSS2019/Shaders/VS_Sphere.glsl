#version 430 core

layout(location = 0) in vec3 Position;

uniform mat4 viewProjectionMatrix;
uniform float fRadius;
uniform vec3 vPosition;

void main()
{		
	gl_Position = viewProjectionMatrix * vec4(Position * fRadius + vPosition,1); 
}