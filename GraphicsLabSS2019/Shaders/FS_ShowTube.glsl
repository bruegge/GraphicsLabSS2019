#version 430

layout(location = 0) out vec3 ColorOut;
layout(location = 1) out vec3 Tube;

in GS_OUT 
{
	vec3 position;
	vec3 normal;
	flat uint nInstanceID;
	vec3 color;
	vec3 vTubeDirection;
} fs_in;

uniform int nRenderMode;
uniform int nCountFibers;

void main()
{   
	if(nRenderMode == 0)
	{
		ColorOut = normalize(fs_in.normal);
	}
	
	int nSquared = int(sqrt(nCountFibers));
	vec3 color = vec3(1,0,1);
	
	uint nTubeNumber = fs_in.nInstanceID + 1;
	color.x = ((nTubeNumber >> 16) & 0xff) / 255.0f;
	color.y = ((nTubeNumber >> 8) & 0xff) / 255.0f;
	color.z = (nTubeNumber & 0xff) / 255.0f;
	Tube = color;
	
	if(nRenderMode == 1)
	{
		ColorOut = color;
	}
	
	else if(nRenderMode == 2)
	{
		ColorOut = vec3(abs(fs_in.vTubeDirection.x),abs(fs_in.vTubeDirection.y),abs(fs_in.vTubeDirection.z));
	}
	else if(nRenderMode == 3)
	{
		ColorOut = vec3(fs_in.position.z,fs_in.position.z,fs_in.position.z) / 100.0f;
	}
}