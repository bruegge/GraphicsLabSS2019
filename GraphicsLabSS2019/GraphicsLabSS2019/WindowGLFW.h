#pragma once
#include <glad/glad.h>
#include <glfw3.h>
#include <glm/glm.hpp>


class CWindowGLFW
{
public:
	CWindowGLFW(unsigned int nWidth, unsigned int nHeight);
	~CWindowGLFW();

	void SetWindowSize(unsigned int nWidth, unsigned int nHeight);
	glm::uvec2 GetWindowSize() const;
	bool ManageInputs() const;
	void SwapBuffers() const;
	GLFWwindow* GetWindowID();

private:
	GLFWwindow* m_pWindow = nullptr; //pointer to the GLFW window
	unsigned int m_nScreenWidth;
	unsigned int m_nScreenHeight;
};

