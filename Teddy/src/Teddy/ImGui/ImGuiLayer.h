#pragma once

#include "Teddy/Core/Layer.h"

#include "Teddy/Events/ApplicationEvent.h"
#include "Teddy/Events/KeyEvent.h"
#include "Teddy/Events/MouseEvent.h"

namespace Teddy {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void Begin();
		void End();

		void BlockEvent(bool block) { m_blockImGuiEvent = block; }

	private:
		bool m_blockImGuiEvent = true;
		float m_Time = 0.0f;
	};

}