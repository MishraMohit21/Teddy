#pragma once

#include <Teddy/Scene/Scene.h>
#include <Teddy/Core/Core.h>
#include <imgui/imgui.h>
#include <Teddy/Scene/Entity.h>

namespace Teddy
{
	class SceneHeirarchyPanel
	{
	public:
		SceneHeirarchyPanel() = default;
		SceneHeirarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiUpdate();
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};

}