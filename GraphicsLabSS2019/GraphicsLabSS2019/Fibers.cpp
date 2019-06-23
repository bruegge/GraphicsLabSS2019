#include "Fibers.h"
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include "glm\gtx\vector_angle.hpp"

std::vector<std::string> split(std::string str, char delimiter)
{
	std::vector<std::string> ret;

	size_t pos = str.find_first_of(delimiter);

	while (!str.empty())
	{
		std::string cur = str.substr(0, pos);
		if (!cur.empty())
			ret.push_back(cur);

		if (pos == std::string::npos)
			break;

		str = str.substr(pos + 1);

		pos = str.find_first_of(delimiter);
	}

	return ret;
}

void CFibers::LoadFile(const char* pFile)
{
	enum EFormat
	{
		ascii,
		binary_little_endian
	};
	EFormat eFormat = EFormat::ascii;
	std::string line;
	std::ifstream myfile(pFile);
	if (myfile.is_open())
	{
		unsigned int nCountvertices = 0;
		m_nCountFibers = 0;
		bool bInHeader = true;

		while (bInHeader && getline(myfile, line))
		{
			std::vector<std::string> lineSplit = split(line, ' ');
			if (lineSplit.size() > 0)
			{
				if (lineSplit[0] == "format" && lineSplit.size() == 3)
				{
					if (lineSplit[1] == "ascii")
					{
						eFormat = EFormat::ascii;
					}
					else if (lineSplit[1] == "binary_little_endian")
					{
						eFormat = EFormat::binary_little_endian;
					}
				}

				if (lineSplit[0] == "element" && lineSplit.size() == 3)
				{
					if (lineSplit[1] == "vertices")
					{
						nCountvertices = std::stoi(lineSplit[2]);
					}
					else if (lineSplit[1] == "fiber")
					{
						m_nCountFibers = std::stoi(lineSplit[2]);
					}
				}

				if (lineSplit[0] == "end_header")
				{
					bInHeader = false;
				}
			}			
		}

		m_vecVBOData.resize(nCountvertices);
		m_vecIBOData.resize(nCountvertices + 1);
		m_vecFiberStartEndIndex.resize(m_nCountFibers);

		m_vMinVertex = glm::vec3(10000, 10000, 10000);
		m_vMaxVertex = glm::vec3(-10000, -10000, -10000);

		if (eFormat == EFormat::ascii)
		{
			for (unsigned int i = 0; i < nCountvertices && getline(myfile, line); ++i)
			{
				std::vector<std::string> splitString = split(line, ' ');
				m_vecVBOData[i].Position.x = std::stof(splitString[0]);
				m_vecVBOData[i].Position.y = std::stof(splitString[1]);
				m_vecVBOData[i].Position.z = std::stof(splitString[2]);
				m_vMaxVertex.x = m_vMaxVertex.x > m_vecVBOData[i].Position.x ? m_vMaxVertex.x : m_vecVBOData[i].Position.x;
				m_vMaxVertex.y = m_vMaxVertex.y > m_vecVBOData[i].Position.y ? m_vMaxVertex.y : m_vecVBOData[i].Position.y;
				m_vMaxVertex.z = m_vMaxVertex.z > m_vecVBOData[i].Position.z ? m_vMaxVertex.z : m_vecVBOData[i].Position.z;

				m_vMinVertex.x = m_vMinVertex.x < m_vecVBOData[i].Position.x ? m_vMinVertex.x : m_vecVBOData[i].Position.x;
				m_vMinVertex.y = m_vMinVertex.y < m_vecVBOData[i].Position.y ? m_vMinVertex.y : m_vecVBOData[i].Position.y;
				m_vMinVertex.z = m_vMinVertex.z < m_vecVBOData[i].Position.z ? m_vMinVertex.z : m_vecVBOData[i].Position.z;
			}

			GLuint nFiberStart = 0;
			GLuint nIBOIndex = 0;
			for (unsigned int i = 0; i < m_nCountFibers && getline(myfile, line); ++i)
			{
				GLuint nFiberEnd = std::stoi(line);
				for (unsigned int j = nFiberStart; j < nFiberEnd; ++j)
				{
					m_vecIBOData[nIBOIndex++] = j;
				}
				m_vecIBOData[nIBOIndex++] = m_vecIBOData.size();
				nFiberStart = nFiberEnd + 1;
			}
		}

		if (eFormat == EFormat::binary_little_endian)
		{
			unsigned int nCurrentVertex = 0;
			unsigned int nCurrentVectorDimension = 0;
			unsigned char cValueFromFile[4];
			myfile >> cValueFromFile[0];
			myfile.close();
			myfile.open(pFile, std::ios_base::binary);
			std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(myfile), {});

			//find first position
			unsigned int nBufferVertexStartPosition = 0;
			char cString[11] = "";
			int nCompare = 1;
			std::string sCompare = "end_header";
			while (nCompare != 0)
			{
				nBufferVertexStartPosition++;
				memcpy(&cString, &buffer[nBufferVertexStartPosition], sizeof(char) * 10);
				cString[10] = '\0';
				std::string sString = static_cast<std::string>(cString);
				nCompare = sString.compare(sCompare);
			}
			nBufferVertexStartPosition += 11;
			//read VBO data
			unsigned int nBufferVertexSize = sizeof(glm::vec3) * nCountvertices;
			memcpy(&m_vecVBOData[0], &buffer[nBufferVertexStartPosition], nBufferVertexSize);
			
			for (unsigned int i = 0; i < nCountvertices; ++i)
			{
				m_vMaxVertex.x = m_vMaxVertex.x > m_vecVBOData[i].Position.x ? m_vMaxVertex.x : m_vecVBOData[i].Position.x;
				m_vMaxVertex.y = m_vMaxVertex.y > m_vecVBOData[i].Position.y ? m_vMaxVertex.y : m_vecVBOData[i].Position.y;
				m_vMaxVertex.z = m_vMaxVertex.z > m_vecVBOData[i].Position.z ? m_vMaxVertex.z : m_vecVBOData[i].Position.z;

				m_vMinVertex.x = m_vMinVertex.x < m_vecVBOData[i].Position.x ? m_vMinVertex.x : m_vecVBOData[i].Position.x;
				m_vMinVertex.y = m_vMinVertex.y < m_vecVBOData[i].Position.y ? m_vMinVertex.y : m_vecVBOData[i].Position.y;
				m_vMinVertex.z = m_vMinVertex.z < m_vecVBOData[i].Position.z ? m_vMinVertex.z : m_vecVBOData[i].Position.z;
			}

			//read IBO data
			unsigned int nBufferIndexPosition = nBufferVertexStartPosition + nBufferVertexSize;
			GLuint nFiberStart = 0;
			GLuint nIBOIndex = 0;

			for (unsigned int i = 0; i < m_nCountFibers; i++)
			{
				int nFiberEnd = 0;
				
				memcpy(&nFiberEnd, &buffer[nBufferIndexPosition], sizeof(int));
				nBufferIndexPosition += sizeof(unsigned int) * 2;
				for (unsigned int j = nFiberStart; j < nFiberEnd; ++j)
				{
					m_vecIBOData[nIBOIndex++] = j;
				}
				m_vecIBOData[nIBOIndex++] = m_vecIBOData.size();

				nFiberStart = nFiberEnd + 1;
			}
		}
		myfile.close();
	
		//storeDataToGPU
		GenerateBuffers();
		GenerateAndFillTubeBuffer();
		GenerateTube();
	}
	else std::cout << "Unable to open file";
}

