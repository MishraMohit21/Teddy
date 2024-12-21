#pragma once
#include "Teddy/Core/Core.h"
#include "Teddy/Core/Application.h"

#ifdef TD_PLATFORM_WINDOWS

extern Teddy::Application* Teddy::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	Teddy::Log::Init();

	TD_PROFILE_BEGIN_SESSION("Startup", "Profiles/TeddyProfile-Startup.json");
	auto app = Teddy::CreateApplication({argc, argv});
	TD_PROFILE_END_SESSION();

	TD_PROFILE_BEGIN_SESSION("Runtime", "Profiles/TeddyProfile-Runtime.json");
	app->Run();
	TD_PROFILE_END_SESSION();

	TD_PROFILE_BEGIN_SESSION("Shutdown", "Profiles/TeddyProfile-Shutdown.json");
	delete app;
	TD_PROFILE_END_SESSION();

	//system("PAUSE");

}

#endif
