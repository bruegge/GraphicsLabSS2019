#version 430 core
layout(local_size_x = 1, local_size_y = 1) in;

uniform sampler2D sTexture;

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

void main() 
{    
	vec4 color = texture(sTexture, vec2(float(gl_GlobalInvocationID.x)/2048.0f,float(gl_GlobalInvocationID.y)/2048.0f));

	int ssboIndex = int(color.x * 256 * 256 * 255) + int(color.y * 256 * 255 + color.z * 255) - 1;
	if(ssboIndex >= 0)
	{
		fibers.fibers[ssboIndex].nVisibleTube = 1;
	}
	
}  