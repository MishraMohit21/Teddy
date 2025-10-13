#include "tdpch.h"
#include "Teddy/Core/Log.h"

#include <iostream>
#include <chrono>
#include <iomanip>

namespace Teddy {

void Log::CoreLog(Level level, const std::string& message)
{
	LogMessage("TD_CORE", level, message);
}

void Log::ClientLog(Level level, const std::string& message)
{
	LogMessage("TD_APP", level, message);
}

void Log::LogMessage(const std::string& name, Level level, const std::string& message)
{
	auto now = std::chrono::system_clock::now();
	auto time = std::chrono::system_clock::to_time_t(now);
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

	std::cout << "[" << std::put_time(std::localtime(&time), "%T") << "." << std::setfill('0') << std::setw(3) << ms.count() << "]";
	std::cout << " [" << name << "] ";

	switch (level)
	{
	case Level::Trace:    std::cout << "[Trace]    : "; break;
	case Level::Info:     std::cout << "[Info]     : "; break;
	case Level::Warn:     std::cout << "[Warning]  : "; break;
	case Level::Error:    std::cout << "[Error]    : "; break;
	case Level::Critical: std::cout << "[Critical] : "; break;
	}

	std::cout << message << std::endl;
}

}
