#version 430 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec3 Tangent;
layout(location = 3) in vec3 Bitangent;

uniform mat4 viewProjectionMatrix;
uniform float fRadius;
uniform float fCountTubeEdges;

struct STubeInfo
{
	mat4 matModelMatrix;
	vec3 PlaneStartNormal;
	float fLength;
	vec3 PlaneStartPosition;
	float fFiberNumber;
	vec3 PlaneEndNormal;
	float fFill0;
	vec3 PlaneEndPosition;
	float fFill1;
};

layout(std430, binding = 0) buffer TubeSSBO
{
	STubeInfo fibers[175317];
} fibers;

out VS_OUT 
{
	vec3 position;
	vec3 normal;
	float fiberNumber;
} vs_out;

vec3 IntersectionPlaneLine(vec3 lineDirection, vec3 linePosition, vec3 planePosition, vec3 planeNormal)
{
	if(dot(planeNormal, normalize(lineDirection)) == 0)
	{
		return vec3(0,0,0);
	}
	float t = (dot(planeNormal, planePosition) - dot(planeNormal,linePosition)) / dot(planeNormal, normalize(lineDirection));
	return linePosition + normalize(lineDirection) * t;
}

void main()
{		
	vec4 pos = vec4(Position,1);
	
	pos.x *= fRadius;
	pos.y *= fRadius;
	pos.z *= fibers.fibers[gl_InstanceID].fLength;
	vec3 vPlaneNormal = gl_VertexID < fCountTubeEdges ? fibers.fibers[gl_InstanceID].PlaneStartNormal : fibers.fibers[gl_InstanceID].PlaneEndNormal;
	vec3 vPlanePosition = gl_VertexID < fCountTubeEdges ? fibers.fibers[gl_InstanceID].PlaneStartPosition : fibers.fibers[gl_InstanceID].PlaneEndPosition;
	
	pos = fibers.fibers[gl_InstanceID].matModelMatrix * pos; 
	vec3 vLineDirection = normalize(pos - fibers.fibers[gl_InstanceID].matModelMatrix * vec4(Position.xy * fRadius,0,1)).xyz;
	pos = vec4(IntersectionPlaneLine(vLineDirection, pos.xyz, vPlanePosition, vPlaneNormal),1);
	
	gl_Position = viewProjectionMatrix * pos;
	vs_out.normal = normalize(transpose(inverse(mat3(fibers.fibers[gl_InstanceID].matModelMatrix))) * Normal);
	vs_out.position = gl_Position.xyz;
	vs_out.fiberNumber = fibers.fibers[gl_InstanceID].fFiberNumber;
}