#pragma once
#include <iostream>
#include <ctime>
#include "../ImGUI/imgui.h"
#include "../ImGUI/imgui_impl_glfw.h"
#include "../ImGUI/imgui_impl_opengl3.h"
#include <sstream>
#include "WindowGLFW.h"
#include "Camera.h"
#include "Shader.h"
#include "Fibers.h"
#include "Exporter.h"
#include "FrameBuffer.h"
#include "ModelObject.h"

CWindowGLFW* pWindow = nullptr;
CCamera* pCamera = nullptr;
CShader* pShaderLine = nullptr;
CShader* pShaderTube = nullptr; 
CShader* pShaderPoints = nullptr;
CShader* pShaderFrameBuffer = nullptr;
CShader* pShaderSphere = nullptr;
CFibers* pFibers = nullptr;
CFrameBuffer* pFrameBuffer = nullptr;
CModel* pSphere = nullptr;

bool bEnableShowFromCameraPosition = false;
bool bEnableWireFrame = false;
bool bEnableSphereMarker = false;
bool bDrawLine = true;
bool bDrawTube = false;
bool bDrawPoints = false;
bool bEnablePlaneStart = false;
bool bEnablePlaneEnd = false;
bool bEnableCuttingPlane0 = false;
bool bEnableCuttingPlane1 = false;
float fCameraRotationSpeed = 1.0f;
float fCameraTranslationSpeed = 1.0f;
float fRadius = 0.1f;
float fSphereRadius = 1.0f;
float fPointSize = 2.0f;
glm::vec3 vSpherePosition = glm::vec3(0, 0, 0);
std::vector<glm::vec4> vecCuttingPlaneVectors;
std::vector<bool> vecCuttingPlaneEnabled;
int nCountCuttingPlanes = 0;
int nCountTubeEdges = 3;
int nCountTubeEdgesOld = 2;
int nRenderMode = 0;
int nShowFromCameraPosition = 0;
int nVisibleTubeMode = 2;
int nMinNumberOfFibers = 1;

void LoadContent()
{
	pWindow = new CWindowGLFW(1200, 1000);
	pCamera = new CCamera(glm::vec3(-100, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, 1), 110, static_cast<float>(pWindow->GetWindowSize().x) / static_cast<float>(pWindow->GetWindowSize().y), 0.01f, 1000.0f);
	pShaderLine = new CShader();
	pShaderLine->CreateShaderProgram("../Shaders/VS_ShowLine.glsl", nullptr, nullptr,nullptr, "../Shaders/FS_ShowLine.glsl");
	pShaderTube = new CShader();
	pShaderTube->CreateShaderProgram("../Shaders/VS_ShowTube.glsl", nullptr, nullptr, "../Shaders/GS_ShowTube.glsl", "../Shaders/FS_ShowTube.glsl");
	pShaderPoints = new CShader();
	pShaderPoints->CreateShaderProgram("../Shaders/VS_ShowPoints.glsl", nullptr, nullptr, nullptr, "../Shaders/FS_ShowPoints.glsl");
	pShaderFrameBuffer = new CShader();
	pShaderFrameBuffer->CreateShaderProgram("../Shaders/VS_FrameBuffer.glsl", nullptr, nullptr, nullptr, "../Shaders/FS_FrameBuffer.glsl");
	pFibers = new CFibers();
	pFrameBuffer = new CFrameBuffer(pWindow->GetWindowSize().x, pWindow->GetWindowSize().y, pShaderFrameBuffer);
	pFibers->LoadFile("../Models/hcp-tractography.ply");
	pSphere = new CModel("../Models/Sphere.obj");
	pShaderSphere = new CShader();
	pShaderSphere->CreateShaderProgram("../Shaders/VS_Sphere.glsl", nullptr, nullptr, nullptr, "../Shaders/FS_Sphere.glsl");
	{ //GUI
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(pWindow->GetWindowID(), true);
		ImGui_ImplOpenGL3_Init("#version 430");
	}
}

