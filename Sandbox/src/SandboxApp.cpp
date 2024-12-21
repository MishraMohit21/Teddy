#include <Teddy.h>
#include <Teddy/Core/EntryPoint.h>

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"



class Sandbox : public Teddy::Application
{
public:
	Sandbox(Teddy::ApplicationCommandLineArgs args)
		: Application("SandBox Appplication")
	{
		// PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{
	}
};

Teddy::Application* Teddy::CreateApplication(ApplicationCommandLineArgs args)
{
	return new Sandbox(args);
}