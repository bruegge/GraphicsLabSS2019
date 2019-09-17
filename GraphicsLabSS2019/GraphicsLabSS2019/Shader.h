#pragma once
#include "glad/glad.h"
#include "glm/glm.hpp"

class CShader
{
public:
	CShader();
	~CShader();

	GLuint GetID(); //return the shader id to work with
	void CreateShaderProgram(const char* pVertex_file_path = nullptr,
		const char* pTesselationControl_file_path = nullptr,
		const char* pTesselationEvaluation_file_path = nullptr,
		const char* pGeometry_file_path = nullptr,
		const char* pFragment_file_path = nullptr);
	void CreateComputeShaderProgram(const char* pComputeShader_file_path);

	void Bind();
	void UnBind();

private:

	GLuint m_nShaderID;
	bool m_bComputeShaderEnabled;
};

