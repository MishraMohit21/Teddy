#include "tdpch.h"
#include "SceneCamera.h"
#include <glm/gtc/matrix_transform.hpp>

Teddy::SceneCamera::SceneCamera()
{
	RecalculateMatrix();

}

void Teddy::SceneCamera::SetOrthograhicProjection(float Size, float NearClip, float FarClip)
{
	m_orthographicSize = Size;
	m_orthoFarClip = FarClip;
	m_orthoNearClip = NearClip; RecalculateMatrix();
}

void Teddy::SceneCamera::SetProjection(uint32_t width, uint32_t height)
{
	m_AspectRatio = (float)width / (float)height;
	RecalculateMatrix();
}

void Teddy::SceneCamera::RecalculateMatrix()
{
	float orthoLeft = -m_orthographicSize * m_AspectRatio * 0.5;
	float orthoRight = m_orthographicSize * m_AspectRatio * 0.5;
	float orthoTop = m_orthographicSize * 0.5;
	float orthoBottom = -m_orthographicSize * 0.5;

	m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_orthoNearClip, m_orthoFarClip);

}

