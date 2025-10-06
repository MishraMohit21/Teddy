#pragma once

#include <memory>
//#include <Teddy/Core/Log.h>

// Platform detection using predefined macros




#define BIT(x) (1 << x)

#define TD_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Teddy {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}
