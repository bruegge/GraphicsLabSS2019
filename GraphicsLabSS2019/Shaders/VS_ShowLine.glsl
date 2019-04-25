#version 430 core

layout(location = 0) in vec3 Position;

uniform mat4 viewProjectionMatrix;

void main()
{		
	gl_Position = viewProjectionMatrix * vec4(Position,1); 
}