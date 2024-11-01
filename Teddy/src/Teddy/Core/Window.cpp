#include "tdpch.h"
#include "Window.h"

#ifdef TD_PLATFORM_WINDOWS
	#include <Platform/Windows/WindowsWindow.h>
#endif

namespace Teddy
{
	Scope<Window> Window::Create(const WindowProps& props)
	{
#ifdef TD_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(props);
#else
		TD_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}
}