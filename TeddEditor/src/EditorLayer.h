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
		void SaveScene();
		void OnSaveSceneAs();
		void OnDuplicateEntity();

		//void OnSelectAll();'

		void OnScenePlay();
		void OnSceneStop();
		void OnSceneSimulate();
		void OnOverlayRender();

	// UI Controls
	private:
		void ViewportRender();
		void ShowSettings();
		void UI_Toolbar();
		ImFont* myFont;

	private:
		void SetDarkThemeColors();
		void SetLightThemeColors();

		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

	private:

		OrthographicCameraController m_CameraController;
		// Temp
		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;
		Ref<FrameBuffer> m_Framebuffer;
		Ref<Texture2D> m_IconPlay, m_IconStop, m_IconSimulate;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;

		std::vector<Entity> m_Elements;
		bool m_PrimaryCamera = true;

		EditorCamera m_EditorCamera;
		int m_GizmoType = -1;
		bool m_ShowPhysicsColliders = false;
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowser m_ContentBrowser;
		bool m_ShowContentBrowser = true;
		bool m_ShowViewport = true;
		bool m_ShowSettingpanel = true;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 1240.0f, 720.0f };
		glm::vec2 m_ViewportBounds[2];

		glm::vec3 m_CameraBackground =  { 0.529f, 0.808f, 0.922f };
		glm::vec3 m_CameraDefault =		{ 0.529f, 0.808f, 0.922f };

		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2,
		};
		SceneState m_SceneState = SceneState::Edit;


		ImVec2 m_cursoePosition;
		Entity m_HoveredEntity;
		int pixelData;
	};

}