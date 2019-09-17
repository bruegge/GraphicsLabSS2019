#version 430 core

layout(location = 0) in vec3 Position;

out VS_OUT 
{
	vec2 texCoordinate;
} vs_out;

void main()
{		
	vs_out.texCoordinate = Position.xy * 0.5f + vec2(0.5f,0.5f);
	gl_Position = vec4(Position,1); 
}