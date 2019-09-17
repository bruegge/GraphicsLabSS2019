#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform int nEnableExport;
uniform int nCountTubes;
uniform int nCountTubeEdges;

in VS_OUT 
{
	flat uint nVertexID;
	flat uint nInstanceID;
	float fStatus;
	vec3 color;
} gs_in[];

out GS_OUT 
{
	flat uint nInstanceID;
	float depth;
	float fStatus;
	flat uint nVertexID;
	vec3 color;
} gs_out;

void CopyInputToOutput(int index)
{
	gs_out.nInstanceID = gs_in[index].nInstanceID; 
	gs_out.depth = gl_in[index].gl_Position.z;
	gs_out.fStatus = gs_in[index].fStatus;
	gs_out.nVertexID = gs_in[index].nVertexID;
	gs_out.color = gs_in[index].color;
}

void main() 
{    
    CopyInputToOutput(0);
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

    CopyInputToOutput(1);
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
    
	CopyInputToOutput(2);
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
    
    EndPrimitive();
}  