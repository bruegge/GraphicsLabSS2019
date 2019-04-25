#version 430

out vec4 ColorOut;

in VS_OUT 
{
	vec3 position;
	vec3 normal;
} fs_in;

void main()
{   
	ColorOut = vec4(fs_in.normal,1);
}