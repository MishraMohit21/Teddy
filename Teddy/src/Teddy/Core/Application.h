#pragma once

#include "Core.h"

#include "Window.h"
#include "Teddy/Core/LayerStack.h"

#include "Teddy/Events/Event.h"
#include "Teddy/Events/ApplicationEvent.h"

#include "Teddy/Core/Timestep.h"

#include "Teddy/ImGui/ImGuiLayer.h"

namespace Teddy {

	class Application
	{
	public:
		Application();
		virtual ~Application() = default;

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

		bool m_Minimized = 0;
	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}