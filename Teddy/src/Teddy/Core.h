#pragma once

#include <memory>

#ifdef TD_PLATFORM_WINDOWS
#if TD_DYNAMIC_LINK
	#ifdef TD_BUILD_DLL
		#define  __declspec(dllexport)
	#else
		#define  __declspec(dllimport)
	#endif
#else
    #define Teddy_API
#endif
#else
	#error Teddy only supports Windows!
#endif

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

#define BIT(x) (1 << x)

#define TD_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Teddy {

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;

}