void CFibers::GenerateBuffers()
{
	glGenVertexArrays(1, &m_nVAOLines);
	glGenBuffers(1, &m_nVBOLines);
	glGenBuffers(1, &m_nIBOLines);

	glBindVertexArray(m_nVAOLines);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, m_nVBOLines);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, m_vecVBOData.size() * sizeof(SVertex), &m_vecVBOData[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nIBOLines);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vecIBOData.size() * sizeof(GLuint), &m_vecIBOData[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)0);

	glBindVertexArray(0);


	glGenBuffers(1, &m_nVBOExport);
	glGenBuffers(1, &m_nIBOExport);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_nVBOExport);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 10, nullptr, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_nIBOExport);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 10, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_nVBOExport);

	glGenBuffers(1, &m_nSSBOFibersIgnoreCuttingPlane);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_nSSBOFibersIgnoreCuttingPlane);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_nCountFibers * sizeof(int), nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_nSSBOFibersIgnoreCuttingPlane);
	std::vector<float> vecIgnoreData(m_nCountFibers * 4);
	for (unsigned int i = 0; i < m_nCountFibers * 4; ++i)
	{
		vecIgnoreData[i] = 0;
	}
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float) * 4 * m_nCountFibers, &vecIgnoreData[0]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void CFibers::ToggleIgnoreCuttingPlaneFiber(int nFiberNumber)
{
	int nFiber = 0;
	bool bFoundFiberNumber = false;
	for (unsigned int i = 0; i < m_vecFiberStartEndIndex.size(); ++i)
	{
		if (m_vecFiberStartEndIndex[i].nStartSSBO <= nFiberNumber && m_vecFiberStartEndIndex[i].nEndSSBO >= nFiberNumber)
		{
			nFiber = i;
			bFoundFiberNumber = true;
		}
	}
	
	if (bFoundFiberNumber)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_nSSBOFibersIgnoreCuttingPlane);
		float data;
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, nFiber * sizeof(float), sizeof(float), &data);
		data = 1 - data;
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, nFiber * sizeof(float), sizeof(float), &data);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
	else
	{
		int error = 0;
		error++;
	}
}

void CFibers::EnableSingleFiber(bool bEnable, unsigned int nFiberNumber)
{
	m_bShowSingleFiber = bEnable;
	for (unsigned int i = 0; i < m_vecFiberStartEndIndex.size(); ++i)
	{
		if (nFiberNumber > m_vecFiberStartEndIndex[i].nStartSSBO && nFiberNumber < m_vecFiberStartEndIndex[i].nEndSSBO)
		{
			m_nShowSingleFiber = i;
		}
	}
	//todo: translation between tubeNumber(instance_ID and Fibernumber)!!!
}

