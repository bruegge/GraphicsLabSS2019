#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include "Shader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



class CMesh
{
public:
	~CMesh();

	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
	};

	enum class ETextureType : int
	{
		Diffuse = 1,
		Specular = 2,
		Normal = 3,
		Height = 4
	};


	/*  Mesh Data  */
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	/*  Functions  */
	CMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, glm::vec3 minValues, glm::vec3 maxValues);
	void Draw(CShader* shader);
	void DrawInstanced(CShader* shader, unsigned int nCount);
	glm::vec3 getMinValues();
	glm::vec3 getMaxValues();

private:
	/*  Render data  */
	unsigned int VAO, VBO, EBO, m_nMeshID;
	glm::vec3 minValues;
	glm::vec3 maxValues;

	/*  Functions    */
	void setupMesh();
};

class CModel
{
public:
	/*  Functions   */
	CModel(char *path);
	void Draw(CShader* pShader);
	void DrawInstanced(CShader* pShader, unsigned int nCount);
	glm::vec3 getMinValues();
	glm::vec3 getMaxValues();
private:
	/*  Model Data  */
	std::vector<CMesh> meshes;
	std::string directory;
	/*  Functions   */
	void loadModel(std::string path);
	void processNode(aiNode *node, const aiScene *scene);
	CMesh processMesh(aiMesh *mesh, const aiScene *scene);
	glm::vec3 minValues;
	glm::vec3 maxValues;
};
