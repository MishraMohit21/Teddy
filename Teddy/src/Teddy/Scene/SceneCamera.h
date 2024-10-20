#pragma once

#include <Teddy/Renderer/Camera.h>

namespace Teddy
{

	class SceneCamera : public Camera
	{
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthograhicProjection(float Size, float NearClip, float FarClip);

		void SetProjection(uint32_t width, uint32_t height);

		float GetOrthoSize() const { return m_orthographicSize; }
		void SetOrthoSize(float size) { m_orthographicSize = size; RecalculateMatrix(); }
			 
	private:
		void RecalculateMatrix();
	private:
		
		float m_orthographicSize = 10.0f;
		float m_orthoNearClip = -1.0f, m_orthoFarClip = 10.0f;


		float m_AspectRatio = 16.0f / 9.0f;


	};

}


