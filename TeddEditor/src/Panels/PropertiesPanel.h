#pragma once

#include "Teddy/Scene/Entity.h"

namespace Teddy
{

	class PropertiesPanel
	{
	public:
		PropertiesPanel() = default;

		void SetContext(Entity entity);
		void OnImGuiRender();

	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void DrawComponents(Entity entity);

	private:
		Entity m_SelectionContext;
	};

}
