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
		TeddEditor()
			: Application("Teddy Editor")
		{
			// PushLayer(new ExampleLayer());
			PushLayer(new (EditorLayer));
		}

		~TeddEditor()
		{
		}
	};

	Application* CreateApplication()
	{
		return new TeddEditor();
	}

}