#version 430

out vec4 ColorOut;

in VS_OUT 
{
	vec3 position;
	vec3 normal;
	float fiberNumber;
} fs_in;

void main()
{   
	ColorOut = vec4(fs_in.fiberNumber / 2000.0f,fs_in.fiberNumber / 2000.0f,fs_in.fiberNumber / 2000.0f,1);
}