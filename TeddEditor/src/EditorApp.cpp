#include <Teddy.h>
#include <Teddy/Core/EntryPoint.h>

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "EditorLayer.h"


namespace Teddy
{
	class TeddEditor : public Application
	{
	public:
		TeddEditor(ApplicationCommandLineArgs args)
			: Application("Teddy Editor", args)
		{
			// PushLayer(new ExampleLayer());
			PushLayer(new (EditorLayer));
		}

		~TeddEditor()
		{
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new TeddEditor(args);
	}

}