bool CFibers::IsSingleFiberEnabled()
{
	return m_bShowSingleFiber;
}

void CFibers::ChangeTubeEdges(unsigned int nCountEdges)
{
	if (nCountEdges != m_nCountTubeEdges)
	{
		m_nCountTubeEdges = nCountEdges;
		GenerateTube();
	}
}

void CFibers::GenerateTube()
{
	m_vecVBOTube.clear();
	m_vecIBOTube.clear();
	//set VBO data
	for (int l = -1; l <= 1; l+=2)
	{
		for (unsigned int i = 0; i < m_nCountTubeEdges; ++i)
		{
			float fAngle = 3.1415926f * 2.0f / static_cast<float>(m_nCountTubeEdges) * i;
			SVertexTube sTube;
			sTube.Position = glm::vec3(cos(fAngle), sin(fAngle), l);
			sTube.Normal = glm::normalize(glm::vec3(cos(fAngle), sin(fAngle), 0));
			sTube.Bitangent = glm::vec3(0, 0, 1);
			sTube.Tangent = glm::normalize(glm::cross(sTube.Bitangent, sTube.Normal));
			m_vecVBOTube.push_back(sTube);
		}
	}

	//set IBO data
	for (unsigned int i = 0; i < m_nCountTubeEdges; ++i)
	{
		//triangle0
		m_vecIBOTube.push_back(i); //save
		m_vecIBOTube.push_back(i + m_nCountTubeEdges); //save
		m_vecIBOTube.push_back((i + 1)% m_nCountTubeEdges); //save
		//triangle1
		m_vecIBOTube.push_back(i + m_nCountTubeEdges); //save
		m_vecIBOTube.push_back((i + 1) % m_nCountTubeEdges + m_nCountTubeEdges); //save
		m_vecIBOTube.push_back((i + 1) % m_nCountTubeEdges); //save
	}


	if (m_bTubeIsGenerated)
	{
		glDeleteBuffers(1, &m_nVBOTube);
		glDeleteBuffers(1, &m_nIBOTube);
	}
	else
	{
		glGenVertexArrays(1, &m_nVAOTube);
	}
	glGenBuffers(1, &m_nVBOTube);
	glGenBuffers(1, &m_nIBOTube);

	glBindVertexArray(m_nVAOTube);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, m_nVBOTube);
	glBufferData(GL_ARRAY_BUFFER, m_vecVBOTube.size() * sizeof(SVertexTube), &m_vecVBOTube[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_nIBOTube);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vecIBOTube.size() * sizeof(GLuint), &m_vecIBOTube[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SVertexTube), (void*)0); //position
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SVertexTube), (void*)(sizeof(glm::vec3)));//normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(SVertexTube), (void*)(sizeof(glm::vec3) * 2));//normal
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SVertexTube), (void*)(sizeof(glm::vec3) * 3));//normal

	glBindVertexArray(0);
	m_bTubeIsGenerated = true;
}

