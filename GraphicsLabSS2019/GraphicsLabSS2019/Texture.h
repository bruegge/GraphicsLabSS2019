#pragma once
#include <vector>
#include <glad\glad.h>

class CTexture
{
public:
	CTexture();
	~CTexture();

	static GLuint LoadTexture2DArray(std::vector<const char*>& pPath);
	static GLuint LoadTexture2D(const char * pPath);

};

