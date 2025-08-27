#include "EditorLayer.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Teddy/Utils/PlatformUtils.h>
#include <Teddy/Math/Math.h>

#include <ImGuizmo.h>
#include <filesystem>
#include <Teddy/Scripting/ScriptingEngine.h>

namespace Teddy {

	extern const std::filesystem::path g_AssetPath;

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f)
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
		m_IconSimulate = Texture2D::Create("external/Icon/SimulateButton.png");


		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);


		if (!m_ActiveScene)
		{
			m_ActiveScene = CreateRef<Scene>("DefaultScene");
			m_EditorScenePath = std::filesystem::path("assets/Scenes/default_scene.tddy");

			// Add a default entity
			auto square = m_ActiveScene->CreateEntity("Square", glm::vec3(0.0f, 0.0f, 0.0f));
			square.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.4, 0.9, 0.7, 1.0 });
			
			auto camera = m_ActiveScene->CreateEntity("Camera", glm::vec3(0.0f, 0.0f, 0.0f));
			camera.AddComponent<CameraComponent>();
			SaveScene();
		}


		m_ActiveScene->OnVeiwportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_EditorScene = m_ActiveScene;
	}

	void EditorLayer::OnDetach()
	{	
		TD_PROFILE_FUNCTION();
		if (m_SceneState != SceneState::Edit)
			OnSceneStop();
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
		RenderCommand::SetClearColor(glm::vec4(m_CameraBackground, 1.0f));
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
		case SceneState::Simulate:
		{
			m_ActiveScene->OnUpdateSimuation(ts, m_EditorCamera);
			break;
		}
		case SceneState::Play:
		{
			m_ActiveScene->OnUpdateRuntime(ts);
			break;
		}
		
		}
		
		auto [mx, my] = ImGui::GetMousePos();
		//TD_CORE_INFO("m_ViewportBounds[0].x: {0}", m_ViewportBounds[0].x);
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
			pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			//TD_CORE_INFO("Pixel data = {0}", pixelData);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		}

		OnOverlayRender();


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
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0.0f, 5.0f });
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
		ImGui::PopStyleVar();
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



		ImGui::End();
	}

	void EditorLayer::UI_Toolbar()
	{
		float multi = 0.8f;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 4)); // Toolbar padding
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));  // Spacing between buttons
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);        // Rounded button corners

		// Set the button styles to be transparent with a hover effect
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.15f, 0.15f, 0.6f));

		ImGui::SetNextWindowSize(ImVec2{ 80, 38 });
		float XpositionOfToolbar = m_ViewportBounds[0].x + ((m_ViewportBounds[1].x - m_ViewportBounds[0].x) / 2);
		float YpositionOfToolbar = m_ViewportBounds[0].y + 5;
		ImGui::SetNextWindowPos(ImVec2{ XpositionOfToolbar, YpositionOfToolbar });

		// Begin a toolbar window inside the viewport
		ImGui::Begin("ViewportToolbar", nullptr,
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoTitleBar); // Add NoBackground flag

		// Center toolbar in the viewport
		float toolbarWidth = ImGui::GetContentRegionAvail().x;
		float buttonSize = 24.0f;
		float totalButtonWidth = buttonSize * 2.0 + 16.0f; // 3 buttons with spacing
		float offsetX = (toolbarWidth - totalButtonWidth) * 0.5f; // Center align
		ImGui::SetCursorPosX(offsetX);

		// Play/Stop Button
		Ref<Texture2D> playStopIcon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
			? m_IconPlay
			: m_IconStop;
		if (ImGui::ImageButton((ImTextureID)playStopIcon->GetRendererID(), ImVec2(buttonSize, buttonSize)))
		{
			if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
				OnScenePlay();
			else if (m_SceneState == SceneState::Play)
				OnSceneStop();
		}
		ImGui::SameLine();

		// Simulate Button
		Ref<Texture2D> simulateIcon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
			? m_IconSimulate
			: m_IconStop;
		if (ImGui::ImageButton((ImTextureID)simulateIcon->GetRendererID(), ImVec2(buttonSize, buttonSize)))
		{
			if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
				OnSceneSimulate();
			else if (m_SceneState == SceneState::Simulate)
				OnSceneStop();
		}

		// End toolbar window
		ImGui::PopStyleColor(4);
		ImGui::PopStyleVar(5);
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
					else if (control)
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
		m_Elements.clear();
		Renderer2D::Shutdown();
		Renderer2D::Init();
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

			m_Elements.clear();
			Renderer2D::Shutdown();
			Renderer2D::Init();

			Ref<Scene> newScene = CreateRef<Scene>();
			SceneSerializer serializer(newScene);

			if (serializer.DeSerialize(path.string()))
			{
				m_ActiveScene = newScene;
				m_EditorScene = m_ActiveScene;

				m_ActiveScene->OnVeiwportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
				m_SceneHierarchyPanel.SetContext(m_ActiveScene);
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


		UI_Toolbar();


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
					extension == L".jpeg" || extension == L".bmp")
				{
					// Create the texture from the file
					const Ref<Texture2D> fileTexture = Texture2D::Create(filepathString.c_str());
					if (m_HoveredEntity != Entity())
					{
						if (m_HoveredEntity.HasComponent<SpriteRendererComponent>())
						{
							auto& src = m_HoveredEntity.GetComponent<SpriteRendererComponent>();
							src.c_texture = fileTexture;
						}
					}
					else
					{

						// Extract the file name without the extension
						std::string entityName = fullPath.stem().string(); // `stem()` gives the file name without extension

						// Create a new entity with the extracted name
						auto square = m_ActiveScene->CreateEntity(entityName, glm::vec3(0.0f, 0.0f, 0.0f));


						// Add a SpriteRendererComponent with the texture
						square.AddComponent<SpriteRendererComponent>(fileTexture);
						m_Elements.push_back(square);
					}
				}
				else
				{
					TD_CORE_WARN("No action for this file");
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

			if (m_SceneState == SceneState::Play)
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
		ImGui::Checkbox("Show physics colliders", &m_ShowPhysicsColliders);
		ImGui::Separator();
		if (ImGui::Button("Dark"))
			SetDarkThemeColors();
		ImGui::SameLine();
		if (ImGui::Button("Light"))
			SetLightThemeColors();
		ImGui::Separator();
		ImGui::DragFloat2("Menubar Padding", menubar);
		ImGui::Separator();

		ImGui::End();


	}
	
	void EditorLayer::OnScenePlay()
	{
		try
		{
			if (m_SceneState == SceneState::Simulate)
				OnSceneStop();

			m_SceneState = SceneState::Play;

			m_ActiveScene = Scene::Copy(m_EditorScene);
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
			if (m_SceneState == SceneState::Play)
				m_ActiveScene->OnRuntimeStop();
			else if (m_SceneState == SceneState::Simulate)
				m_ActiveScene->OnSimulationStop();

			m_SceneState = SceneState::Edit;

			m_ActiveScene = m_EditorScene;

			m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		}
		catch (const std::exception& e)
		{
			TD_CORE_ERROR("Failed to stop scene: {0}", e.what());
		}
	}

	void EditorLayer::OnSceneSimulate()
	{
		try
		{
			if (m_SceneState == SceneState::Play)
				OnSceneStop();
			m_SceneState = SceneState::Simulate;
			m_ActiveScene = Scene::Copy(m_EditorScene);
			m_ActiveScene->OnSimulationStart();
			m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		}
		catch (const std::exception& e)
		{
			TD_CORE_ERROR("Failed to stop scene: {0}", e.what());
		}


	}

	void EditorLayer::OnOverlayRender()
	{
		if (m_SceneState == SceneState::Play)
		{
			Entity camera = m_ActiveScene->GetPrimarySceneCamera();
			Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());
		}
		else
		{
			Renderer2D::BeginScene(m_EditorCamera);
		}
		if (m_ShowPhysicsColliders)
		{
			// Box Colliders
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, Box2DColliderComponent>();
				for (auto entity : view)
				{
					auto [tc, bc2d] = view.get<TransformComponent, Box2DColliderComponent>(entity);
					glm::vec3 translation = tc.Translation + glm::vec3(bc2d.Offset, 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
						* glm::scale(glm::mat4(1.0f), scale);
					Renderer2D::DrawRect(transform, glm::vec4(0, 1, 0, 1));
				}
			}
			// Circle Colliders
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, Circle2DColliderComponent>();
				for (auto entity : view)
				{
					auto [tc, cc2d] = view.get<TransformComponent, Circle2DColliderComponent>(entity);
					glm::vec3 translation = tc.Translation + glm::vec3(cc2d.Offset, 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.0f);
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::scale(glm::mat4(1.0f), scale);
					Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.05f);
				}
			}
		}

		if (Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity()) {
			TransformComponent transform = selectedEntity.GetComponent<TransformComponent>();
			//Red
			Renderer2D::SetLineWidth(4.0f);
			Renderer2D::DrawRect(transform.GetTransform(), glm::vec4(1, 1, 1, 1));
		}

		Renderer2D::EndScene();
	}


	void EditorLayer::SetDarkThemeColors() {
		auto& colors = ImGui::GetStyle().Colors;

		// --- Background Colors ---
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.03f, 0.03f, 0.04f, 1.0f };
		colors[ImGuiCol_ChildBg] = ImVec4{ 0.03f, 0.03f, 0.04f, 1.0f };
		colors[ImGuiCol_PopupBg] = ImVec4{ 0.03f, 0.03f, 0.04f, 1.0f };
		
		m_CameraBackground = { 0.03f, 0.03f, 0.04f };

		// --- Header Colors ---
		colors[ImGuiCol_Header] = ImVec4{ 0.08f, 0.08f, 0.1f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.12f, 0.12f, 0.15f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.05f, 0.05f, 0.07f, 1.0f };

		// --- Button Colors ---
		colors[ImGuiCol_Button] = ImVec4{ 0.1f, 0.1f, 0.12f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.0f, 0.2f, 0.4f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.0f, 0.15f, 0.3f, 1.0f };

		// --- Frame Background Colors ---
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.08f, 0.08f, 0.1f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.15f, 0.15f, 0.2f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.05f, 0.05f, 0.07f, 1.0f };

		// --- Tab Colors ---
		colors[ImGuiCol_Tab] = ImVec4{ 0.07f, 0.07f, 0.09f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.3f, 0.1f, 0.4f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.2f, 0.05f, 0.3f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.04f, 0.04f, 0.05f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.08f, 0.08f, 0.1f, 1.0f };

		// --- Title Bar Colors ---
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.07f, 0.07f, 0.09f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.07f, 0.07f, 0.09f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.04f, 0.04f, 0.05f, 1.0f };

		// --- Separator Colors ---
		colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);

		// --- Text Colors ---
		colors[ImGuiCol_Text] = ImVec4{ 0.8f, 0.8f, 0.82f, 1.0f };
		colors[ImGuiCol_CheckMark] = ImVec4{ 0.0f, 0.6f, 1.0f, 1.0f };
		colors[ImGuiCol_SliderGrab] = ImVec4{ 0.0f, 0.4f, 0.8f, 1.0f };
		colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.0f, 0.3f, 0.7f, 1.0f };

		// --- Menu Bar Colors ---
		colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.03f, 0.03f, 0.04f, 1.0f };
		colors[ImGuiCol_Border] = ImVec4{ 0.1f, 0.1f, 0.12f, 1.0f };


		// --- Style Settings ---
		auto& style = ImGui::GetStyle();
		style.WindowRounding = 3.0f;
		style.FrameRounding = 2.0f;
		style.GrabRounding = 2.0f;
		style.WindowPadding = ImVec2(8.0f, 8.0f);
		style.FramePadding = ImVec2(5.0f, 3.0f);
	}


	void EditorLayer::SetLightThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;

		m_CameraBackground = { 0.94f, 0.94f, 0.94f };

		// Main background
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.94f, 0.94f, 0.94f, 1.0f }; // Lighter gray background
		colors[ImGuiCol_ChildBg] = ImVec4{ 0.92f, 0.92f, 0.92f, 1.0f };
		colors[ImGuiCol_PopupBg] = ImVec4{ 0.98f, 0.98f, 0.98f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.86f, 0.86f, 0.86f, 1.0f }; // Slightly darker header
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
		colors[ImGuiCol_Text] = ImVec4{ 0.10f, 0.10f, 0.10f, 1.0f }; // Dark text for contrast
		colors[ImGuiCol_TextDisabled] = ImVec4{ 0.50f, 0.50f, 0.50f, 1.0f };

		// Scrollbar
		colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.95f, 0.95f, 0.95f, 1.0f };
		colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.80f, 0.80f, 0.80f, 1.0f };
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.70f, 0.70f, 0.70f, 1.0f };
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.60f, 0.60f, 0.60f, 1.0f };
	}

	
}