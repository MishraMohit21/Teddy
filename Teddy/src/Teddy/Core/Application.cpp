#include "tdpch.h"
#include "Teddy/Core/Application.h"
#include "Teddy/Core/Log.h"
#include "Teddy/Renderer/Renderer.h"
#include "Teddy/Core/Input.h"
#include "Teddy/Scripting/ScriptingEngine.h"
#include "Teddy/Project/Project.h"
#include "Teddy/Project/ProjectSerializer.h"
#include "Teddy/Scripting/ScriptCompiler.h"

#include <glfw/glfw3.h>
#include <fstream>

namespace Teddy {

	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name, ApplicationCommandLineArgs args)
		: m_CommandLineArgs(args)
	{
		TD_PROFILE_FUNCTION();

		TD_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		WindowProps winprop;
		winprop.Title = name;
		m_Window = Window::Create(winprop);
		m_Window->SetEventCallback(TD_BIND_EVENT_FN(Application::OnEvent));

		m_LayerStack = std::make_unique<LayerStack>();

		Renderer::Init();
 		ScriptingEngine::Init();

		if (m_CommandLineArgs.Count > 1)
		{
			std::string command = m_CommandLineArgs[1];
			if (command == "--new" && m_CommandLineArgs.Count > 2)
			{
				NewProject(m_CommandLineArgs[2]);
			}
			else
			{
				OpenProject(m_CommandLineArgs[1]);
			}
		}

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	void Application::NewProject(const std::filesystem::path& path)
	{
		TD_CORE_INFO("Creating new project at: {}", path.string());

		Ref<Project> project = CreateRef<Project>();
		project->SetName(path.stem().string());
		project->SetProjectDirectory(path);
		project->SetProjectFilePath(path / (path.stem().string() + ".teddyproj"));

		std::filesystem::create_directory(path);
		std::filesystem::create_directory(path / "Assets");
		std::filesystem::create_directory(path / "Assets" / "Scenes");
		std::filesystem::create_directory(path / "Scripts");
		std::filesystem::path scriptCoreDestDir = path / "Binaries" / "ScriptCore";
		std::filesystem::create_directories(scriptCoreDestDir);

		std::filesystem::path scriptCoreSource = "Resources/Scripts/ScriptCore.dll";
		if (std::filesystem::exists(scriptCoreSource))
			std::filesystem::copy(scriptCoreSource, scriptCoreDestDir / "ScriptCore.dll");
		else
			TD_CORE_ERROR("Could not find ScriptCore.dll at {}", scriptCoreSource.string());

		std::filesystem::path scriptCorePDB_Source = "Resources/Scripts/ScriptCore.pdb";
		if (std::filesystem::exists(scriptCorePDB_Source))
			std::filesystem::copy(scriptCorePDB_Source, scriptCoreDestDir / "ScriptCore.pdb");
		else
			TD_CORE_ERROR("Could not find ScriptCore.pdb at {}", scriptCorePDB_Source.string());

		const char* defaultSceneContent = R"(Scene: main
Entities:
  - EntityId: 499784567368447835
    TagComponent:
      Tag: Square
    TransformComponent:
      Translation: [0, 0, 0]
      Rotation: [0, 0, 0]
      Scale: [1, 1, 1]
    SpriteRendererComponent:
      TintColor: [0.400000006, 0.899999976, 0.699999988, 1]
      TextureFile: 0
      TilingFactor: 1
  - EntityId: 17551659058006136178
    TagComponent:
      Tag: Camera
    TransformComponent:
      Translation: [0, 0, 0]
      Rotation: [0, 0, 0]
      Scale: [1, 1, 1]
    CameraComponent:
      Camera:
        ProjectionType: 1
        PerspectiveFOV: 0.785398185
        PerspectiveNearClip: 0.00999999978
        PerspectiveFarClip: 1000
        OrthographicSize: 10
        OrthographicNearClip: -1
        OrthographicFarClip: 1
      Primary: true
      FixedAspectRatio: false
)";
		std::ofstream mainScene(path / "Assets" / "Scenes" / "main.tddy");
		mainScene << defaultSceneContent;
		mainScene.close();

		// Configure the project properties
				mainScene.close();

		// Configure the project properties
		project->SetMainScenePath("Assets/Scenes/main.tddy");
		project->SetScriptCorePath("Binaries/ScriptCore/ScriptCore.dll");
		project->SetGameAssemblyPath("Binaries/GameAssembly.dll");
		project->SetAssetDirectory("Assets");
		project->SetReloadAssemblyOnPlay(true);
		project->AddSceneToList("Assets/Scenes/main.tddy");

		// Save the project file. This is important so the compiler can read it.
		ProjectSerializer serializer(project);
		serializer.Serialize(project->GetProjectFilePath());

		// Generate solution and compile scripts
		ScriptCompiler::Compile(project);

		// Set the new project as the active one
		Project::SetActive(project);
		
		// Load the brand new assemblies
		ScriptingEngine::LoadAssemblies(
			project->GetAbsolute(project->GetScriptCorePath()),
			project->GetAbsolute(project->GetGameAssemblyPath())
		);
	}
	

	void Application::OpenProject(const std::filesystem::path& path)
	{
		TD_CORE_INFO("Opening project: {}", path.string());
		Ref<Project> project = CreateRef<Project>();
		ProjectSerializer serializer(project);
		if (serializer.Deserialize(path)) // This sets the project's directory correctly
		{
			TD_CORE_INFO("Project deserialized successfully: {}", path.string());
			Project::SetActive(project);

			ScriptingEngine::LoadAssemblies(
				project->GetAbsolute(project->GetScriptCorePath()),
				project->GetAbsolute(project->GetGameAssemblyPath())
			);
		}
		else
		{
			TD_CORE_ERROR("Failed to deserialize project: {}", path.string());
		}
	}

	Application::~Application()
	{
		TD_PROFILE_FUNCTION();

		m_LayerStack.reset();

		ScriptingEngine::Shutdown();
		Renderer::Shutdown();
	}

	void Application::PushLayer(Layer* layer)
	{
		TD_PROFILE_FUNCTION();

		m_LayerStack->PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		TD_PROFILE_FUNCTION();

		m_LayerStack->PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::OnEvent(Event& e)
	{
		TD_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(TD_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(TD_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack->end(); it != m_LayerStack->begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Run()
	{
		TD_PROFILE_FUNCTION();

		while (m_Running)
		{
			TD_PROFILE_SCOPE("RunLoop");

			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				{
					TD_PROFILE_SCOPE("LayerStack OnUpdate");

					for (Layer* layer : *m_LayerStack)
						layer->OnUpdate(timestep);
				}

				m_ImGuiLayer->Begin();
				{
					TD_PROFILE_SCOPE("LayerStack OnImGuiRender");

					for (Layer* layer : *m_LayerStack)
						layer->OnImGuiRender();
				}
				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		TD_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

}
