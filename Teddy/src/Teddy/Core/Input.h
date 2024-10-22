#pragma once

#include "Teddy/Core/Core.h"
#include "KeyCodes.h"
#include "MouseCode.h"

namespace Teddy {

	class Input
	{
	public:

		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}