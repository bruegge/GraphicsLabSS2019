#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glad/glad.h"
#include "Camera.h"
#include "Exporter.h"
#include "Shader.h"
#include "FrameBuffer.h"

class CFibers
{
public:
	CFibers();
	~CFibers();

	void LoadFile(const char* pFile);
	void DrawLines(CCamera* pCamera, CShader* pShader);
	void DrawPoints(CCamera* pCamera, CShader* pShader);
	void DrawTubes(CCamera* pCamera, CShader* pShader, CFrameBuffer* pFrameBuffer, float fRadius, unsigned int nRenderMode, unsigned int nVisibleTubeMode);
	void ChangeTubeEdges(unsigned int nCountEdges);
	unsigned int GetFiberCount();
	void EnableSingleFiber(bool bEnable, unsigned int nFiberNumber);
	bool IsSingleFiberEnabled();
	void EnableTubePlanes(bool bStartPlane, bool bEndPlane);
	void Export();
	void DisableHiddenFibers(CShader* pShader, float fRadius);
	void BringEndingsTogether();
	void SetCuttingPlaneVectors(std::vector<bool>& vecCuttingPlaneEnabled, std::vector<glm::vec4>& vecCuttingPlaneVectors);
	void ToggleIgnoreCuttingPlaneFiber(int nFiberNumber);
	void IgnoreCuttingPlaneForSphere(glm::vec3 vPosition, float radius);

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
		glm::vec3 PlaneStartNormal;
		float fLength;
		glm::vec3 PlaneStartPosition;
		int nVisibleTube;
		glm::vec3 PlaneEndNormal;
		float fEnableStartPlane;
		glm::vec3 PlaneEndPosition;
		float fEnableEndPlane;
		float fFiberNumber;
		float fFill0;
		float fFill1;
		float fFill2;
	};

	struct SFiberStartEnd
	{
		unsigned int nStartSSBO;
		unsigned int nEndSSBO;
		unsigned int nStartIBO;
		unsigned int nEndIBO;
	};

	std::vector<SVertex> m_vecVBOData;
	std::vector<GLuint> m_vecIBOData;
	std::vector<SVertexTube> m_vecVBOTube;
	std::vector<GLuint> m_vecIBOTube;
	std::vector<SFiberStartEnd> m_vecFiberStartEndIndex;
	GLuint m_nVAOLines;
	GLuint m_nVBOLines;
	GLuint m_nIBOLines;
	GLuint m_nSSBOTubeID;
	GLuint m_nSSBOFibersIgnoreCuttingPlane;
	std::vector<STubeInfo> m_vecTubes;
	GLuint m_nVAOTube;
	GLuint m_nVBOTube;
	GLuint m_nIBOTube;
	bool m_bShowSingleFiber = false;
	unsigned int m_nShowSingleFiber = 0;
	bool m_bTubeIsGenerated = false;
	bool m_bEnableExport = false;
	GLuint m_nVBOExport = 0;
	GLuint m_nIBOExport = 0;
	unsigned int m_nCountFibers;
	unsigned int m_nCountTubeEdges;
	CExporter* m_pExporter;
	unsigned int m_nEnablePlaneStart;
	unsigned int m_nEnablePlaneEnd;
	glm::vec3 m_vMinVertex;
	glm::vec3 m_vMaxVertex;
	CShader* m_pShaderScreenSpacedEnableVisibleTubes;
	CFrameBuffer* m_pFrameBufferRenderVisibleTubes;
	std::vector<int> m_vecCuttingPlaneEnabled;
	std::vector<glm::vec4> m_vecCuttingPlaneVectors;
};

