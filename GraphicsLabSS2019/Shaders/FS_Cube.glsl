#version 430 core

layout(location = 0) out vec3 ColorOut;
layout(location = 1) out vec3 Cube;

in GS_OUT 
{
	flat uint nInstanceID;
	float depth;
	float fStatus;
	flat uint nVertexID;
	vec3 color;
} fs_in;

void main()
{   
	if(fs_in.fStatus == 1)
	{
		ColorOut = vec3(fs_in.depth / 100.0f, fs_in.depth / 100.0f, fs_in.depth / 100.0f);
	}
	else
	{
		ColorOut = vec3(0, fs_in.depth / 100.0f, 0.5f);
		
	}
	//ColorOut = fs_in.color;

	uint nCubeNumber = fs_in.nInstanceID + 2;
	vec3 cubeNumber;
	cubeNumber.x = ((nCubeNumber >> 16) & 0xff) / 255.0f;
	cubeNumber.y = ((nCubeNumber >> 8) & 0xff) / 255.0f;
	cubeNumber.z = (nCubeNumber & 0xff) / 255.0f;
	Cube = cubeNumber;
}