#pragma once
#include "glad/glad.h"
#include "Shader.h"

class CFrameBuffer
{
public:
	CFrameBuffer(unsigned int nWidth, unsigned int nHeight, CShader* pShader);
	~CFrameBuffer();
	void Bind();
	void UnBind();
	void DrawToScreen();
	void Clear();
	bool GetFiberNumber(unsigned int nX, unsigned int nY, unsigned int& rFiberNumber);
	GLuint GetColorPickingTexture();

private:
	GLuint m_nFBO;
	GLuint m_nFBOPick;
	GLuint m_nVBO;
	GLuint m_nVAO;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	CShader* m_pShader;

	GLuint m_nTextureColor0;
	GLuint m_nTextureColor1;
	GLuint m_nTextureDepth;
};

