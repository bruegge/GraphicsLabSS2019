#include "Texture.h"
#include "ImageLoader.h"
#include <iostream>
#include <sstream>
#include <string>

CTexture::CTexture()
{
}

CTexture::~CTexture()
{
}

GLuint CTexture::LoadTexture2D(const char * pPath)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
										   // set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	unsigned char *data = stbi_load(pPath, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (nrChannels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else if (nrChannels == 4)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}

		//glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	return texture;
}


GLuint CTexture::LoadTexture2DArray(std::vector<const char*>& pPath)
{
	if (pPath.size())
	{
		int width, height, nrChannels;
		unsigned char *pData = stbi_load(pPath[0], &width, &height, &nrChannels, 0);
		stbi_image_free(pData);

		GLuint result = 0;
		glGenTextures(1, &result);
		glBindTexture(GL_TEXTURE_2D_ARRAY, result);
		if (nrChannels == 3)
		{
			glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGB32F, width, height, pPath.size());
		}
		if (nrChannels == 4)
		{
			glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA32F, width, height, pPath.size());
		}

		for (unsigned int i = 0; i < pPath.size(); ++i)
		{
			pData = stbi_load(pPath[i], &width, &height, &nrChannels, 0);
			if (pData)
			{
				if (nrChannels == 3)
				{
					glBindTexture(GL_TEXTURE_2D_ARRAY, result);
					glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, pData);
				}
				if (nrChannels == 4)
				{
					glBindTexture(GL_TEXTURE_2D_ARRAY, result);
					glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, pData);
				}
			}
			stbi_image_free(pData);
		}

		return result;
	}
	return 0;
}


