#include "Camera.h"
#include <glm\gtx\transform.hpp>

CCamera::CCamera()
{
}

CCamera::CCamera(glm::vec3 EyePosition, glm::vec3 LookAtVector, glm::vec3 UpVector, float fFieldOfView, float fAspectRatio, float fNearClippingplane, float fFarClippingPlane)
{
	SetPerspective(fFieldOfView, fAspectRatio, fNearClippingplane, fFarClippingPlane);
	SetOrientation(EyePosition, LookAtVector, UpVector);
}

CCamera::~CCamera()
{
}

void CCamera::SetPerspective(float fFieldOfView, float fAspectRatio, float fNearClippingplane, float fFarClippingPlane)
{
	m_mProjectionMatrix = glm::perspective(fFieldOfView * 3.1415926f / 180.0f, fAspectRatio, fNearClippingplane, fFarClippingPlane);
}

void CCamera::SetOrthographic(float fLeft, float fRight, float fBottom, float fTop, float fNear, float fFar)
{
	m_mProjectionMatrix = glm::ortho(fLeft, fRight, fBottom, fTop, fNear, fFar);
}

void CCamera::SetOrientation(glm::vec3 EyePosition, glm::vec3 LookAtVector, glm::vec3 UpVector)
{
	m_mViewMatrix = glm::lookAt(EyePosition, LookAtVector, UpVector);
}

void CCamera::Translate(glm::vec3 vDirection)
{
	m_mViewMatrix = glm::translate(vDirection) * m_mViewMatrix;
}

void CCamera::Rotate(glm::vec3 vAxis, float fAngle)
{
	m_mViewMatrix = glm::rotate(fAngle, vAxis) * m_mViewMatrix;
}

glm::mat4 CCamera::GetViewMatrix()
{
	return m_mViewMatrix;
}

glm::mat4 CCamera::GetProjectionMatrix()
{
	return m_mProjectionMatrix;
}

glm::mat4 CCamera::GetViewProjectionMatrix()
{
	return m_mProjectionMatrix * m_mViewMatrix;
}

glm::vec3 CCamera::GetCameraPosition()
{
	glm::mat4 modelViewT = glm::transpose(m_mViewMatrix);

	// Get plane normals 
	glm::vec3 n1(modelViewT[0]);
	glm::vec3 n2(modelViewT[1]);
	glm::vec3 n3(modelViewT[2]);

	// Get plane distances
	float d1(modelViewT[0].w);
	float d2(modelViewT[1].w);
	float d3(modelViewT[2].w);

	glm::vec3 n2n3 = glm::cross(n2, n3);
	glm::vec3 n3n1 = glm::cross(n3, n1);
	glm::vec3 n1n2 = glm::cross(n1, n2);

	glm::vec3 top = (n2n3 * d1) + (n3n1 * d2) + (n1n2 * d3);
	float denom = dot(n1, n2n3);

	return top / -denom;
}
