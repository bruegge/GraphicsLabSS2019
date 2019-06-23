#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CCamera
{
public:
	CCamera();
	CCamera(glm::vec3 EyePosition, glm::vec3 LookAtVector, glm::vec3 UpVector, float fFieldOfView, float fAspectRatio, float fNearClippingplane, float fFarClippingPlane);
	~CCamera();

	void SetOrientation(glm::vec3 EyePosition, glm::vec3 LookAtVector, glm::vec3 UpVector);
	void SetPerspective(float fFieldOfView, float fAspectRatio, float fNearClippingplane, float fFarClippingPlane);
	void SetOrthographic(float fLeft, float fRight, float fBottom, float fTop, float fNear, float fFar);

	void Translate(glm::vec3 vDirection);
	void Rotate(glm::vec3 vAxis, float fAngle);
	
	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetViewProjectionMatrix();
	glm::vec3 GetCameraPosition();

private:
	glm::mat4 m_mProjectionMatrix;
	glm::mat4 m_mViewMatrix;
	float m_fCameraSpeed;
};

