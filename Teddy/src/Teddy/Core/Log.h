#pragma once

#include "Core.h"
#include <iostream>
#include <string>
#include <memory>
#include <format>
#include <chrono>
#include <iomanip>

namespace Teddy {

	class Log
	{
	public:
		enum class Level
		{
			Trace, Info, Warn, Error, Critical
		};

		static void Init() {}

		template<typename... Args>
		inline static void CoreLog(Level level, std::format_string<Args...> format, Args&&... args)
        {
            LogMessage("TD_CORE", level, std::vformat(format.get(), std::make_format_args(args...)));
        }
		static void CoreLog(Level level, const std::string& message);

		template<typename... Args>
		inline static void ClientLog(Level level, std::format_string<Args...> format, Args&&... args)
        {
            LogMessage("TD_APP", level, std::vformat(format.get(), std::make_format_args(args...)));
        }
		static void ClientLog(Level level, const std::string& message);

	private:
		static void LogMessage(const std::string& name, Level level, const std::string& message);
	};
}

// Core log macros
#define TD_CORE_TRACE(...)    ::Teddy::Log::CoreLog(::Teddy::Log::Level::Trace, __VA_ARGS__)
#define TD_CORE_INFO(...)     ::Teddy::Log::CoreLog(::Teddy::Log::Level::Info, __VA_ARGS__)
#define TD_CORE_WARN(...)     ::Teddy::Log::CoreLog(::Teddy::Log::Level::Warn, __VA_ARGS__)
#define TD_CORE_ERROR(...)    ::Teddy::Log::CoreLog(::Teddy::Log::Level::Error, __VA_ARGS__)
#define TD_CORE_CRITICAL(...) ::Teddy::Log::CoreLog(::Teddy::Log::Level::Critical, __VA_ARGS__)

// Client log macros
#define TD_TRACE(...)         ::Teddy::Log::ClientLog(::Teddy::Log::Level::Trace, __VA_ARGS__)
#define TD_INFO(...)          ::Teddy::Log::ClientLog(::Teddy::Log::Level::Info, __VA_ARGS__)
#define TD_WARN(...)          ::Teddy::Log::ClientLog(::Teddy::Log::Level::Warn, __VA_ARGS__)
#define TD_ERROR(...)         ::Teddy::Log::ClientLog(::Teddy::Log::Level::Error, __VA_ARGS__)
#define TD_CRITICAL(...)      ::Teddy::Log::ClientLog(::Teddy::Log::Level::Critical, __VA_ARGS__)

// Assert macros
#ifdef TD_ENABLE_ASSERTS
	#define TD_ASSERT(x, ...) { if(!(x)) { TD_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define TD_CORE_ASSERT(x, ...) { if(!(x)) { TD_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define TD_ASSERT(x, ...)
	#define TD_CORE_ASSERT(x, ...)
#endif
