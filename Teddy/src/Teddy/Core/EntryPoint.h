#pragma once
#include "Teddy/Core/Core.h"

#ifdef TD_PLATFORM_WINDOWS

extern Teddy::Application* Teddy::CreateApplication();

int main(int argc, char** argv)
{
	Teddy::Log::Init();

	TD_PROFILE_BEGIN_SESSION("Startup", "Profiles/TeddyProfile-Startup.json");
	auto app = Teddy::CreateApplication();
	TD_PROFILE_END_SESSION();

	TD_PROFILE_BEGIN_SESSION("Runtime", "Profiles/TeddyProfile-Runtime.json");
	app->Run();
	TD_PROFILE_END_SESSION();

	TD_PROFILE_BEGIN_SESSION("Startup", "Profiles/TeddyProfile-Shutdown.json");
	delete app;
	TD_PROFILE_END_SESSION();
}

#endif
