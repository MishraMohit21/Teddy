#pragma once

#include "Teddy.h"
#include "./Panels/SceneHeirarchyPanel.h"
#include "Panels/ContentBrowser.h"


namespace Teddy {

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
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);


		void OnNewScene();
		void OnOpenScene();
		void OnSaveSceneAs();
	private:
		Teddy::OrthographicCameraController m_CameraController;

		// Temp
		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;
		Ref<FrameBuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity;
		Entity m_SecondSquare;
		Entity m_SecondCamera;
		std::vector<Entity> m_Elements;
		bool m_PrimaryCamera = true;

		EditorCamera m_EditorCamera;
		int m_GizmoType = -1;

		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowser m_ContentBrowser;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];

		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
		glm::vec3 m_CameraBackground = { 0.15f, 0.1505f, 0.151f };
		glm::vec3 m_CameraDefault = { 0.15f, 0.1505f, 0.151f };

		bool runGame = false;
		Entity m_HoveredEntity;
	};

}