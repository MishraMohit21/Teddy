#pragma once

#include "Teddy/Core/Layer.h"

#include "Teddy/Events/ApplicationEvent.h"
#include "Teddy/Events/KeyEvent.h"
#include "Teddy/Events/MouseEvent.h"

namespace Teddy {

	class Teddy_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};

}