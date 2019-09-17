#version 430

in VS_OUT 
{
	vec2 texCoordinate;
} fs_in;

out vec4 ColorOut;
uniform sampler2D Tex0;
uniform sampler2D Tex1;

void main()
{   
	ColorOut = texture(Tex0, fs_in.texCoordinate);
	//ColorOut = vec4(fs_in.texCoordinate,0,1);
}