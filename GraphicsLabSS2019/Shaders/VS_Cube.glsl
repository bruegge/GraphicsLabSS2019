#version 430 core

layout(location = 0) in vec3 Position;

uniform mat4 viewProjectionMatrix;
uniform int nEnableExport;

uniform int nEnableCuttingPlane[10];
uniform vec4 vCuttingPlane[10];
uniform int nCountCuttingPlanes;
uniform int bAndLinkage;
uniform int bDefineAsVisible;

struct SCube
{
	vec4 CubeEnabled;
};

layout(std430, binding = 0) buffer CubeSSBO
{
	SCube cubes[];
} cubes;

out VS_OUT 
{
	flat uint nVertexID;
	flat uint nInstanceID;
	float fStatus;
	vec3 color;
} vs_out;

layout(std430, binding = 1) buffer ExportVertexSSBO
{
	vec4 vertexPosition[];
} vertexSSBO;

void ExportVertexToSSBO(vec3 position)
{
	uint instance = gl_InstanceID;
	
	//if(cubes.cubes[instance].CubeEnabled.a != 1)
	{
		//vertexSSBO.vertexPosition[vs_out.nVertexID] = vec4(0,0,0, 0);
	}
	//else
	{
		vertexSSBO.vertexPosition[vs_out.nVertexID] = vec4(position, 1);
	}
	
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

	uint instance = gl_InstanceID;
	if(instance != 0)
	{
		vs_out.nVertexID = uint(36 * instance + gl_VertexID);
		vs_out.color = Position * 2;
		if(CheckCuttingPlanes(cubes.cubes[instance].CubeEnabled.xyz) == false)
		{
			gl_Position = vec4(0,0,0,0);
			float cubeStatus = cubes.cubes[instance].CubeEnabled.a;
			if(bDefineAsVisible == 1)
			{
				cubes.cubes[instance].CubeEnabled.a = 0;
			}
		}
		else
		{
			uint cubeStatus = uint(cubes.cubes[instance].CubeEnabled.a);
			uint nIsActive = cubeStatus & 1;

		
			if(nIsActive == 1)
			{
				gl_Position = viewProjectionMatrix * vec4(cubes.cubes[instance].CubeEnabled.xyz + Position,1); 
				vs_out.nInstanceID = instance;
				vs_out.fStatus = cubes.cubes[instance].CubeEnabled.a;
			}
			else
			{
				gl_Position = vec4(0,0,0,0);
			}

			if(nEnableExport == 1)
			{
				uint nBottom = cubeStatus & 0x00000004;
				uint nFront = cubeStatus  & 0x00000008;
				uint nLeft = cubeStatus   & 0x00000010;
				uint nBack = cubeStatus   & 0x00000020;
				uint nRight = cubeStatus  & 0x00000040;
				uint nTop = cubeStatus    & 0x00000080;
		
				uint face = gl_VertexID / 6;
			
				bool save = false;
				if(face == 0 && nBottom > 0){ save = true;}
				if(face == 1 && nFront > 0){ save = true;}
				if(face == 2 && nLeft > 0){ save = true;}
				if(face == 3 && nBack > 0){ save = true;}
				if(face == 4 && nRight > 0){ save = true;}
				if(face == 5 && nTop > 0){ save = true;}
			
				if(save == true)
				{
					ExportVertexToSSBO(cubes.cubes[instance].CubeEnabled.xyz + Position);
				}
			}
		}
	}
}