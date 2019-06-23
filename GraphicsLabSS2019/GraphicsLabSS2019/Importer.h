#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "glad/glad.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class CImporter
{
public:
	CImporter();
	~CImporter();

	void Import(const char* pFile);

private:
	std::vector<GLuint> m_vecIBO;
	std::vector<glm::vec3> m_vecVBO;
};

