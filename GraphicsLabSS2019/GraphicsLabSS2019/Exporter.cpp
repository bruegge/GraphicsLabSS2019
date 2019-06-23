#include "Exporter.h"
#include <iostream>


CExporter::CExporter()
{
	m_pExporter = new Assimp::Exporter();
	m_pScene = new aiScene();
}


CExporter::~CExporter()
{
	m_pExporter->FreeBlob();
	delete m_pExporter;
	try
	{
		delete m_pScene->mRootNode;
		delete[] m_pScene->mMaterials;
		delete[] m_pScene->mMeshes;
		delete m_pScene;
	}
	catch (const std::exception& exception)
	{

	}
}

void CExporter::Export(const char* path, std::vector<glm::vec4>* vecVBO, std::vector<GLuint>* vecIBO)
{	
	std::vector<glm::vec3> vertices(vecIBO->size());

	unsigned int nCurrentVertex = 0;
	for (unsigned int i = 0; i < vecIBO->size(); i += 3)
	{
		glm::vec3 vertex0 = vecVBO->at(vecIBO->at(i));
		glm::vec3 vertex1 = vecVBO->at(vecIBO->at(i + 1));
		glm::vec3 vertex2 = vecVBO->at(vecIBO->at(i + 2));

		if (!(vertex0 == glm::vec3(0, 0, 0) && vertex1 == glm::vec3(0, 0, 0) && vertex2 == glm::vec3(0, 0, 0)))
		{
			vertices[nCurrentVertex] = vertex0;
			vertices[nCurrentVertex+1] = vertex1;
			vertices[nCurrentVertex+2] = vertex2;
			nCurrentVertex += 3;
		}
	}

	vertices.resize(nCurrentVertex - 3);

	size_t nCount = m_pExporter->GetExportFormatCount();

	for (unsigned i = 0; i < nCount; ++i)
	{
		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
		const aiExportFormatDesc* pFormat = m_pExporter->GetExportFormatDescription(i);
		std::cout << "ID: " << pFormat->id << std::endl;
		std::cout << "Extension: " << pFormat->fileExtension << std::endl;
		std::cout << "Description: " << pFormat->description << std::endl;
	}
	
	
	m_pScene->mRootNode = new aiNode();
	  		
	m_pScene->mMaterials = new aiMaterial*[1];
	m_pScene->mMaterials[0] = nullptr;
	m_pScene->mNumMaterials = 1;
	  		
	m_pScene->mMaterials[0] = new aiMaterial();
	  		
	m_pScene->mMeshes = new aiMesh*[1];
	m_pScene->mMeshes[0] = nullptr;
	m_pScene->mNumMeshes = 1;
	  		
	m_pScene->mMeshes[0] = new aiMesh();
	m_pScene->mMeshes[0]->mMaterialIndex = 0;
	  		
	m_pScene->mRootNode->mMeshes = new unsigned int[1];
	m_pScene->mRootNode->mMeshes[0] = 0;
	m_pScene->mRootNode->mNumMeshes = 1;

	auto pMesh = m_pScene->mMeshes[0];
	//MatchInfo
		//const auto& vVertices = matchInfo.mvVertices;

	pMesh->mVertices = new aiVector3D[vertices.size()];
	pMesh->mNormals = new aiVector3D[vertices.size()];
	pMesh->mTextureCoords[0] = new aiVector3D[vertices.size()];

	pMesh->mNumVertices = vertices.size();

	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
		pMesh->mVertices[i].Set(vertices[i].x, vertices[i].y, vertices[i].z);
		pMesh->mNormals[i] = aiVector3D(vertices[i].x, vertices[i].y, vertices[i].z);
		pMesh->mTextureCoords[0][i] = aiVector3D(0, 0, 0);
	}
		
	pMesh->mFaces = new aiFace[vertices.size() / 3];
	pMesh->mNumFaces = (unsigned int)(vertices.size() / 3);

	int k = 0;
	for (int i = 0; i < (vertices.size() / 3); i++)
	{
		aiFace &face = pMesh->mFaces[i];
		face.mIndices = new unsigned int[3];
		face.mNumIndices = 3;

		face.mIndices[0] = i * 3;
		face.mIndices[1] = i * 3 + 1;
		face.mIndices[2] = i * 3 + 2;
	}

	m_pExporter->Export(m_pScene, "obj", path);
}

