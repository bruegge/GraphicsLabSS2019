#version 430 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec3 Tangent;
layout(location = 3) in vec3 Bitangent;

uniform mat4 viewProjectionMatrix;
uniform float fRadius;
uniform float fCountTubeEdges;
uniform int nEnableExport;
uniform int nOffset;
uniform int nVisibleTubeMode;

uniform int nEnableCuttingPlane[10];
uniform vec4 vCuttingPlane[10];
uniform int nCountCuttingPlanes;
uniform int bAndLinkage;

struct STubeInfo
{
	mat4 matModelMatrix;
	vec3 PlaneStartNormal;
	float fLength;
	vec3 PlaneStartPosition;
	int nVisibleTube;
	vec3 PlaneEndNormal;
	float fEnableStartPlane;
	vec3 PlaneEndPosition;
	float fEnableEndPlane;
	float fFiberNumber;
	float fFill0;
	float fFill1;
	float fFill2;
};

layout(std430, binding = 0) buffer TubeSSBO
{
	STubeInfo fibers[];
} fibers;

out VS_OUT 
{
	vec3 position;
	vec3 normal;
	vec3 color;
	uint nVertexID;
	uint nInstanceID;
	vec3 vTubeDirection;
} vs_out;

layout(std430, binding = 1) buffer ExportVertexSSBO
{
	vec4 vertexPosition[];
} vertexSSBO;

struct SCPIgnorInfo
{	
	float nEnabled;
};


layout(std430, binding = 5) buffer IgnoreCuttingPlanesFibers
{
	SCPIgnorInfo Fiber[];
} IgnoreCPFibers;



void ExportVertexToSSBO(vec3 position)
{
	uint instance = gl_InstanceID;
	vs_out.nVertexID = uint(instance * fCountTubeEdges * 2 + gl_VertexID);
	
	int visible = fibers.fibers[instance].nVisibleTube & 0x00000001;

	if(nVisibleTubeMode == 0 &&  visible == 1)
	{
		position = vec3(0,0,0);
	}
	if(nVisibleTubeMode == 1 && visible == 0)
	{
		position = vec3(0,0,0);
	}
	vertexSSBO.vertexPosition[vs_out.nVertexID] = vec4(position, 1);

}

vec3 IntersectionPlaneLine(vec3 lineDirection, vec3 linePosition, vec3 planePosition, vec3 planeNormal)
{
	if(dot(planeNormal, normalize(lineDirection)) == 0)
	{
		return vec3(0,0,0);
	}
	float t = (dot(planeNormal, planePosition) - dot(planeNormal,linePosition)) / dot(planeNormal, normalize(lineDirection));
	return linePosition + normalize(lineDirection) * t;
}

bool CheckCuttingPlanes(vec3 midpointPosition)
{
	bool result = false;
	bool oneActive = false;
	for(int i = 0; i< nCountCuttingPlanes; ++i)
	{
		if(nEnableCuttingPlane[i] > 0)
		{
			oneActive = true;
			vec3 translatedMidPoint = midpointPosition  - (normalize(vCuttingPlane[i].xyz) * vCuttingPlane[i].a);
			float direction = dot(normalize(vCuttingPlane[i].xyz), translatedMidPoint);
	
			if(direction > 0)
			{
				result = true;
			}
			if(bAndLinkage == 1 && direction <0)
			{
				return false;
			}
		}
	}
	if(oneActive == true)
	{
		return result;	
	}
	return true;
}

void main()
{		
	vec4 pos = vec4(Position,1);
	pos.x *= fRadius;
	pos.y *= fRadius;
	uint instance = gl_InstanceID + nOffset;
	pos.z *= fibers.fibers[instance].fLength;
	
	vec3 vPlaneNormal = gl_VertexID < fCountTubeEdges ? fibers.fibers[instance].PlaneStartNormal : fibers.fibers[instance].PlaneEndNormal;
	vec3 vPlanePosition = gl_VertexID < fCountTubeEdges ? fibers.fibers[instance].PlaneStartPosition : fibers.fibers[instance].PlaneEndPosition;
	float fFibernumber = fibers.fibers[instance].fFiberNumber;
	float nEnablePlane = 1;
	nEnablePlane = gl_VertexID < fCountTubeEdges ? fibers.fibers[instance].fEnableStartPlane : fibers.fibers[instance].fEnableEndPlane;

	if(nEnablePlane == 0)
	{
		pos.xy = vec2(0,0);
	}
	pos = fibers.fibers[instance].matModelMatrix * pos; 
	vec3 vLineDirection = normalize(pos - fibers.fibers[instance].matModelMatrix * vec4(Position.xy * fRadius,0,1)).xyz;
	if(nEnablePlane == 1.0f)
	{
		pos = vec4(IntersectionPlaneLine(vLineDirection, pos.xyz, vPlanePosition, vPlaneNormal),1);
	}
	
	
	gl_Position = viewProjectionMatrix * pos;
	vs_out.normal = normalize(transpose(inverse(mat3(fibers.fibers[instance].matModelMatrix))) * Normal);
	vs_out.position = gl_Position.xyz;
	vs_out.color = gl_VertexID < fCountTubeEdges ? vec3(1,0,0) : vec3(0,1,0);
	vs_out.nInstanceID = gl_InstanceID;
	vs_out.vTubeDirection = normalize((fibers.fibers[instance].matModelMatrix * vec4(0,0,0,1) - fibers.fibers[instance].matModelMatrix * vec4(0,0,1,1)).xyz);
	
	int visible = fibers.fibers[instance].nVisibleTube & 0x00000001;
	if(nVisibleTubeMode == 0 && visible == 1)
	{
		gl_Position = vec4(0,0,0,0);
	}
	if(nVisibleTubeMode == 1 && visible == 0)
	{
		gl_Position = vec4(0,0,0,0);
	}
	if(CheckCuttingPlanes(fibers.fibers[instance].matModelMatrix[3].xyz) == false && IgnoreCPFibers.Fiber[int(fFibernumber)].nEnabled == 0)
	{
		gl_Position = vec4(0,0,0,0);
		fibers.fibers[instance].nVisibleTube = fibers.fibers[instance].nVisibleTube & 0xfffffffd;
	}
	else
	{
		fibers.fibers[instance].nVisibleTube = fibers.fibers[instance].nVisibleTube | 0x00000002;
	}
	if(nEnableExport == 1)
	{
		if(gl_Position == vec4(0,0,0,0))
		{
			ExportVertexToSSBO(vec3(0,0,0));
		}
		else
		{
			ExportVertexToSSBO(pos.xyz);
		}
		
	}
	
}