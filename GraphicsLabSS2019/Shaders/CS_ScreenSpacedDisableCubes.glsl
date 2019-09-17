#version 430 core
layout(local_size_x = 1, local_size_y = 1) in;

uniform sampler2D sTexture;

struct SCube
{
	vec4 CubeEnabled;
};

layout(std430, binding = 0) buffer CubeSSBO
{
	SCube cubes[];
} cubes;

void main() 
{    
	vec4 color = texture(sTexture, vec2(float(gl_GlobalInvocationID.x)/2048.0f,float(gl_GlobalInvocationID.y)/2048.0f));

	int ssboIndex = int(color.x * 256 * 256 * 255) + int(color.y * 256 * 255 + color.z * 255) - 2;

	if(ssboIndex >= 0)
	{
		cubes.cubes[ssboIndex].CubeEnabled.a = 0;

		cubes.cubes[0].CubeEnabled = ivec4(1,1,1,1);
	}	
}  