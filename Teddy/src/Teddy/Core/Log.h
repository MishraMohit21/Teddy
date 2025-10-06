
#pragma once

#include "Core.h"
#include <iostream>
#include <string>
#include <memory>
#include <format>

namespace Teddy {

	class Log
	{
	public:
		static void Init();

		static void Print(const std::string& message);
		static void Print(const char* message);

		template<typename... Args>
		static void Print(std::format_string<Args...> format, Args&&... args)
		{
			Print(std::format(format, std::forward<Args>(args)...));
		}
	};

}

// Core log macros
#define TD_CORE_TRACE(...)    ::Teddy::Log::Print(__VA_ARGS__)
#define TD_CORE_INFO(...)     ::Teddy::Log::Print(__VA_ARGS__)
#define TD_CORE_WARN(...)     ::Teddy::Log::Print(__VA_ARGS__)
#define TD_CORE_ERROR(...)    ::Teddy::Log::Print(__VA_ARGS__)
#define TD_CORE_FATAL(...)    ::Teddy::Log::Print(__VA_ARGS__)
#define TD_CORE_LOG(...)      ::Teddy::Log::Print(__VA_ARGS__)
#define TD_CORE_CRITICAL(...) ::Teddy::Log::Print(__VA_ARGS__)


// Client log macros
#define TD_TRACE(...)         ::Teddy::Log::Print(__VA_ARGS__)
#define TD_INFO(...)          ::Teddy::Log::Print(__VA_ARGS__)
#define TD_WARN(...)          ::Teddy::Log::Print(__VA_ARGS__)
#define TD_ERROR(...)         ::Teddy::Log::Print(__VA_ARGS__)
#define TD_FATAL(...)         ::Teddy::Log::Print(__VA_ARGS__)
#define TD_LOG(...)           ::Teddy::Log::Print(__VA_ARGS__)
#define TD_CRITICAL(...)      ::Teddy::Log::Print(__VA_ARGS__)


// Assert macros
#ifdef TD_ENABLE_ASSERTS
	#define TD_ASSERT(x, ...) { if(!(x)) { TD_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define TD_CORE_ASSERT(x, ...) { if(!(x)) { TD_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define TD_ASSERT(x, ...)
	#define TD_CORE_ASSERT(x, ...)
#endif
