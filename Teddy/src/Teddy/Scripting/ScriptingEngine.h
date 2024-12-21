#pragma once


namespace Teddy
{
	class ScriptingEngine
	{
	public:
		static void Init();
		static void Shutdown();
	private:
		static void InitMono();
		static void ShutdownMono();
	};
}