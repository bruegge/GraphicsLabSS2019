#version 430 core

uniform mat4 viewProjectionMatrix;

layout(std430, binding = 1) buffer ExportVertexSSBO
{
	vec4 vertexPosition[];
} vertexSSBO;

void main()
{		
	gl_Position = viewProjectionMatrix * vertexSSBO.vertexPosition[gl_VertexID]; 
}