#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "glad/glad.h"

#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class CExporter
{
public:
	CExporter();
	~CExporter();

	void Export(const char* path, std::vector<glm::vec4>* vecVBO, std::vector<GLuint>* vecIBO, std::vector<glm::vec4>* vecVBO2);

private:
	Assimp::Exporter* m_pExporter;
	aiScene* m_pScene;

};

