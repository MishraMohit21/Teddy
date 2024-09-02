#pragma once

#include "Teddy/Core/Core.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Teddy {

	class Log
	{
	public:
		static void Init();

		inline static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};

}

// Core log macros
#define TD_CORE_TRACE(...)    ::Teddy::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define TD_CORE_INFO(...)     ::Teddy::Log::GetCoreLogger()->info(__VA_ARGS__)
#define TD_CORE_WARN(...)     ::Teddy::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define TD_CORE_ERROR(...)    ::Teddy::Log::GetCoreLogger()->error(__VA_ARGS__)
#define TD_CORE_CRITICAL(...) ::Teddy::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define TD_TRACE(...)         ::Teddy::Log::GetClientLogger()->trace(__VA_ARGS__)
#define TD_INFO(...)          ::Teddy::Log::GetClientLogger()->info(__VA_ARGS__)
#define TD_WARN(...)          ::Teddy::Log::GetClientLogger()->warn(__VA_ARGS__)
#define TD_ERROR(...)         ::Teddy::Log::GetClientLogger()->error(__VA_ARGS__)
#define TD_CRITICAL(...)      ::Teddy::Log::GetClientLogger()->critical(__VA_ARGS__)
