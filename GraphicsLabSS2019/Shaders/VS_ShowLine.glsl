#version 430 core

layout(location = 0) in vec3 Position;

uniform mat4 viewProjectionMatrix;
uniform int nStart;
uniform int nEnd;

void main()
{		
	gl_Position = viewProjectionMatrix * vec4(Position,1); 

	if(gl_VertexID < nStart || gl_VertexID > nEnd)
	{
		gl_Position = vec4(0,0,0,0);
	}
}