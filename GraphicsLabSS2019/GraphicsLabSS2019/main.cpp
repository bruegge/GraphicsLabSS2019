#pragma once
#include <iostream>
#include "../ImGUI/imgui.h"
#include "../ImGUI/imgui_impl_glfw.h"
#include "../ImGUI/imgui_impl_opengl3.h"

#include "WindowGLFW.h"
#include "Camera.h"
#include "Shader.h"
#include "Fibers.h"

CWindowGLFW* pWindow = nullptr;
CCamera* pCamera = nullptr;
CShader* pShaderLine = nullptr;
CShader* pShaderTube = nullptr;
CFibers* pFibers = nullptr;

bool bEnableWireFrame = false;
bool bDrawLine = true;
bool bDrawTube = false;
float fRadius = 0.1f;
int nCountTubeEdges = 3;

void LoadContent()
{
	pWindow = new CWindowGLFW(800, 600);
	pCamera = new CCamera(glm::vec3(0, 0, -50), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), 110, static_cast<float>(pWindow->GetWindowSize().x) / static_cast<float>(pWindow->GetWindowSize().y), 0.01f, 1000.0f);
	pShaderLine = new CShader();
	pShaderLine->CreateShaderProgram("../Shaders/VS_ShowLine.glsl", nullptr, nullptr,nullptr, "../Shaders/FS_ShowLine.glsl");
	pShaderTube = new CShader();
	pShaderTube->CreateShaderProgram("../Shaders/VS_ShowTube.glsl", nullptr, nullptr, nullptr, "../Shaders/FS_ShowTube.glsl");
	pFibers = new CFibers();
	pFibers->LoadFile("../Models/sagittal-t1-video.ply");

	{ //GUI
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(pWindow->GetWindowID(), true);
		ImGui_ImplOpenGL3_Init("#version 430");
	}
}

void InputManagement()
{
	float fRotationSpeed = 0.01f;
	float fTranslationSpeed = 0.1f;
	if (glfwGetKey(pWindow->GetWindowID(), GLFW_KEY_W) == GLFW_PRESS)
	{
		pCamera->Translate(glm::vec3(0, 0, fTranslationSpeed));
	}
	if (glfwGetKey(pWindow->GetWindowID(), GLFW_KEY_S) == GLFW_PRESS)
	{
		pCamera->Translate(glm::vec3(0, 0, -fTranslationSpeed));
	}
	if (glfwGetKey(pWindow->GetWindowID(), GLFW_KEY_A) == GLFW_PRESS)
	{
		pCamera->Translate(glm::vec3(fTranslationSpeed, 0, 0));
	}
	if (glfwGetKey(pWindow->GetWindowID(), GLFW_KEY_D) == GLFW_PRESS)
	{
		pCamera->Translate(glm::vec3(-fTranslationSpeed, 0, 0));
	}
	if (glfwGetKey(pWindow->GetWindowID(), GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		pCamera->Rotate(glm::vec3(0, 1, 0), -fRotationSpeed);
	}
	if (glfwGetKey(pWindow->GetWindowID(), GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		pCamera->Rotate(glm::vec3(0, 1, 0), fRotationSpeed);
	}
	if (glfwGetKey(pWindow->GetWindowID(), GLFW_KEY_UP) == GLFW_PRESS)
	{
		pCamera->Rotate(glm::vec3(1, 0, 0), -fRotationSpeed);
	}
	if (glfwGetKey(pWindow->GetWindowID(), GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		pCamera->Rotate(glm::vec3(1, 0, 0), fRotationSpeed);
	}
}

void GameLoop()
{
	glClearColor(0.1f, 0.1f, 0.02f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	bool bExitGame = false;
	while (!bExitGame)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear the color and the depth buffer

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		bExitGame = pWindow->ManageInputs();
		InputManagement();

	
		if (bEnableWireFrame)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	
		//draw geometry
		if (bDrawLine)
		{
			pShaderLine->Bind();
			GLint nUniformLocationViewProjectionMatrix = glGetUniformLocation(pShaderLine->GetID(), "viewProjectionMatrix");
			glm::mat4 mViewProjectionMatrix = pCamera->GetViewProjectionMatrix();
			glUniformMatrix4fv(nUniformLocationViewProjectionMatrix, 1, GL_FALSE, &(mViewProjectionMatrix[0][0]));
			pFibers->DrawLines(pCamera, pShaderLine);
			pShaderLine->UnBind();
		}
		if (bDrawTube)
		{
			pFibers->ChangeTubeEdges(nCountTubeEdges);
			pShaderTube->Bind();
			GLint nUniformLocationViewProjectionMatrix = glGetUniformLocation(pShaderTube->GetID(), "viewProjectionMatrix");
			glm::mat4 mViewProjectionMatrix = pCamera->GetViewProjectionMatrix();
			glUniformMatrix4fv(nUniformLocationViewProjectionMatrix, 1, GL_FALSE, &(mViewProjectionMatrix[0][0]));
			pFibers->DrawTubes(pCamera, pShaderTube, fRadius);
			pShaderTube->UnBind();
		}
		
		{
			ImGui::Begin("Settings");                          // Create a window 

															  
			ImGui::Text("(%.1f FPS)", ImGui::GetIO().Framerate);
			ImGui::Checkbox("Enable WireFrames", &bEnableWireFrame);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Draw Lines", &bDrawLine);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Draw Tubes", &bDrawTube);      // Edit bools storing our window open/close state
			ImGui::SliderFloat("Radius", &fRadius, 0.001f, 10.0f);
			ImGui::SliderInt("CountTubeEdges", &nCountTubeEdges, 3, 100);
			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		pWindow->SwapBuffers();
	}

}

void DeleteContent()
{
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	delete pWindow;
	delete pCamera;
	delete pShaderLine;
	delete pShaderTube;
	delete pFibers;
}

void main()
{
	LoadContent();
	GameLoop();
	DeleteContent();
}
