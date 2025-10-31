#include <Teddy.h>
#include <Teddy/Core/EntryPoint.h>

#include "Teddy/Renderer/Font.h"

#include "EditorLayer.h"


namespace Teddy
{
	class TeddEditor : public Application
	{
	public:
		TeddEditor(ApplicationCommandLineArgs args)
			: Application("Teddy Editor", args)
		{
			Font::Init();
			PushLayer(new (EditorLayer));
		}

		~TeddEditor()
		{
			Font::Shutdown();
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new TeddEditor(args);
	}

}