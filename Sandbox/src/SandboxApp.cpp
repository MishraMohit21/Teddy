#include <Teddy.h>

#include <imgui/imgui.h>

class ExampleLayer : public Teddy::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		if (Teddy::Input::IsKeyPressed(TD_KEY_TAB))
			TD_TRACE("Tab key is pressed (poll)!");
	}

	virtual void OnImGuiRender() override
	{

		ImGui::Begin("Test");
		ImGui::Text("Hello Test Layer");
		ImGui::End();

	}

	void OnEvent(Teddy::Event& event) override
	{
		if (event.GetEventType() == Teddy::EventType::KeyPressed)
		{
			Teddy::KeyPressedEvent& e = (Teddy::KeyPressedEvent&)event;
			if (e.GetKeyCode() == TD_KEY_TAB)
				TD_TRACE("Tab key is pressed (event)!");
			TD_TRACE("{0}", (char)e.GetKeyCode());
		}
	}

};

class Sandbox : public Teddy::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Teddy::ImGuiLayer());
	}

	~Sandbox()
	{

	}

};

Teddy::Application* Teddy::CreateApplication()
{
	return new Sandbox();
}