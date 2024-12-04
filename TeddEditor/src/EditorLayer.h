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
		void OnOpenScene(const std::filesystem::path& path);
		void OnSaveSceneAs();
		//void OnSelectAll();'

	// UI Controls
	private:
		void ViewportRender();
		void ShowSettings();
		void UI_Toolbar();
			
	private:
		void SetDarkThemeColors();
		void SetLightThemeColors();


	private:
		Teddy::OrthographicCameraController m_CameraController;

		// Temp
		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;
		Ref<FrameBuffer> m_Framebuffer;
		Ref<Texture2D> m_IconPlay, m_IconStop;
		Ref<Scene> m_ActiveScene;
		
		std::vector<Entity> m_Elements;
		bool m_PrimaryCamera = true;

		EditorCamera m_EditorCamera;
		int m_GizmoType = -1;

		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowser m_ContentBrowser;
		bool m_ShowContentBrowser = true;
		bool m_ShowViewport = true;
		bool m_ShowSettingpanel = true;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];

		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
		glm::vec3 m_CameraBackground = { 0.15f, 0.1505f, 0.151f };
		glm::vec3 m_CameraDefault = { 0.098f, 0.129f, 0.164f };


		ImVec2 m_cursoePosition;
		bool runGame = false;
		Entity m_HoveredEntity;
	};

}