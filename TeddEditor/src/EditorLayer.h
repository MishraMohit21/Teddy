#pragma once

#include "Teddy.h"

namespace Teddy
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;
	private:
		OrthographicCameraController m_CameraController;

		// Temp
		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;

		Ref<Texture2D> m_CheckerboardTexture;
		Ref<Texture2D> m_SpriteSheet;
		Ref<FrameBuffer> m_FrameBuffer;

		bool m_ViewportFoc = false, m_ViewportHov = false;

		glm::vec2 m_ViewPortSize = { 0.0f, 0.0f };

		float m_RotationAngle = 45.0;
		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };


	};

}