void CFibers::GenerateAndFillTubeBuffer()
{
	//convert to STubeInfo format
	m_vecTubes.resize(m_vecVBOData.size() - m_nCountFibers * 2 + 1);
	
	int nIBOIndex = 0;
	unsigned int nSSBOIndex = 0;
	GLuint nRestartSymbol = m_vecIBOData.size();
	float fFiberNumber = 0;
	glm::vec3 vUp = glm::vec3(0, 1, 0);
	while(nIBOIndex < m_vecIBOData.size()-1)
	{
		GLuint nIBO0 = nRestartSymbol;
		GLuint nIBO1 = nRestartSymbol;
		GLuint nIBO2 = nRestartSymbol;
		GLuint nIBO3 = nRestartSymbol;
		glm::vec3 vPosition0;
		glm::vec3 vPosition1;
		glm::vec3 vPosition2;
		glm::vec3 vPosition3;

		if (m_vecIBOData.size() > nIBOIndex - 1 && nIBOIndex - 1 >= 0)
		{
			nIBO0 = m_vecIBOData[nIBOIndex - 1];
			if(nIBO0 != nRestartSymbol)
				vPosition0 = m_vecVBOData[nIBO0].Position;
		}
		if (m_vecIBOData.size() > nIBOIndex && nIBOIndex >= 0)
		{
			nIBO1 = m_vecIBOData[nIBOIndex];
			if (nIBO1 != nRestartSymbol)
				vPosition1 = m_vecVBOData[nIBO1].Position;
		}
		if (m_vecIBOData.size() > nIBOIndex + 1 && nIBOIndex + 1 >= 0)
		{
			nIBO2 = m_vecIBOData[nIBOIndex + 1];
			if (nIBO2 != nRestartSymbol)
				vPosition2 = m_vecVBOData[nIBO2].Position;
		}
		if (m_vecIBOData.size() > nIBOIndex + 2 && nIBOIndex +2 >= 0)
		{
			nIBO3 = m_vecIBOData[nIBOIndex + 2];
			if (nIBO3 != nRestartSymbol)
				vPosition3 = m_vecVBOData[nIBO3].Position;
		}
	
		if (nIBO1 == nRestartSymbol)
		{
			m_vecFiberStartEndIndex[fFiberNumber].nEndSSBO = nSSBOIndex;
			m_vecFiberStartEndIndex[fFiberNumber].nEndIBO = nIBOIndex;
			fFiberNumber++;
			m_vecFiberStartEndIndex[fFiberNumber].nStartSSBO = nSSBOIndex;
			m_vecFiberStartEndIndex[fFiberNumber].nStartIBO = nIBOIndex + 1;

		}

		if (nIBO1 != nRestartSymbol && nIBO2 != nRestartSymbol)
		{
			//generate coordinateMatrix in the middle of the line direction to z
			glm::mat4 mTBNMatrix;

			glm::vec3 vStartPosition = vPosition1;
			glm::vec3 vEndPosition = vPosition2;

			glm::vec3 vTangent = glm::normalize(vEndPosition - vStartPosition);
			glm::vec3 vNormal;
			if (vTangent != glm::vec3(0, 1, 0) && vTangent != glm::vec3(0, -1, 0))
			{
				vNormal = glm::normalize(glm::cross(vTangent, vUp));
			}
			else
			{
				vNormal = glm::normalize(glm::cross(vTangent, glm::vec3(1, 0, 0)));
			}
			glm::vec3 vBitangent = glm::normalize(glm::cross(vTangent, vNormal));
			vUp = -vBitangent;
			mTBNMatrix[0] = glm::vec4(vNormal.x, vNormal.y, vNormal.z, 0);
			mTBNMatrix[1] = glm::vec4(vBitangent.x, vBitangent.y, vBitangent.z, 0);
			mTBNMatrix[2] = glm::vec4(vTangent.x, vTangent.y, vTangent.z, 0);
			mTBNMatrix[3] = glm::vec4((vStartPosition + vEndPosition) / 2.0f, 1);
			m_vecTubes[nSSBOIndex].matModelMatrix = mTBNMatrix;
			m_vecTubes[nSSBOIndex].fLength = glm::length(vEndPosition - vStartPosition) * 0.5f;
			m_vecTubes[nSSBOIndex].fFiberNumber = fFiberNumber;
			m_vecTubes[nSSBOIndex].fFill0 = 0.0f;
			m_vecTubes[nSSBOIndex].fFill1 = 0.0f;
			m_vecTubes[nSSBOIndex].fFill2 = 0.0f;
			
			//calculate planes
			
			m_vecTubes[nSSBOIndex].PlaneStartPosition = vStartPosition;
			if (nIBO0 == nRestartSymbol)
			{
				m_vecTubes[nSSBOIndex].PlaneStartNormal = glm::normalize(glm::transpose(glm::inverse(glm::mat3(mTBNMatrix))) * glm::vec3(0, 0, -1));
			}
			else
			{
				glm::vec3 vDirection01Normalized = glm::normalize(vPosition0 - vPosition1);
				glm::vec3 vDirection21Normalized = glm::normalize(vPosition2 - vPosition1);
				
				glm::vec3 vInPlane0 = (vDirection01Normalized + vDirection21Normalized) * 0.5f;
				glm::vec3 vInPlane1 = glm::normalize(glm::cross(vDirection01Normalized, vDirection21Normalized));
				glm::vec3 vCross = glm::cross(vInPlane0, vInPlane1);
				m_vecTubes[nSSBOIndex].fEnableStartPlane = 1;
				if (vDirection01Normalized + vDirection21Normalized == glm::vec3(0, 0, 0))
				{
					m_vecTubes[nSSBOIndex].PlaneStartNormal = vDirection01Normalized;
				}
				else
				{
					m_vecTubes[nSSBOIndex].PlaneStartNormal = glm::normalize(glm::cross(vInPlane0, vInPlane1));
				}
				if (glm::angle(m_vecTubes[nSSBOIndex].PlaneStartNormal, vDirection01Normalized) > 3.1415926f * 0.25f)
				{
					m_vecTubes[nSSBOIndex].fEnableStartPlane = 0;
				}

			}

			m_vecTubes[nSSBOIndex].PlaneEndPosition = vEndPosition;
			if (nIBO3 == nRestartSymbol)
			{
				m_vecTubes[nSSBOIndex].PlaneEndNormal = glm::normalize(glm::transpose(glm::inverse(glm::mat3(mTBNMatrix))) * glm::vec3(0, 0, 1));
			}
			else
			{
				glm::vec3 vDirection32Normalized = glm::normalize(vPosition3 - vPosition2);
				glm::vec3 vDirection12Normalized = glm::normalize(vPosition1 - vPosition2);

				glm::vec3 vInPlane0 = (vDirection32Normalized + vDirection12Normalized) * 0.5f;
				glm::vec3 vInPlane1 = glm::normalize(glm::cross(vDirection32Normalized, vDirection12Normalized));
				m_vecTubes[nSSBOIndex].fEnableEndPlane = 1;
				if (vDirection32Normalized + vDirection12Normalized == glm::vec3(0, 0, 0))
				{
					m_vecTubes[nSSBOIndex].PlaneEndNormal = vDirection32Normalized;
				}
				else
				{
					m_vecTubes[nSSBOIndex].PlaneEndNormal = glm::normalize(glm::cross(vInPlane0, vInPlane1));
				}
				if (glm::angle(m_vecTubes[nSSBOIndex].PlaneEndNormal, vDirection32Normalized) > 3.1415926f * 0.25f)
				{
					m_vecTubes[nSSBOIndex].fEnableEndPlane = 0;
				}

			}
			nSSBOIndex++;	
		}
		nIBOIndex++;
	}

	glGenBuffers(1, &m_nSSBOTubeID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_nSSBOTubeID);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(STubeInfo) * m_vecTubes.size(), &m_vecTubes[0], GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void CFibers::EnableTubePlanes(bool bStartPlane, bool bEndPlane)
{
	if (bStartPlane)
	{
		m_nEnablePlaneStart = true;
	}
	else
	{
		m_nEnablePlaneStart = false;
	}
	if (bEndPlane)
	{
		m_nEnablePlaneEnd = true;
	}
	else
	{
		m_nEnablePlaneEnd = false;
	}
}

void CFibers::DrawLines(CCamera* pCamera, CShader* pShader)
{
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(m_vecIBOData.size());
	glBindVertexArray(m_nVAOLines);
	// draw mesh

	if (!m_bShowSingleFiber)
	{
		GLuint nLocationStart = glGetUniformLocation(pShader->GetID(), "nStart");
		GLuint nLocationEnd = glGetUniformLocation(pShader->GetID(), "nEnd");
		glUniform1i(nLocationStart, 0);
		glUniform1i(nLocationEnd, m_vecIBOData.size());
		glDrawElements(GL_LINE_STRIP, m_vecIBOData.size(), GL_UNSIGNED_INT, 0);
	}
	else
	{
		unsigned int nFiberStart = m_vecFiberStartEndIndex[m_nShowSingleFiber].nStartIBO;
		unsigned int nFiberEnd = m_vecFiberStartEndIndex[m_nShowSingleFiber].nEndIBO;
		GLuint nLocationStart = glGetUniformLocation(pShader->GetID(), "nStart");
		GLuint nLocationEnd = glGetUniformLocation(pShader->GetID(), "nEnd");
		glUniform1i(nLocationStart, nFiberStart);
		glUniform1i(nLocationEnd, nFiberEnd);
		glDrawElements(GL_LINE_STRIP, m_vecIBOData.size(), GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);
	glDisable(GL_PRIMITIVE_RESTART);
}

void CFibers::DrawTubes(CCamera* pCamera, CShader* pShader, CFrameBuffer* pFrameBuffer, float fRadius, unsigned int nRenderMode, unsigned int nVisibleTubeMode)
{
	if (pFrameBuffer)
	{
		pFrameBuffer->Bind();
	}
	glBindVertexArray(m_nVAOTube);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_nSSBOTubeID);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_nSSBOFibersIgnoreCuttingPlane);
	pShader->Bind();
	GLuint nLocationRadius = glGetUniformLocation(pShader->GetID(), "fRadius");
	glUniform1f(nLocationRadius, fRadius);
	GLint nUniformLocationRenderMode = glGetUniformLocation(pShader->GetID(), "nRenderMode");
	glUniform1i(nUniformLocationRenderMode, nRenderMode);
	GLint nUniformLocationVisibleTubeMode = glGetUniformLocation(pShader->GetID(), "nVisibleTubeMode");
	glUniform1i(nUniformLocationVisibleTubeMode, nVisibleTubeMode);

	GLint nUniformLocationViewProjectionMatrix = glGetUniformLocation(pShader->GetID(), "viewProjectionMatrix");
	glm::mat4 mViewProjectionMatrix = pCamera->GetViewProjectionMatrix();
	glUniformMatrix4fv(nUniformLocationViewProjectionMatrix, 1, GL_FALSE, &(mViewProjectionMatrix[0][0]));
	GLint nUniformLocationnCountFibers = glGetUniformLocation(pShader->GetID(), "nCountFibers");
	glUniform1i(nUniformLocationnCountFibers, GetFiberCount());

	GLuint nLocationCountSeparations = glGetUniformLocation(pShader->GetID(), "fCountTubeEdges");
	glUniform1f(nLocationCountSeparations, static_cast<float>(m_nCountTubeEdges));
	GLuint nLocationEnableExport = glGetUniformLocation(pShader->GetID(), "nEnableExport");
	glUniform1i(nLocationEnableExport, static_cast<int>(m_bEnableExport));
	GLuint nLocationEnablePlaneEnd = glGetUniformLocation(pShader->GetID(), "nEnablePlaneEnd");
	glUniform1i(nLocationEnablePlaneEnd, m_nEnablePlaneEnd);
	GLuint nLocationEnablePlaneStart = glGetUniformLocation(pShader->GetID(), "nEnablePlaneStart");
	glUniform1i(nLocationEnablePlaneStart, m_nEnablePlaneStart);

	if (m_vecCuttingPlaneEnabled.size())
	{
		GLuint nLocationEnableCuttingPlane = glGetUniformLocation(pShader->GetID(), "nEnableCuttingPlane");
		glUniform1iv(nLocationEnableCuttingPlane, m_vecCuttingPlaneEnabled.size(), &m_vecCuttingPlaneEnabled[0]);

		GLuint nLocationCuttingPlane = glGetUniformLocation(pShader->GetID(), "vCuttingPlane");
		GLfloat* pTemp = new GLfloat[m_vecCuttingPlaneEnabled.size() * 4];
		for (unsigned int i = 0; i < m_vecCuttingPlaneEnabled.size(); ++i)
		{
			pTemp[i * 4 + 0] = m_vecCuttingPlaneVectors[i].x;
			pTemp[i * 4 + 1] = m_vecCuttingPlaneVectors[i].y;
			pTemp[i * 4 + 2] = m_vecCuttingPlaneVectors[i].z;
			pTemp[i * 4 + 3] = m_vecCuttingPlaneVectors[i].a;
		}
		glUniform4fv(nLocationCuttingPlane, m_vecCuttingPlaneEnabled.size(), pTemp);
		delete[] pTemp;
	}
	GLuint nLocationCountCuttingPlanes = glGetUniformLocation(pShader->GetID(), "nCountCuttingPlanes");
	glUniform1i(nLocationCountCuttingPlanes, m_vecCuttingPlaneEnabled.size());


	if (!m_bShowSingleFiber)
	{
		GLuint nLocationOffset = glGetUniformLocation(pShader->GetID(), "nOffset");
		glUniform1i(nLocationOffset, 0);
		glDrawElementsInstanced(GL_TRIANGLES, m_vecIBOTube.size(), GL_UNSIGNED_INT, 0, m_vecTubes.size());
	}
	else
	{
		unsigned int nFiberStart = m_vecFiberStartEndIndex[m_nShowSingleFiber].nStartSSBO;
		unsigned int nTubeCount = m_vecFiberStartEndIndex[m_nShowSingleFiber].nEndSSBO - m_vecFiberStartEndIndex[m_nShowSingleFiber].nStartSSBO;
		GLuint nLocationOffset = glGetUniformLocation(pShader->GetID(), "nOffset");
		glUniform1i(nLocationOffset, nFiberStart);
		glDrawElementsInstanced(GL_TRIANGLES, m_vecIBOTube.size(), GL_UNSIGNED_INT, 0, nTubeCount);
	}
	glBindVertexArray(0);
	pShader->UnBind();

	if (m_bEnableExport)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_nVBOExport);
		std::vector<glm::vec4> dataVBO(m_vecTubes.size() * m_nCountTubeEdges * 2);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_vecTubes.size() * m_nCountTubeEdges * 2 * sizeof(glm::vec4), dataVBO.data());
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_nIBOExport);
		std::vector<GLuint> dataIBO(m_vecTubes.size() * m_nCountTubeEdges * 6);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_vecTubes.size() * m_nCountTubeEdges * 6 * sizeof(GLuint), dataIBO.data());
		
		std::vector<STubeInfo> data(m_vecTubes.size());
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_vecTubes.size() * sizeof(STubeInfo), &data[0]);

		m_pExporter->Export("d:/backup/test.obj", &dataVBO, &dataIBO);
		
	}
	m_bEnableExport = false;
	if (pFrameBuffer)
	{
		pFrameBuffer->UnBind();
	}
}

