#pragma once

#include "Core.h"

#include "Window.h"
#include "Teddy/LayerStack.h"
#include "Teddy/ImGui/ImGuiLayer.h"
#include "Teddy/Events/Event.h"
#include "Teddy/Events/ApplicationEvent.h"
#include "Teddy/Renderer/Shader.h"
#include "Teddy/Renderer/Buffer.h"

namespace Teddy {

	class TEDDY_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;

		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		unsigned int m_VertexArray;
	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}