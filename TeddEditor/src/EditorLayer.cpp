#include "EditorLayer.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Teddy/Utils/PlatformUtils.h>
#include <Teddy/Math/Math.h>

#include <ImGuizmo.h>
#include <filesystem>

namespace Teddy {

	extern const std::filesystem::path g_AssetPath;

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f), m_ViewportSize({ 1280.0f, 720.0f })
	{
	}

	void EditorLayer::OnAttach()
	{
		TD_PROFILE_FUNCTION();


		FrameBufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = FrameBuffer::Create(fbSpec);

		
		auto commandLineArgs = Application::Get().GetCommandLineArgs();
		if (commandLineArgs.Count > 1)
		{
			auto sceneFilePath = commandLineArgs[1];
			SceneSerializer serializer(m_ActiveScene);
			serializer.DeSerialize(sceneFilePath);
		}


		m_IconStop = Texture2D::Create("external/Icon/stop.png");
		m_IconPlay = Texture2D::Create("external/Icon/play.png");

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);


		


		m_ActiveScene->OnVeiwportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_EditorScene = m_ActiveScene;
		
	}

	void EditorLayer::OnDetach()
	{	
		TD_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		TD_PROFILE_FUNCTION();

		// Resize
		if (FrameBufferSpecification spec = m_Framebuffer->GetFrameBufferSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->NewSize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);

			m_ActiveScene->OnVeiwportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		

		m_EditorCamera.OnUpdate(ts);

		// Render
		Renderer2D::ResetStats();
		m_Framebuffer->bind();
		RenderCommand::SetClearColor(glm::vec4(m_CameraBackground, 1));
		RenderCommand::Clear();

		m_Framebuffer->ClearAttachmentValue(1, -1);

		

		switch (m_SceneState)
		{
		case SceneState::Edit:
		{
			if (m_ViewportFocused)
				m_CameraController.OnUpdate(ts);

			m_EditorCamera.OnUpdate(ts);

			m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
			break;
		}
		case SceneState::Play:
		{
			m_ActiveScene->OnUpdateRuntime(ts);
			break;
		}
		}
		
		auto [mx, my] = ImGui::GetMousePos();
		//TD_CORE_INFO("mx -= m_ViewportBounds[0].x: {0} -= {1}", mx, m_ViewportBounds[0].x);
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		//TD_CORE_INFO("Mouse Position: {0} {1}", mx, my);


		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		//TD_CORE_INFO("Mouse Position: {0} {1}", mouseX, mouseY);

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			//TD_CORE_INFO("Pixel data = {0}", pixelData);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		}

		m_Framebuffer->unBind();
	}

	void EditorLayer::OnImGuiRender()
	{
		TD_PROFILE_FUNCTION();

		// Note: Switch this to true to enable dockspace
		static bool dockspaceOpen = false;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 395.0f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;
		myFont = io.Fonts->Fonts[1];
		ImGui::PushFont(myFont);
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("New"))
					OnNewScene();
				ImGui::Separator();
				if (ImGui::MenuItem("Open ..."))
					OnOpenScene();
				ImGui::Separator();
				if (ImGui::MenuItem("Save As..."))
					OnSaveSceneAs();
				ImGui::Separator();
				if (ImGui::MenuItem("Exit")) Application::Get().Close();
				ImGui::EndMenu();
			}


			if (ImGui::BeginMenu("Windows"))
			{
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
				if (ImGui::MenuItem("Viewport"))
					m_ShowViewport = !m_ShowViewport;
				ImGui::Separator();

				if (ImGui::MenuItem("Stats"))
					m_ShowSettingpanel = !m_ShowSettingpanel;
				ImGui::Separator();

				if (ImGui::MenuItem("Content Browser"))
					m_ShowContentBrowser = !m_ShowContentBrowser;
				ImGui::Separator();

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();

		}
		ImGui::PopFont();
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		//ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
		//ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		//ImGui::PushStyleVar(ImGuiStyleVar_TabBorderSize, 1.0f);
		m_SceneHierarchyPanel.OnImGuiRender();
		if (m_ShowContentBrowser)
			m_ContentBrowser.OnImGuiRender(m_ShowContentBrowser);
		ImGui::PopStyleVar(); 

		if (m_ShowSettingpanel)
			ShowSettings();

		
		if (m_ShowViewport)
			ViewportRender();


		UI_Toolbar();

		ImGui::End();
	}

	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));
		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		float size = ImGui::GetWindowHeight() - 4.0f;
		Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
		if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			if (m_SceneState == SceneState::Edit)
				OnScenePlay();
			else if (m_SceneState == SceneState::Play)
				OnSceneStop();
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}




	void EditorLayer::OnEvent(Event& e)
	{
		//m_CameraController.OnEvent(e);
				m_EditorCamera.OnEvent(e);

		EventDispatcher evnDis(e);
		evnDis.Dispatch<KeyPressedEvent>(TD_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		evnDis.Dispatch<MouseButtonPressedEvent>(TD_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}



	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{

		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(KeyCode::LeftControl) || Input::IsKeyPressed(KeyCode::RightControl);
		bool shift = Input::IsKeyPressed(KeyCode::LeftShift) || Input::IsKeyPressed(KeyCode::RightShift);
		
		switch (e.GetKeyCode())
		{
			case Key::N:
				if (control)
					OnNewScene();
				break;
		
			case Key::O:
				if (control)
					OnOpenScene();
				break; 

			case Key::S:
					if (control && shift)
						OnSaveSceneAs();
					else
						SaveScene();

					break;

			case Key::Space:
				if (control)
					m_ShowContentBrowser = !m_ShowContentBrowser;

			case Key::D:
			{
				if (control)
					OnDuplicateEntity();

				break;
			}

			// Gizmos
			case Key::Q:
				m_GizmoType = -1;
				break;
			case Key::W:
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case Key::E:
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			case Key::R:
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
				break;

			
		}




		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == (int)Mouse::ButtonLeft)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
		}
		return false;
	}



	void EditorLayer::OnNewScene()
	{

		m_ActiveScene = CreateRef<Scene>("NewScene");

		m_ActiveScene->OnVeiwportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		m_EditorScenePath = std::filesystem::path();
	}

	void EditorLayer::OnOpenScene()
	{

		std::string filepath = FileDialogs::OpenFile("Teddy Scene (*.tddy)\0*.tddy\0");
		if (!filepath.empty())
		{
			OnOpenScene(filepath);
		}

	}


	void EditorLayer::SaveScene()
	{
		try
		{
			if (m_ActiveScene == nullptr)
			{
				TD_CORE_ERROR("Cannot save: No active scene exists");
				return;
			}

			// If no path exists, prompt for save location
			if (m_EditorScenePath.empty())
			{
				OnSaveSceneAs();
				return;
			}

			// Ensure directory exists
			std::filesystem::path directory = m_EditorScenePath.parent_path();
			if (!std::filesystem::exists(directory))
			{
				std::filesystem::create_directories(directory);
			}

			SerializeScene(m_ActiveScene, m_EditorScenePath);
			TD_CORE_INFO("Scene saved successfully to {0}", m_EditorScenePath.string());
		}
		catch (const std::exception& e)
		{
			TD_CORE_ERROR("Failed to save scene: {0}", e.what());
			// Optionally, show a dialog to user about save failure
		}
	}

	void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(path.string());
	}

	void EditorLayer::OnOpenScene(const std::filesystem::path& path)
	{
		try
		{
			if (m_SceneState != SceneState::Edit)
				OnSceneStop();

			// Validate file extension
			if (path.extension().string() != ".tddy")
			{
				TD_CORE_WARN("Could not load {0} - not a scene file", path.filename().string());
				return;
			}

			// Check if file exists
			if (!std::filesystem::exists(path))
			{
				TD_CORE_ERROR("Scene file does not exist: {0}", path.string());
				return;
			}

			OnNewScene();

			Ref<Scene> newScene = CreateRef<Scene>();
			SceneSerializer serializer(newScene);

			if (serializer.DeSerialize(path.string()))
			{
				m_EditorScene = newScene;
				m_EditorScene->OnVeiwportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
				m_SceneHierarchyPanel.SetContext(m_EditorScene);

				m_ActiveScene = m_EditorScene;
				m_EditorScenePath = path;

				TD_CORE_INFO("Scene loaded successfully from {0}", path.string());
			}
			else
			{
				TD_CORE_ERROR("Failed to deserialize scene from {0}", path.string());
			}
		}
		catch (const std::exception& e)
		{
			TD_CORE_ERROR("Error opening scene: {0}", e.what());
		}

	}

	void EditorLayer::OnSaveSceneAs()
	{
		try
		{
			if (m_ActiveScene == nullptr)
			{
				TD_CORE_ERROR("Cannot save: No active scene exists");
				return;
			}

			std::string filepath = FileDialogs::SaveFile("Teddy Scene (*.tddy)\0*.tddy\0");
			if (!filepath.empty())
			{
				// Ensure .tddy extension
				std::filesystem::path scenePath(filepath);
				if (scenePath.extension() != ".tddy")
				{
					scenePath += ".tddy";
				}

				SceneSerializer serializer(m_ActiveScene);
				serializer.Serialize(scenePath.string());

				m_EditorScenePath = scenePath;
				TD_CORE_INFO("Scene saved successfully to {0}", scenePath.string());
			}
		}
		catch (const std::exception& e)
		{
			TD_CORE_ERROR("Failed to save scene as: {0}", e.what());
			// Optionally, show a dialog to user about save failure
		}
	}


	void EditorLayer::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
			return;

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity)
			m_EditorScene->DuplicateEntity(selectedEntity);
	}


	void EditorLayer::ViewportRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport", &m_ShowViewport);

		ImVec2 viewportOffset = ImGui::GetCursorPos();
		//TD_CORE_WARN("Viewport Offset: ( {0}, {1} )", viewportOffset.x, viewportOffset.y);
		m_cursoePosition = ImVec2(
			ImGui::GetMousePos().x - ImGui::GetWindowPos().x,
			ImGui::GetMousePos().y - ImGui::GetWindowPos().y
		);

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvent(!m_ViewportFocused && !m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = ((const wchar_t*)payload->Data);
				std::filesystem::path fullPath(g_AssetPath);
				fullPath /= path;  // Using /= operator instead of / for path concatenation

				std::wstring extension = fullPath.extension().wstring();
				std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
				std::string filepathString = fullPath.string();
				if (extension == L".tddy") {
					OnOpenScene(fullPath);
				}
				else if (extension == L".png" || extension == L".jpg" ||
					extension == L".jpeg" || extension == L".bmp") {
					//ImGui::Dummy(ImVec2(200.0f, 200.0f));  // Space for image
					auto square = m_ActiveScene->CreateEntity("newEntity", glm::vec3(0.0f, 0.0f, 0.0f));
					const Ref<Texture2D> fileTexture = Texture2D::Create(filepathString.c_str());

					square.AddComponent<SpriteRendererComponent>(fileTexture);
					m_Elements.push_back(square);
				}
				else
				{
					TD_CORE_ASSERT(false, "No action for this file")
				}
			}
			ImGui::EndDragDropTarget();
		}

		auto windowSize = ImGui::GetWindowSize();
		ImVec2 minBound = ImGui::GetWindowPos();
		minBound.x += viewportOffset.x;
		minBound.y += viewportOffset.y;
		ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
		m_ViewportBounds[0] = { minBound.x, minBound.y };
		m_ViewportBounds[1] = { maxBound.x, maxBound.y };


		//TD_CORE_WARN("Min Bound: ( {0}, {1} )", minBound.x, minBound.y);
		//TD_CORE_WARN("Max Bound: ( {0}, {1} )", maxBound.x, maxBound.y);


		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			// Camera

			if (runGame)
			{
				// Runtime camera from entity
				auto cameraEntity = m_ActiveScene->GetPrimarySceneCamera();
				const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
				const glm::mat4& cameraProjection = camera.GetProjection();
				glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());
			}
			else
			{
			// Editor camera

				const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
				glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

				// Entity transform
				auto& tc = selectedEntity.GetComponent<TransformComponent>();
				glm::mat4 transform = tc.GetTransform();

				// Snapping
				bool snap = Input::IsKeyPressed(Key::LeftControl);
				float snapValue = 0.5f; // Snap to 0.5m for translation/scale
				// Snap to 45 degrees for rotation
				if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
					snapValue = 45.0f;
				float snapValues[3] = { snapValue, snapValue, snapValue };
				ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
					(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
					nullptr, snap ? snapValues : nullptr);
				if (ImGuizmo::IsUsing())
				{
					glm::vec3 translation, rotation, scale;
					Math::DecomposeTransform(transform, translation, rotation, scale);
					glm::vec3 deltaRotation = rotation - tc.Rotation;
					tc.Translation = translation;
					tc.Rotation += deltaRotation;
					tc.Scale = scale;
				}
			}
		}




		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::ShowSettings()
	{
		

		ImGui::Begin("Stats", &m_ShowSettingpanel);
		
		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		ImGui::Text("Cursor Position: (%d, %d)", m_cursoePosition.x, m_cursoePosition.y);
		ImGui::Separator();
		ImGui::ColorEdit3("Camera Background", glm::value_ptr(m_CameraBackground));
		if (ImGui::Button("Set Default"))
		{
			m_CameraBackground = m_CameraDefault;
		}
		
		ImGui::Separator();

		

		//if (ImGui::Button("Dark"))
		//	SetDarkThemeColors();
		//if (ImGui::Button("Light"))
		//	SetLightThemeColors();

		ImGui::End();


	}
	
	void EditorLayer::OnScenePlay()
	{
		try
		{
			m_SceneState = SceneState::Play;

			// Create a runtime copy of the current scene
			m_ActiveScene = Scene::Copy(m_EditorScene);

			// If no path exists for the original scene, create a temporary one
			if (m_EditorScenePath.empty())
			{
				m_EditorScenePath = std::filesystem::temp_directory_path() / "temp_scene.tddy";
				SerializeScene(m_EditorScene, m_EditorScenePath);
			}

			m_ActiveScene->OnRuntimeStart();
			m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		}
		catch (const std::exception& e)
		{
			TD_CORE_ERROR("Failed to start scene play: {0}", e.what());
			// Revert to edit mode if play fails
			m_SceneState = SceneState::Edit;
		}
	}

	void EditorLayer::OnSceneStop()
	{
		try
		{
			m_SceneState = SceneState::Edit;

			m_ActiveScene->OnRuntimeStop();
			m_ActiveScene = m_EditorScene;

			m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		}
		catch (const std::exception& e)
		{
			TD_CORE_ERROR("Failed to stop scene: {0}", e.what());
		}
	}


	void EditorLayer::SetDarkThemeColors()
	{

		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	}

	void EditorLayer::SetLightThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;

		// Main background
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.96f, 0.96f, 0.96f, 1.0f }; // Light gray
		colors[ImGuiCol_ChildBg] = ImVec4{ 0.92f, 0.92f, 0.92f, 1.0f };
		colors[ImGuiCol_PopupBg] = ImVec4{ 0.98f, 0.98f, 0.98f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.86f, 0.86f, 0.86f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.76f, 0.76f, 0.76f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.66f, 0.66f, 0.66f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.85f, 0.85f, 0.85f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.75f, 0.75f, 0.75f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.65f, 0.65f, 0.65f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.90f, 0.90f, 0.90f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.80f, 0.80f, 0.80f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.70f, 0.70f, 0.70f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.92f, 0.92f, 0.92f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.76f, 0.76f, 0.76f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.82f, 0.82f, 0.82f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.92f, 0.92f, 0.92f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.86f, 0.86f, 0.86f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.85f, 0.85f, 0.85f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.75f, 0.75f, 0.75f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.85f, 0.85f, 0.85f, 1.0f };

		// Separator
		colors[ImGuiCol_Separator] = ImVec4{ 0.80f, 0.80f, 0.80f, 1.0f };
		colors[ImGuiCol_SeparatorHovered] = ImVec4{ 0.70f, 0.70f, 0.70f, 1.0f };
		colors[ImGuiCol_SeparatorActive] = ImVec4{ 0.60f, 0.60f, 0.60f, 1.0f };

		// Text
		colors[ImGuiCol_Text] = ImVec4{ 0.10f, 0.10f, 0.10f, 1.0f }; // Dark text
		colors[ImGuiCol_TextDisabled] = ImVec4{ 0.50f, 0.50f, 0.50f, 1.0f };

		// Scrollbar
		colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.95f, 0.95f, 0.95f, 1.0f };
		colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.80f, 0.80f, 0.80f, 1.0f };
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.70f, 0.70f, 0.70f, 1.0f };
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.60f, 0.60f, 0.60f, 1.0f };

	}

	
}