void CFibers::DrawPoints(CCamera* pCamera, CShader* pShader)
{
	int nCountPoints = m_vecTubes.size() * m_nCountTubeEdges * 2;
	glBindVertexArray(m_nVAOTube);
	glDrawArrays(GL_POINTS, 0, nCountPoints);
	glBindVertexArray(0);
}

void CFibers::BringEndingsTogether()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_nSSBOTubeID);
	std::vector<STubeInfo> data(m_vecTubes.size());
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_vecTubes.size() * sizeof(STubeInfo), &data[0]);

	for (unsigned int i = 1; i < data.size()-1; ++i)
	{
		if (data[i].nVisibleTube == 0)
		{
			data[i + 1].fEnableStartPlane = 0;
			data[i - 1].fEnableEndPlane = 0;
		}
	}

	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(STubeInfo) * data.size(), &data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

unsigned int CFibers::GetFiberCount()
{
	return m_nCountFibers;
}

void CFibers::Export()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_nVBOExport);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_vecTubes.size() * m_nCountTubeEdges * 2 * sizeof(glm::vec4), nullptr, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_nIBOExport);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_vecTubes.size() * m_nCountTubeEdges * 6 * sizeof(GLuint), nullptr, GL_STATIC_DRAW);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_nVBOExport);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_nIBOExport);

	m_bEnableExport = true;
}

