#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform int nEnableExport;
uniform float fCountTubeEdges;

in VS_OUT 
{
	vec3 position;
	vec3 normal;
	vec3 color;
	uint nVertexID;
	uint nInstanceID;
	vec3 vTubeDirection;
} gs_in[];

out GS_OUT 
{
	vec3 position;
	vec3 normal;
	flat uint nInstanceID;
	vec3 color;
	vec3 vTubeDirection;
} gs_out;

layout(std430, binding = 2) buffer ExportIndicesSSBO
{
	uint index[];
} indexSSBO;

void CopyInputToOutput(int index)
{
    gs_out.position = gs_in[index].position; 
	gs_out.normal = gs_in[index].normal; 
	gs_out.nInstanceID = gs_in[index].nInstanceID; 
	gs_out.color = gs_in[index].color; 
	gs_out.vTubeDirection = gs_in[index].vTubeDirection;
}

void ExportTriangleToSSBO()
{
	uint index0ID = gs_in[0].nInstanceID * uint(fCountTubeEdges) * 6 + gl_PrimitiveIDIn * 3;
	indexSSBO.index[index0ID] = gs_in[0].nVertexID;
	indexSSBO.index[index0ID + 1] = gs_in[1].nVertexID;
	indexSSBO.index[index0ID + 2] = gs_in[2].nVertexID;	
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
	if(nEnableExport == 1)
	{
		ExportTriangleToSSBO();
	}
}  