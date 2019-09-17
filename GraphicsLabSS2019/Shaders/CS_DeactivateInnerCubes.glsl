#version 430 core
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct SCube
{
	vec4 CubeEnabled;
};

layout(std430, binding = 0) buffer CubeSSBO
{
	SCube cubes[];
} cubes;

uint GetCubeIndex(ivec3 Position)
{
	return 130 * 130 * Position.x + 130 * Position.y + Position.z;
}

bool GetStatus(uvec3 invocationID)
{
	float status = cubes.cubes[GetCubeIndex(ivec3(invocationID))].CubeEnabled.a;
	if(status > 0)
	{
		return true;
	}
	return false;
}

void main() 
{    
	uint invocationID = GetCubeIndex(ivec3(gl_GlobalInvocationID.xyz));
	vec4 pos = cubes.cubes[invocationID].CubeEnabled;
	
	bool isInside = false;
	
	uint isBottomOpen = 0;	//2^2  4
	uint isFrontOpen = 0;	//2^3  8
	uint isLeftOpen = 0;	//2^4  16
	uint isBackOpen = 0;	//2^5  32
	uint isRightOpen = 0;	//2^6  64
	uint isTopOpen = 0;		//2^7  128

	uint mask = 0;
	if(gl_GlobalInvocationID.x > 0)
	{
		if(GetStatus(gl_GlobalInvocationID.xyz - uvec3(1,0,0)) == false)
		{
			isLeftOpen = 16;
			isInside = false;
		}
	}
	else
	{
		isLeftOpen = 16;
		isInside = false;
	}
	if(gl_GlobalInvocationID.x < 119)
	{
		if(GetStatus(gl_GlobalInvocationID.xyz + uvec3(1,0,0)) == false)
		{
			isRightOpen = 64;
			isInside = false;
		}
	}
	else
	{
		isRightOpen = 64;
		isInside = false;
	}
	if(gl_GlobalInvocationID.y > 0)
	{
		if(GetStatus(gl_GlobalInvocationID.xyz - uvec3(0,1,0)) == false)
		{
			isBottomOpen = 4;
			isInside = false;
		}
	}
	else
	{
		isBottomOpen = 4;
		isInside = false;
	}
	if(gl_GlobalInvocationID.y < 129)
	{
		if(GetStatus(gl_GlobalInvocationID.xyz + uvec3(0,1,0)) == false)
		{
			isTopOpen = 128;
			isInside = false;
		}
	}
	else
	{
		isTopOpen = 128;
		isInside = false;
	}
	if(gl_GlobalInvocationID.z > 0)
	{
		if(GetStatus(gl_GlobalInvocationID.xyz - uvec3(0,0,1)) == false)
		{
			isFrontOpen = 8;
			isInside = false;
		}
	}
	else
	{
		isFrontOpen = 8;
		isInside = false;
	}
	if(gl_GlobalInvocationID.z < 129)
	{
		if(GetStatus(gl_GlobalInvocationID.xyz + uvec3(0,0,1)) == false)
		{
			isBackOpen = 32;
			isInside = false;
		}
	}
	else
	{
		isBackOpen = 32;
		isInside = false;
	}

	mask = isBackOpen + isFrontOpen + isTopOpen + isBottomOpen + isLeftOpen + isRightOpen + 1;
	if(isInside == true)
	{	
		mask = 2;
	}
	if(GetStatus(gl_GlobalInvocationID.xyz) == true)
	{
		cubes.cubes[invocationID].CubeEnabled.a = float(mask);		
	}
	
	

}  