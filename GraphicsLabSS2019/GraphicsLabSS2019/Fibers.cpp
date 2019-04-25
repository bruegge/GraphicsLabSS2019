#include "Fibers.h"
#include <iostream>
#include <fstream>
#include <string>

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
	std::string line;
	std::ifstream myfile(pFile);
	if (myfile.is_open())
	{
		unsigned int nCountvertices = 0;
		m_nCountFibers = 0;
		unsigned int nLine = 0;
		while (getline(myfile, line))
		{
			if (nLine == 3)
			{
				std::vector<std::string> lineSplit = split(line, ' ');
				nCountvertices = std::stoi(lineSplit[2]);
			}

			if (nLine == 7)
			{
				std::vector<std::string> lineSplit = split(line, ' ');
				m_nCountFibers = std::stoi(lineSplit[2]);
			}

			if (nLine > 8)
			{
				break;
			}
			nLine++;
		}

		m_vecVBOData.resize(nCountvertices);
		m_vecIBOData.resize(nCountvertices + 1);
		for(unsigned int i = 0; i< nCountvertices && getline(myfile, line); ++i)
		{
			std::vector<std::string> splitString = split(line, ' ');
			m_vecVBOData[i].Position.x = std::stof(splitString[0]);
			m_vecVBOData[i].Position.y = std::stof(splitString[1]);
			m_vecVBOData[i].Position.z = std::stof(splitString[2]);
		}

		GLuint nFibreStart = 0;
		GLuint nIBOIndex = 0;
		for (unsigned int i = 0; i< m_nCountFibers && getline(myfile, line); ++i)
		{
			GLuint nFibreEnd = std::stoi(line);
			for (unsigned int j = nFibreStart; j < nFibreEnd; ++j)
			{
				m_vecIBOData[nIBOIndex++] = j;
			}
			//split char
			m_vecIBOData[nIBOIndex++] = m_vecIBOData.size();
			nFibreStart = nFibreEnd + 1;
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
	for (unsigned int l = 0; l <= 1; ++l)
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
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
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
	
	unsigned int nIBOIndex = 0;
	unsigned int nSSBOIndex = 0;
	GLuint nRestartSymbol = m_vecIBOData.size();
	while(nIBOIndex < m_vecIBOData.size()-1)
	{
		GLuint nIBO0 = m_vecIBOData[nIBOIndex];
		GLuint nIBO1 = m_vecIBOData[nIBOIndex + 1];

		if (nIBO0 != nRestartSymbol && nIBO1 != nRestartSymbol)
		{
			glm::mat4 mTBNMatrix;

			glm::vec3 vStartPosition = m_vecVBOData[nIBO0].Position;
			glm::vec3 vEndPosition = m_vecVBOData[nIBO1].Position;

			glm::vec3 vTangent = glm::normalize(vEndPosition - vStartPosition);
			glm::vec3 vNormal = glm::normalize(glm::cross(vTangent, glm::vec3(0, 1, 0)));
			glm::vec3 vBitangent = glm::normalize(glm::cross(vTangent, vNormal));
			mTBNMatrix[2] = glm::vec4(vTangent.x, vTangent.y, vTangent.z, 0);
			mTBNMatrix[1] = glm::vec4(vBitangent.x, vBitangent.y, vBitangent.z, 0);
			mTBNMatrix[0] = glm::vec4(vNormal.x, vNormal.y, vNormal.z, 0);
			mTBNMatrix[3] = glm::vec4(vStartPosition.x, vStartPosition.y, vStartPosition.z, 1);

			m_vecTubes[nSSBOIndex].matModelMatrix = mTBNMatrix;
			m_vecTubes[nSSBOIndex].fLength = glm::length(vEndPosition - vStartPosition);
			m_vecTubes[nSSBOIndex].fFill0 = 0;
			m_vecTubes[nSSBOIndex].fFill1 = 0;
			m_vecTubes[nSSBOIndex].fFill2 = 0;
			nSSBOIndex++;
		}
		nIBOIndex++;
	}
	//

	glGenBuffers(1, &m_nSSBOTubeID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_nSSBOTubeID);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(STubeInfo) * m_vecTubes.size(), &m_vecTubes[0], GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void CFibers::DrawLines(CCamera* pCamera, CShader* pShader)
{
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(m_vecIBOData.size());
	glBindVertexArray(m_nVAOLines);
	// draw mesh
	glDrawElements(GL_LINE_STRIP, m_vecIBOData.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glDisable(GL_PRIMITIVE_RESTART);
}

void CFibers::DrawTubes(CCamera* pCamera, CShader* pShader, float fRadius)
{
	glBindVertexArray(m_nVAOTube);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_nSSBOTubeID);
	glUniform1f(glGetUniformLocation(pShader->GetID(), "fRadius"), fRadius);
	glDrawElementsInstanced(GL_TRIANGLES, m_vecIBOTube.size(), GL_UNSIGNED_INT, 0, m_vecTubes.size());
	glBindVertexArray(0);
}

CFibers::CFibers()
{
	m_nCountTubeEdges = 3;
}

CFibers::~CFibers()
{
}

