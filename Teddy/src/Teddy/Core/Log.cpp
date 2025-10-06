
#include "tdpch.h"
#include "Log.h"

namespace Teddy {

	void Log::Init()
	{
	}

	void Log::Print(const std::string& message)
	{
		std::cout << message << std::endl;
	}

	void Log::Print(const char* message)
	{
		std::cout << message << std::endl;
	}

}
