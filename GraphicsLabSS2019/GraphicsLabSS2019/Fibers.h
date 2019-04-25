#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glad/glad.h"
#include "Camera.h"
#include "Shader.h"

class CFibers
{
public:
	CFibers();
	~CFibers();

	void LoadFile(const char* pFile);
	void DrawLines(CCamera* pCamera, CShader* pShader);
	void DrawTubes(CCamera* pCamera, CShader* pShader, float fRadius);
	void ChangeTubeEdges(unsigned int nCountEdges);

private:
	void GenerateBuffers();
	void GenerateAndFillTubeBuffer();
	void GenerateTube();
	struct SVertex
	{
		glm::vec3 Position;
	};

	struct SVertexTube
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
	};
	
	struct STubeInfo
	{
		glm::mat4 matModelMatrix;
		float fLength;
		float fFill0;
		float fFill1;
		float fFill2;
	};

	std::vector<SVertex> m_vecVBOData;
	std::vector<GLuint> m_vecIBOData;
	std::vector<SVertexTube> m_vecVBOTube;
	std::vector<GLuint> m_vecIBOTube;
	GLuint m_nVAOLines;
	GLuint m_nVBOLines;
	GLuint m_nIBOLines;
	GLuint m_nSSBOTubeID;
	std::vector<STubeInfo> m_vecTubes;
	GLuint m_nVAOTube;
	GLuint m_nVBOTube;
	GLuint m_nIBOTube;
	bool m_bTubeIsGenerated = false;
	unsigned int m_nCountFibers;
	unsigned int m_nCountTubeEdges;
};

