#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "FrameBuffer.h"

GLuint CreateRenderTexture(unsigned int nWidth, unsigned int nHeight)
{
	GLuint nColorTexture;
	glGenTextures(1, &nColorTexture);

	glBindTexture(GL_TEXTURE_2D, nColorTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	return nColorTexture;
}

CFrameBuffer::CFrameBuffer(unsigned int nWidth, unsigned int nHeight, CShader* pShader)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_pShader = pShader;

	glGenFramebuffers(1, &m_nFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_nFBO);

	m_nTextureColor0 = CreateRenderTexture(nWidth, nHeight);
	m_nTextureColor1 = CreateRenderTexture(nWidth, nHeight);

	glGenRenderbuffers(1, &m_nTextureDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_nTextureDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, nWidth, nHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_nTextureDepth);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_nTextureColor0, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_nTextureColor1, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, DrawBuffers);

	//create VBO
	glGenVertexArrays(1, &m_nVAO);
	glBindVertexArray(m_nVAO);

	glGenBuffers(1, &m_nVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
	std::vector<glm::vec3> vecVBOData;
	vecVBOData.push_back(glm::vec3(-1, -1, 0));
	vecVBOData.push_back(glm::vec3(-1, 1, 0));
	vecVBOData.push_back(glm::vec3(1, -1, 0));

	vecVBOData.push_back(glm::vec3(-1, 1, 0));
	vecVBOData.push_back(glm::vec3(1, 1, 0));
	vecVBOData.push_back(glm::vec3(1, -1, 0));

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vecVBOData.size(), vecVBOData.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &m_nFBOPick);
	glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOPick);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_nTextureColor1, 0);
	glDrawBuffers(1, DrawBuffers);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_pShader = new CShader();
	m_pShader->CreateShaderProgram("../Shaders/VS_FrameBuffer.glsl", nullptr, nullptr, nullptr, "../Shaders/FS_FrameBuffer.glsl");
}


CFrameBuffer::~CFrameBuffer()
{
}

void CFrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_nFBO);
	glViewport(0, 0, m_nWidth, m_nHeight);
}

void CFrameBuffer::UnBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CFrameBuffer::DrawToScreen()
{
	glBindVertexArray(m_nVAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	m_pShader->Bind();
	
	GLuint unit = 0;

	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_nTextureColor0);
	glUniform1i(glGetUniformLocation(m_pShader->GetID(), "Tex0"), unit);

	unit = 1;

	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_nTextureColor1);
	glUniform1i(glGetUniformLocation(m_pShader->GetID(), "Tex1"), unit);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	m_pShader->UnBind();
	glBindVertexArray(0);
}

void CFrameBuffer::Clear()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_nFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool CFrameBuffer::GetFiberNumber(unsigned int nX, unsigned int nY, unsigned int& rFiberNumber)
{
	GLint x = nX;
	GLint y = nY;
	unsigned int nData = 0;
	glm::u8vec3 vec;
	glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOPick);

	glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &vec);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	nData = vec.x * 256 * 256 + vec.y * 256 + vec.z;

	if (nData == 0)
	{
		return false;
	}
	rFiberNumber = nData - 1;
	return true;
}

GLuint CFrameBuffer::GetColorPickingTexture()
{
	return m_nTextureColor1;
}