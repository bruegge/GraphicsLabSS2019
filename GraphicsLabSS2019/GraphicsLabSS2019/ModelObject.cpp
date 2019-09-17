#include <iostream>
#include <glad\glad.h>
#include "ModelObject.h"
#include <math.h>

static unsigned int nCounter = 0;

CMesh::CMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, glm::vec3 minValues, glm::vec3 maxValues)
{
	this->vertices = vertices;
	this->indices = indices;
	this->minValues = minValues;
	this->maxValues = maxValues;
	this->m_nMeshID = nCounter++;
	setupMesh();
}

CMesh::~CMesh()
{
}

glm::vec3 CMesh::getMaxValues()
{
	return this->maxValues;
}

glm::vec3 CMesh::getMinValues()
{
	return this->minValues;
}

void CMesh::setupMesh()
{
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glBindVertexArray(0);
}

void CMesh::Draw(CShader* shader)
{
	glBindVertexArray(VAO);

	glUniform1i(glGetUniformLocation(shader->GetID(), "MeshID"), this->m_nMeshID);
	// draw mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void CMesh::DrawInstanced(CShader* shader, unsigned int nCount)
{
	glBindVertexArray(VAO);

	glUniform1i(glGetUniformLocation(shader->GetID(), "MeshID"), this->m_nMeshID);
	// draw mesh
	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, nCount);
	glBindVertexArray(0);
}



CModel::CModel(char *path)
{
	loadModel(path);
}

void CModel::Draw(CShader* pShader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(pShader);
	}
}

void CModel::DrawInstanced(CShader* pShader, unsigned int nCount)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].DrawInstanced(pShader, nCount);
	}
}

void CModel::loadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
	}
	else
	{
		directory = path.substr(0, path.find_last_of('/'));
		processNode(scene->mRootNode, scene);
	}

	if (this->meshes.size())
	{
		this->maxValues = this->meshes[0].getMaxValues();
		this->minValues = this->meshes[0].getMinValues();

		for (int i = 0; i < this->meshes.size(); i++)
		{
			this->maxValues.x = std::max(this->maxValues.x, this->meshes[i].getMaxValues().x);
			this->maxValues.y = std::max(this->maxValues.y, this->meshes[i].getMaxValues().y);
			this->maxValues.z = std::max(this->maxValues.z, this->meshes[i].getMaxValues().z);

			this->minValues.x = std::min(this->minValues.x, this->meshes[i].getMinValues().x);
			this->minValues.y = std::min(this->minValues.y, this->meshes[i].getMinValues().y);
			this->minValues.z = std::min(this->minValues.z, this->meshes[i].getMinValues().z);
		}
	}
	return;
}

glm::vec3 CModel::getMinValues()
{
	return this->minValues;
}

glm::vec3 CModel::getMaxValues()
{
	return this->maxValues;
}

void CModel::processNode(aiNode *node, const aiScene *scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

CMesh CModel::processMesh(aiMesh *mesh, const aiScene *scene)
{
	std::vector<CMesh::Vertex> vertices;
	std::vector<unsigned int> indices;

	glm::vec3 maxValues = glm::vec3(mesh->mVertices[0].x, mesh->mVertices[0].y, mesh->mVertices[0].z);
	glm::vec3 minValues = glm::vec3(mesh->mVertices[0].x, mesh->mVertices[0].y, mesh->mVertices[0].z);

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		CMesh::Vertex vertex;
		// process vertex positions, normals and texture coordinates
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		maxValues.x = std::max(vector.x, maxValues.x);
		maxValues.y = std::max(vector.y, maxValues.y);
		maxValues.z = std::max(vector.z, maxValues.z);
		minValues.x = std::min(vector.x, minValues.x);
		minValues.y = std::min(vector.y, minValues.y);
		minValues.z = std::min(vector.z, minValues.z);

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		if (mesh->HasTangentsAndBitangents())
		{
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;

			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}

		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
		{
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}
	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	return CMesh(vertices, indices, minValues, maxValues);
}