void InputManagement(double dElapsed_ms)
{
	float fRotationSpeed = fCameraRotationSpeed * 1000.0f * static_cast<float>(dElapsed_ms);
	float fTranslationSpeed = fCameraTranslationSpeed * 10000.0f * static_cast<float>(dElapsed_ms);
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
	if (glfwGetKey(pWindow->GetWindowID(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		pFibers->EnableSingleFiber(false, 0);
	}
	if (bDrawTube && glfwGetMouseButton(pWindow->GetWindowID(), GLFW_MOUSE_BUTTON_LEFT))
	{
		double dPosX;
		double dPosY;

		glfwGetCursorPos(pWindow->GetWindowID(), &dPosX, &dPosY);
		dPosY = pWindow->GetWindowSize().y - dPosY;
		
		unsigned int nFiberNumber = 0;
		bool bSelected = pFrameBuffer->GetFiberNumber(static_cast<unsigned int>(dPosX), static_cast<unsigned int>(dPosY), nFiberNumber);
		if (bSelected)
		{
			/*if (!pFibers->IsSingleFiberEnabled())
			{
				pFibers->EnableSingleFiber(bSelected, nFiberNumber);
			}*/
			pFibers->ToggleIgnoreCuttingPlaneFiber(nFiberNumber);
		}
	}

}

void GameLoop()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	bool bExitGame = false;
	clock_t cTimeStamp = clock();

	while (!bExitGame)
	{
		glPointSize(fPointSize);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear the color and the depth buffer

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		bExitGame = pWindow->ManageInputs();

		clock_t cNewTimeStamp = clock();
		double dElapsed_ms = double(cNewTimeStamp - cTimeStamp) / (CLOCKS_PER_SEC * 1000.0);
		cTimeStamp = cNewTimeStamp;
		InputManagement(dElapsed_ms);

	
		if (bEnableWireFrame)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	
		pFrameBuffer->Clear();

		//draw geometry
		if (bDrawLine)
		{
			pFrameBuffer->Bind();
			pShaderLine->Bind();
			GLint nUniformLocationViewProjectionMatrix = glGetUniformLocation(pShaderLine->GetID(), "viewProjectionMatrix");
			glm::mat4 mViewProjectionMatrix = pCamera->GetViewProjectionMatrix();
			glUniformMatrix4fv(nUniformLocationViewProjectionMatrix, 1, GL_FALSE, &(mViewProjectionMatrix[0][0]));
			pFibers->DrawLines(pCamera, pShaderLine);
			pShaderLine->UnBind();
			pFrameBuffer->UnBind();
		}
		if (bDrawTube)
		{
			glDisable(GL_BLEND);
			if (nCountTubeEdgesOld != nCountTubeEdges)
			{
				pFibers->ChangeTubeEdges(nCountTubeEdges);
				nCountTubeEdgesOld = nCountTubeEdges;
			}
			
			pFibers->DrawTubes(pCamera, pShaderTube, pFrameBuffer, fRadius, nRenderMode, nVisibleTubeMode);
			
		}
		if (bDrawPoints)
		{
			pShaderPoints->Bind();
			GLint nUniformLocationViewProjectionMatrix = glGetUniformLocation(pShaderPoints->GetID(), "viewProjectionMatrix");
			glm::mat4 mViewProjectionMatrix = pCamera->GetViewProjectionMatrix();
			glUniformMatrix4fv(nUniformLocationViewProjectionMatrix, 1, GL_FALSE, &(mViewProjectionMatrix[0][0]));
			pFibers->DrawPoints(pCamera, pShaderPoints);
			pShaderPoints->UnBind();
		}
		
		if (bEnableSphereMarker)
		{
			glEnable(GL_BLEND);

			pFrameBuffer->Bind();
			pShaderSphere->Bind();
			GLint nUniformLocationViewProjectionMatrix = glGetUniformLocation(pShaderSphere->GetID(), "viewProjectionMatrix");
			glm::mat4 mViewProjectionMatrix = pCamera->GetViewProjectionMatrix();
			glUniformMatrix4fv(nUniformLocationViewProjectionMatrix, 1, GL_FALSE, &(mViewProjectionMatrix[0][0]));

			GLint nUniformLocationRadius = glGetUniformLocation(pShaderSphere->GetID(), "fRadius");
			glUniform1f(nUniformLocationRadius, fSphereRadius);

			GLint nUniformLocationPosition = glGetUniformLocation(pShaderSphere->GetID(), "vPosition");
			glUniform3f(nUniformLocationPosition, vSpherePosition.x, vSpherePosition.y, vSpherePosition.z);


			pSphere->Draw(pShaderSphere);
			pShaderSphere->UnBind();
			pFrameBuffer->UnBind();
			glDisable(GL_BLEND);

		}

		pFrameBuffer->DrawToScreen();
		pFrameBuffer->UnBind();

		{
			ImGui::Begin("Settings");                          

															  
			ImGui::Text("(%.1f FPS)", ImGui::GetIO().Framerate);
			ImGui::Checkbox("Enable WireFrames", &bEnableWireFrame);      
			ImGui::Checkbox("Draw Lines", &bDrawLine);      
			ImGui::Checkbox("Draw Tubes", &bDrawTube);      
			ImGui::RadioButton("Show Normals", &nRenderMode, 0);
			ImGui::RadioButton("Show Fiber numbers", &nRenderMode, 1);
			ImGui::RadioButton("Show Fiber Direction", &nRenderMode, 2);
			ImGui::RadioButton("Show Fiber Depth", &nRenderMode, 3);
			if (ImGui::Button("Detect Inner and Outer Tubes"))
			{
				pFibers->DisableHiddenFibers(pShaderTube, fRadius);
			}
			
			ImGui::SliderFloat("Radius", &fRadius, 0.001f, 10.0f);
			ImGui::SliderFloat("Point Size", &fPointSize, 1.0f, 10.0f);
			ImGui::SliderInt("CountTubeEdges", &nCountTubeEdges, 3, 100);
		
			
			ImGui::RadioButton("Show inner Tubes", &nVisibleTubeMode, 0);
			ImGui::RadioButton("Show outer Tubes", &nVisibleTubeMode, 1);
			ImGui::RadioButton("Show all Tubes", &nVisibleTubeMode, 2);
			
			pFibers->EnableTubePlanes(bEnablePlaneStart, bEnablePlaneEnd);
			if (ImGui::Button("Export"))
			{
				pFibers->Export();
			}
			if (ImGui::Button("Close Fiber Endings"))
			{
				pFibers->BringEndingsTogether();
			}
			ImGui::End();

			/////
			ImGui::Begin("Controlls");                         
			ImGui::SliderFloat("Camera rotation Speed", &fCameraRotationSpeed, 0.01f, 10.0f);
			ImGui::SliderFloat("Camera translation Speed", &fCameraTranslationSpeed, 0.01f, 10.0f);

			ImGui::End();

			/////
			ImGui::Begin("Edit");
			
			ImGui::Checkbox("Enable Sphere Marker", &bEnableSphereMarker);
			ImGui::SliderFloat("Sphere Radius", &fSphereRadius, 0.1f, 20.0f);

			float aSpherePosition[3] = { vSpherePosition.x, vSpherePosition.y, vSpherePosition.z };
			ImGui::InputFloat3("Sphere Position", aSpherePosition);
			vSpherePosition = glm::vec3(aSpherePosition[0], aSpherePosition[1], aSpherePosition[2]);
			if (ImGui::Button("Ignor Cutting Plane Sphere"))
			{
				pFibers->IgnoreCuttingPlaneForSphere(vSpherePosition, fSphereRadius);
			}

			for (unsigned int i = 0; i < nCountCuttingPlanes; ++i)
			{
				float temp[4] = { vecCuttingPlaneVectors[i].x, vecCuttingPlaneVectors[i].y ,vecCuttingPlaneVectors[i].z ,vecCuttingPlaneVectors[i].a };
			
				std::stringstream name0;
				name0 << "Plane " << i;

				ImGui::InputFloat4(name0.str().c_str(), temp);
				vecCuttingPlaneVectors[i].x = temp[0];
				vecCuttingPlaneVectors[i].y = temp[1];
				vecCuttingPlaneVectors[i].z = temp[2];
				vecCuttingPlaneVectors[i].a = temp[3];

				bool bTemp = vecCuttingPlaneEnabled[i];
				std::stringstream name1;
				name1 << "Enable Plane " << i;

				ImGui::Checkbox(name1.str().c_str(), &bTemp);
				vecCuttingPlaneEnabled[i] = bTemp;
			}
			if (ImGui::Button("Add Cutting Plane"))
			{	
				nCountCuttingPlanes++;
				vecCuttingPlaneEnabled.push_back(false);
				vecCuttingPlaneVectors.push_back(glm::vec4(0, 0, 0, 0));
			}			
			ImGui::End();
			
			pFibers->SetCuttingPlaneVectors(vecCuttingPlaneEnabled, vecCuttingPlaneVectors);

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
	delete pShaderPoints;
	delete pShaderFrameBuffer;
	delete pFibers;
	delete pFrameBuffer;
	delete pSphere;
	delete pShaderSphere;
}

void main()
{
	LoadContent();
	GameLoop();
	DeleteContent();
}
