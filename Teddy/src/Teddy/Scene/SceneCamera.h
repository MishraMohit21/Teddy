#pragma once

#include <Teddy/Renderer/Camera.h>

namespace Teddy
{

	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType {
			Perspective, Orthographic
		};
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthograhicProjection(float Size, float NearClip, float FarClip);
		void SetPerspectiveProjection(float verticalFOV, float NearClip, float FarClip);

		void SetProjection(uint32_t width, uint32_t height);


		float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
		void SetPerspectiveVerticalFOV(float verticalFov) { m_PerspectiveFOV = verticalFov; RecalculateMatrix(); }
		float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
		void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; RecalculateMatrix(); }
		float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
		void SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; RecalculateMatrix(); }


		// OrthoGraphic
		float GetOrthoSize() const { return m_orthographicSize; }
		void SetOrthoSize(float size) { m_orthographicSize = size; RecalculateMatrix(); }
			
		float GetOrthoNearClip() const { return m_orthoNearClip; }
		void SetOrthoNearClip(float nearclip) { m_orthoNearClip = nearclip; RecalculateMatrix(); }

		float GetOrthoFarClip() const { return m_orthoFarClip; }
		void SetOrthoFarClip(float farclip) { m_orthoFarClip = farclip; RecalculateMatrix(); }

		ProjectionType GetProjection() const { return m_type; }
		void SetProjectionType(ProjectionType type) { m_type = type; }
	private:
		void RecalculateMatrix();
	private:
		ProjectionType m_type = ProjectionType::Orthographic;

		float m_PerspectiveFOV = glm::radians(45.0f);
		float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 1000.0f;


		float m_orthographicSize = 10.0f;
		float m_orthoNearClip = -1.0f, m_orthoFarClip = 10.0f;


		float m_AspectRatio = 16.0f / 9.0f;


	};

}


