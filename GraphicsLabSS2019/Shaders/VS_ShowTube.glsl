#version 430 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec3 Tangent;
layout(location = 3) in vec3 Bitangent;

uniform mat4 viewProjectionMatrix;
uniform float fRadius;
struct STubeInfo
{
	mat4 matModelMatrix;
	float fLength;
	float fFill0;
	float fFill1;
	float fFill2;
};

layout(std430, binding = 0) buffer TubeSSBO
{
	STubeInfo fibers[175317];
} fibers;

out VS_OUT 
{
	vec3 position;
	vec3 normal;
} vs_out;

void main()
{		
	vec4 pos = vec4(Position,1);
	pos.x *= fRadius;
	pos.y *= fRadius;
	pos.z *= fibers.fibers[gl_InstanceID].fLength;
	gl_Position = viewProjectionMatrix * fibers.fibers[gl_InstanceID].matModelMatrix * pos; 
	vs_out.normal = normalize(transpose(inverse(mat3(fibers.fibers[gl_InstanceID].matModelMatrix))) * Normal);
	vs_out.position = gl_Position.xyz;
}