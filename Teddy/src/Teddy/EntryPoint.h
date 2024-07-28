#pragma once

#ifdef TD_PLATFORM_WINDOWS

extern Teddy::Application* Teddy::CreateApplication();

int main(int argc, char** argv)
{
	Teddy::Log::Init();
	TD_CORE_WARN("Initialized Log!");
	int a = 5;
	TD_INFO("Hello! Var={0}", a);

	auto app = Teddy::CreateApplication();
	app->Run();
	delete app;
}

#endif