void CFibers::SetCuttingPlaneVectors(std::vector<bool>& vecCuttingPlaneEnabled, std::vector<glm::vec4>& vecCuttingPlaneVectors)
{
	m_vecCuttingPlaneEnabled.resize(vecCuttingPlaneEnabled.size());
	m_vecCuttingPlaneVectors.resize(vecCuttingPlaneVectors.size());

	for (unsigned int i = 0; i < vecCuttingPlaneEnabled.size(); ++i)
	{
		m_vecCuttingPlaneEnabled[i] = static_cast<int>(vecCuttingPlaneEnabled[i]);
		m_vecCuttingPlaneVectors[i] = vecCuttingPlaneVectors[i];
	}
}

void CFibers::IgnoreCuttingPlaneForSphere(glm::vec3 vPosition, float fRadius)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_nSSBOTubeID);
	std::vector<STubeInfo> data(m_vecTubes.size());
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_vecTubes.size() * sizeof(STubeInfo), &data[0]);

	std::vector<bool> vecFiberAlreadyActivated(m_vecFiberStartEndIndex.size());
	for (unsigned int i = 0; i < vecFiberAlreadyActivated.size(); ++i)
	{
		vecFiberAlreadyActivated[i] = false;
	}

	for (unsigned int i = 0; i < data.size(); ++i)
	{
		if (glm::length(data[i].PlaneEndNormal - vPosition) < fRadius || glm::length(data[i].PlaneStartPosition - vPosition) < fRadius)
		{
			int nFiber = 0;
			bool bFoundFiberNumber = false;
			for (unsigned int j = 0; j < m_vecFiberStartEndIndex.size(); ++j)
			{
				if (m_vecFiberStartEndIndex[j].nStartSSBO <= i && m_vecFiberStartEndIndex[j].nEndSSBO >= i)
				{
					nFiber = j;
					bFoundFiberNumber = true;
				}
			}
			if (bFoundFiberNumber && !vecFiberAlreadyActivated[nFiber])
			{
				float data = 1.0f;
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_nSSBOFibersIgnoreCuttingPlane);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, nFiber * sizeof(float), sizeof(float), &data);
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
				vecFiberAlreadyActivated[nFiber] = true;
			}
		}
	}

	

}


