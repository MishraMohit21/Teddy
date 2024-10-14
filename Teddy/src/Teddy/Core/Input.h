#pragma once

#include "Teddy/Core/Core.h"

namespace Teddy {

	class Input
	{
	public:

		static bool IsKeyPressed(int keycode);

		static bool IsMouseButtonPressed(int button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}