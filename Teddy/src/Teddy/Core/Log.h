#pragma once

#include "Teddy/Core/Core.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

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



#ifdef TD_DEBUG
#define TD_ENABLE_ASSERTS
#endif

#ifdef TD_ENABLE_ASSERTS
#define TD_ASSERT(x, ...) { if(!(x)) { TD_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define TD_CORE_ASSERT(x, ...) { if(!(x)) { TD_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define TD_ASSERT(x, ...)
#define TD_CORE_ASSERT(x, ...)
#endif