CFibers::CFibers()
{
	m_nCountTubeEdges = 3;
	m_pExporter = new CExporter();
	m_pFrameBufferRenderVisibleTubes = new CFrameBuffer(2048, 2048, nullptr);
	m_pShaderScreenSpacedEnableVisibleTubes = new CShader();
	m_pShaderScreenSpacedEnableVisibleTubes->CreateComputeShaderProgram("../Shaders/CS_ScreenSpacedEnableVisibleTubes.glsl");
}

CFibers::~CFibers()
{

	try
	{
		delete m_pExporter;
	}
	catch (const std::exception&)
	{

	}
}

void CFibers::DisableHiddenFibers(CShader* pShader, float fRadiusTubes)
{
	//reset SSBO visibility
	for (unsigned int i = 0; i < m_vecTubes.size(); ++i)
	{
		m_vecTubes[i].nVisibleTube = 0;
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_nSSBOTubeID);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(STubeInfo) * m_vecTubes.size(), &m_vecTubes[0], GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	
	//generate camera positions
	unsigned int nDivision = 10;
	std::vector<glm::vec3> vecCameraPosition(nDivision * nDivision * 6);
	struct SCubeFaces
	{
		glm::vec3 EdgeUL;
		glm::vec3 EdgeUR;
		glm::vec3 EdgeDL;
		glm::vec3 EdgeDR;
	};
	std::vector<SCubeFaces> vecCubeEdges(6);
	vecCubeEdges[0].EdgeUL = glm::vec3(m_vMinVertex.x, m_vMaxVertex.y, m_vMaxVertex.z);
	vecCubeEdges[0].EdgeUR = glm::vec3(m_vMaxVertex.x, m_vMaxVertex.y, m_vMaxVertex.z);
	vecCubeEdges[0].EdgeDL = glm::vec3(m_vMinVertex.x, m_vMaxVertex.y, m_vMinVertex.z);
	vecCubeEdges[0].EdgeDR = glm::vec3(m_vMaxVertex.x, m_vMaxVertex.y, m_vMinVertex.z);

	vecCubeEdges[1].EdgeUL = glm::vec3(m_vMinVertex.x, m_vMinVertex.y, m_vMaxVertex.z);
	vecCubeEdges[1].EdgeUR = glm::vec3(m_vMaxVertex.x, m_vMinVertex.y, m_vMaxVertex.z);
	vecCubeEdges[1].EdgeDL = glm::vec3(m_vMinVertex.x, m_vMinVertex.y, m_vMinVertex.z);
	vecCubeEdges[1].EdgeDR = glm::vec3(m_vMaxVertex.x, m_vMinVertex.y, m_vMinVertex.z);

	vecCubeEdges[2].EdgeUL = glm::vec3(m_vMaxVertex.x, m_vMaxVertex.y, m_vMinVertex.z);
	vecCubeEdges[2].EdgeUR = glm::vec3(m_vMaxVertex.x, m_vMaxVertex.y, m_vMaxVertex.z);
	vecCubeEdges[2].EdgeDL = glm::vec3(m_vMaxVertex.x, m_vMinVertex.y, m_vMinVertex.z);
	vecCubeEdges[2].EdgeDR = glm::vec3(m_vMaxVertex.x, m_vMinVertex.y, m_vMaxVertex.z);

	vecCubeEdges[3].EdgeUL = glm::vec3(m_vMinVertex.x, m_vMaxVertex.y, m_vMinVertex.z);
	vecCubeEdges[3].EdgeUR = glm::vec3(m_vMinVertex.x, m_vMaxVertex.y, m_vMaxVertex.z);
	vecCubeEdges[3].EdgeDL = glm::vec3(m_vMinVertex.x, m_vMinVertex.y, m_vMinVertex.z);
	vecCubeEdges[3].EdgeDR = glm::vec3(m_vMinVertex.x, m_vMinVertex.y, m_vMaxVertex.z);

	vecCubeEdges[4].EdgeUL = glm::vec3(m_vMinVertex.x, m_vMaxVertex.y, m_vMaxVertex.z);
	vecCubeEdges[4].EdgeUR = glm::vec3(m_vMaxVertex.x, m_vMaxVertex.y, m_vMaxVertex.z);
	vecCubeEdges[4].EdgeDL = glm::vec3(m_vMinVertex.x, m_vMinVertex.y, m_vMaxVertex.z);
	vecCubeEdges[4].EdgeDR = glm::vec3(m_vMaxVertex.x, m_vMinVertex.y, m_vMaxVertex.z);

	vecCubeEdges[5].EdgeUL = glm::vec3(m_vMinVertex.x, m_vMaxVertex.y, m_vMinVertex.z);
	vecCubeEdges[5].EdgeUR = glm::vec3(m_vMaxVertex.x, m_vMaxVertex.y, m_vMinVertex.z);
	vecCubeEdges[5].EdgeDL = glm::vec3(m_vMinVertex.x, m_vMinVertex.y, m_vMinVertex.z);
	vecCubeEdges[5].EdgeDR = glm::vec3(m_vMaxVertex.x, m_vMinVertex.y, m_vMinVertex.z);

	unsigned int nCurrentCameraPosition = 0;

	glm::vec3 vCenter = m_vMaxVertex + m_vMinVertex * 0.5f;
	float fRadius = glm::length(m_vMaxVertex - m_vMinVertex) * 0.5f;
	for (unsigned int nFace = 0; nFace < 6; ++nFace)
	{
		for (float u = 0; u < nDivision; ++u)
		{
			for (float v = 0; v < nDivision; ++v)
			{
				glm::vec3 vU = vecCubeEdges[nFace].EdgeUL + (vecCubeEdges[nFace].EdgeUR - vecCubeEdges[nFace].EdgeUL) * (1.0f / nDivision) * u;
				glm::vec3 vD = vecCubeEdges[nFace].EdgeDL + (vecCubeEdges[nFace].EdgeDR - vecCubeEdges[nFace].EdgeDL) * (1.0f / nDivision) * u;
				glm::vec3 vPosition = vD + (vU - vD) * (1.0f / nDivision) * v;
				vecCameraPosition[nCurrentCameraPosition] = glm::normalize(vPosition - vCenter) * fRadius;
				nCurrentCameraPosition++;
			}
		}
	}

	//generate camera
	CCamera oCamera;
	oCamera.SetOrthographic(-fRadius, fRadius, -fRadius, fRadius, 0.001f, fRadius * 3.0f);
	
	//Render from all directions
	for (unsigned int i = 0; i < vecCameraPosition.size(); ++i)
	{
		glm::vec3 vViewDirection;
		vViewDirection = glm::normalize(vCenter - vecCameraPosition[i]);
		vViewDirection.x = abs(vViewDirection.x);
		vViewDirection.y = abs(vViewDirection.y);
		vViewDirection.z = abs(vViewDirection.z);

		if (vViewDirection != glm::vec3(0, 1, 0))
		{
			oCamera.SetOrientation(vecCameraPosition[i], glm::normalize(vCenter - vecCameraPosition[i]), glm::vec3(0, 1, 0));
		}
		else
		{
			oCamera.SetOrientation(vecCameraPosition[i], glm::normalize(vCenter - vecCameraPosition[i]), glm::vec3(0, 0, 1));
		}
		m_pFrameBufferRenderVisibleTubes->Clear();
		this->DrawTubes(&oCamera, pShader, m_pFrameBufferRenderVisibleTubes, fRadiusTubes, 0, 3);
		
		//execute computeshader for enable visible tubes
		m_pShaderScreenSpacedEnableVisibleTubes->Bind();
		GLint nShaderID = m_pShaderScreenSpacedEnableVisibleTubes->GetID();
		GLint baseImageLoc = glGetUniformLocation(nShaderID, "sTexture");
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_pFrameBufferRenderVisibleTubes->GetColorPickingTexture());
		glUniform1i(baseImageLoc, 0);
		
		glDispatchCompute(2048, 2048, 1);
		m_pShaderScreenSpacedEnableVisibleTubes->UnBind();	
	}

}
