#include "EditorLayer.h"
#include "Panels/ProjectBrowser.h"
#include "Teddy/Project/Project.h"
#include "Teddy/Project/ProjectSerializer.h"
#include "Teddy/Scripting/ScriptCompiler.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Teddy/Utils/PlatformUtils.h>
#include <Teddy/Math/Math.h>

#include <ImGuizmo.h>
#include <filesystem>
#include <Teddy/Scripting/ScriptingEngine.h>

namespace Teddy {

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f)
	{
	}

	void EditorLayer::OnAttach()
	{
		TD_PROFILE_FUNCTION();

		m_EditorState = EditorState::Welcome;

		m_ProjectBrowser.SetOpenProjectCallback([this](const std::filesystem::path& path) { LoadProject(path); });

		FrameBufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = FrameBuffer::Create(fbSpec);

		auto commandLineArgs = Application::Get().GetCommandLineArgs();
		if (commandLineArgs.Count > 1)
		{
			LoadProject(commandLineArgs[1]);
		}

		m_IconStop = Texture2D::Create("external/Icon/stop.png");
		m_IconPlay = Texture2D::Create("external/Icon/play.png");
		m_IconSimulate = Texture2D::Create("external/Icon/SimulateButton.png");

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
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

		if (m_EditorState == EditorState::Editor)
		{
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
			mx -= m_ViewportBounds[0].x;
			my -= m_ViewportBounds[0].y;

			glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
			my = viewportSize.y - my;
			int mouseX = (int)mx;
			int mouseY = (int)my;

			if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
			{
				pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
				m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
			}

			OnOverlayRender();

			m_Framebuffer->unBind();
		}
	}

	void EditorLayer::OnImGuiRender()
	{
		TD_PROFILE_FUNCTION();

		switch (m_EditorState)
		{
		case EditorState::Welcome:
		{
			// Only render project browser in Welcome state
			bool show = true;
			m_ProjectBrowser.OnImGuiRender(&show);
			break;
		}
		case EditorState::Editor:
		{
			static bool dockspaceOpen = true;
			static bool opt_fullscreen_persistant = true;
			bool opt_fullscreen = opt_fullscreen_persistant;
			static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

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

			if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
				window_flags |= ImGuiWindowFlags_NoBackground;

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
			ImGui::PopStyleVar();

			if (opt_fullscreen)
				ImGui::PopStyleVar(2);

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
					if (ImGui::MenuItem("New Project..."))
						OnNewProject();

					if (ImGui::MenuItem("Open Project..."))
						OnOpenProject();

					ImGui::Separator();

					if (ImGui::MenuItem("New Scene", "Ctrl+N"))
						OnNewScene();

					if (ImGui::MenuItem("Open Scene...", "Ctrl+O"))
						OnOpenScene();

					if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
						OnSaveSceneAs();

					ImGui::Separator();

					if (ImGui::MenuItem("Exit")) Application::Get().Close();
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Windows"))
				{
					if (ImGui::MenuItem("Viewport"))
						m_ShowViewport = !m_ShowViewport;
					ImGui::Separator();

					if (ImGui::MenuItem("Stats"))
						m_ShowSettingpanel = !m_ShowSettingpanel;
					ImGui::Separator();

					if (ImGui::MenuItem("Content Browser"))
						m_ShowContentBrowser = !m_ShowContentBrowser;
					ImGui::Separator();

					if (ImGui::MenuItem("Project Browser"))
						m_ShowProjectBrowser = !m_ShowProjectBrowser;
					ImGui::Separator();

					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
			ImGui::PopStyleVar();
			ImGui::PopFont();

			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
			m_SceneHierarchyPanel.OnImGuiRender();
			m_PropertiesPanel.SetContext(m_SceneHierarchyPanel.GetSelectedEntity());
			m_PropertiesPanel.OnImGuiRender();
			if (m_ShowContentBrowser)
				m_ContentBrowser.OnImGuiRender(m_ShowContentBrowser);

			// Only render project browser if explicitly requested in Editor state
			if (m_ShowProjectBrowser)
				m_ProjectBrowser.OnImGuiRender(&m_ShowProjectBrowser);
			ImGui::PopStyleVar();

			if (m_ShowSettingpanel)
				ShowSettings();

			if (m_ShowViewport)
				ViewportRender();

			ImGui::End();
			break;
		}
		}
	}

	void EditorLayer::UI_Toolbar()
	{
		float multi = 0.8f;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 4)); // Toolbar padding
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));  // Spacing between buttons
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);        // Rounded button corners

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.15f, 0.15f, 0.6f));

		ImGui::SetNextWindowSize(ImVec2{ 80, 38 });
		float XpositionOfToolbar = m_ViewportBounds[0].x + ((m_ViewportBounds[1].x - m_ViewportBounds[0].x) / 2);
		float YpositionOfToolbar = m_ViewportBounds[0].y + 5;
		ImGui::SetNextWindowPos(ImVec2{ XpositionOfToolbar, YpositionOfToolbar });

		ImGui::Begin("ViewportToolbar", nullptr,
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoTitleBar);

		float toolbarWidth = ImGui::GetContentRegionAvail().x;
		float buttonSize = 24.0f;
		float totalButtonWidth = buttonSize * 2.0 + 16.0f;
		float offsetX = (toolbarWidth - totalButtonWidth) * 0.5f;
		ImGui::SetCursorPosX(offsetX);

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

		ImGui::PopStyleColor(4);
		ImGui::PopStyleVar(5);
		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(TD_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(TD_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
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

	void EditorLayer::OnNewProject()
	{
		// This will be handled by the project browser now
	}

	void EditorLayer::OnOpenProject()
	{
		// This will be handled by the project browser now
	}

	void EditorLayer::LoadProject(const std::filesystem::path& path)
	{
		Application::Get().OpenProject(path);
		m_ContentBrowser.SetContext(Project::GetActive());

		// Script Compilation
		if (ScriptCompiler::IsSolutionReady(Project::GetActive()))
		{
			if (!ScriptCompiler::Compile(Project::GetActive()))
			{
				TD_CORE_ERROR("Script compilation failed!");
			}
			else
			{
				ScriptingEngine::ReloadGameAssembly();
			}
		}
		else
		{
			TD_CORE_WARN("Script solution not found for this project.");
		}

		OnOpenScene(Project::GetActive()->GetAbsolute(Project::GetActive()->GetMainScenePath()));

		// Transition to Editor state and close project browser
		m_EditorState = EditorState::Editor;
		m_ShowProjectBrowser = false;  // Explicitly set to false
	}

	void EditorLayer::OnNewScene()
	{
		m_ContentBrowser.ShowNewScenePopup();
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

			if (m_EditorScenePath.empty())
			{
				OnSaveSceneAs();
				return;
			}

			std::filesystem::path directory = m_EditorScenePath.parent_path();
			if (!std::filesystem::exists(directory))
			{
				std::filesystem::create_directories(directory);
			}

			SerializeScene(m_ActiveScene, m_EditorScenePath);
			TD_CORE_INFO("Scene saved successfully to {0}", m_EditorScenePath.string());

			RegisterSceneInProject(m_EditorScenePath);
		}
		catch (const std::exception& e)
		{
			TD_CORE_ERROR("Failed to save scene: {0}", e.what());
		}
	}

	void EditorLayer::RegisterSceneInProject(const std::filesystem::path& scenePath)
	{
		auto project = Project::GetActive();
		if (!project)
			return;

		std::filesystem::path relativeScenePath = std::filesystem::relative(scenePath, project->GetProjectDirectory());

		// Disallow saving scenes outside of the project directory
		if (relativeScenePath.string().find("..") != std::string::npos)
		{
			TD_CORE_ERROR("Scene must be saved inside the project directory!");
			return;
		}

		const auto& sceneList = project->GetSceneList();
		bool sceneExists = false;
		for (const auto& existingScene : sceneList)
		{
			if (existingScene == relativeScenePath)
			{
				sceneExists = true;
				break;
			}
		}

		if (!sceneExists)
		{
			project->AddSceneToList(relativeScenePath);
			ProjectSerializer projectSerializer(project);
			projectSerializer.Serialize(project->GetProjectFilePath());
			TD_CORE_INFO("Added new scene '{0}' to project.", relativeScenePath.string());
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

			TD_CORE_INFO("Attempting to open scene: {0}", path.string());
			TD_CORE_INFO("Scene file extension: {0}", path.extension().string());
			if (path.extension().string() != ".tddy")
			{
				TD_CORE_WARN("Could not load {0} - not a scene file", path.filename().string());
				return;
			}

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
				std::filesystem::path scenePath(filepath);
				if (scenePath.extension() != ".tddy")
				{
					scenePath += ".tddy";
				}

				SerializeScene(m_ActiveScene, scenePath);

				m_EditorScenePath = scenePath;
				TD_CORE_INFO("Scene saved successfully to {0}", scenePath.string());

				RegisterSceneInProject(scenePath);
			}
		}
		catch (const std::exception& e)
		{
			TD_CORE_ERROR("Failed to save scene as: {0}", e.what());
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
				std::filesystem::path fullPath = Project::GetActive()->GetAssetDirectory();
				fullPath /= path;

				std::wstring extension = fullPath.extension().wstring();
				std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
				std::string filepathString = fullPath.string();

				if (extension == L".tddy") {
					OnOpenScene(fullPath);
				}
				else if (extension == L".png" || extension == L".jpg" ||
					extension == L".jpeg" || extension == L".bmp")
				{
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

						std::string entityName = fullPath.stem().string();

						auto square = m_ActiveScene->CreateEntity(entityName, glm::vec3(0.0f, 0.0f, 0.0f));

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

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			if (m_SceneState == SceneState::Play)
			{
				auto cameraEntity = m_ActiveScene->GetPrimarySceneCamera();
				const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
				const glm::mat4& cameraProjection = camera.GetProjection();
				glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());
			}
			else
			{
				const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
				glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

				auto& tc = selectedEntity.GetComponent<TransformComponent>();
				glm::mat4 transform = tc.GetTransform();

				bool snap = Input::IsKeyPressed(Key::LeftControl);
				float snapValue = 0.5f;
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

			// Script Compilation
			if (ScriptCompiler::IsSolutionReady(Project::GetActive()))
			{
				if (!ScriptCompiler::Compile(Project::GetActive()))
				{
					TD_CORE_ERROR("Script compilation failed!");
					return;
				}
				ScriptingEngine::ReloadGameAssembly();
			}

			m_SceneState = SceneState::Play;

			m_ActiveScene = Scene::Copy(m_EditorScene);
			m_ActiveScene->OnVeiwportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_ActiveScene->OnRuntimeStart();

			m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		}
		catch (const std::exception& e)
		{
			TD_CORE_ERROR("Failed to start scene play: {0}", e.what());
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
			Renderer2D::SetLineWidth(4.0f);
			Renderer2D::DrawRect(transform.GetTransform(), glm::vec4(1, 1, 1, 1));
		}

		Renderer2D::EndScene();
	}



}
