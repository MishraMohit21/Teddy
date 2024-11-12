#pragma once

#include "Teddy/Core/Core.h"
#include "Teddy/Scene/Scene.h"
#include "Teddy/Scene/Entity.h"
#include <Teddy/Utils/PlatformUtils.h>


namespace Teddy {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		void OnImGuiRender();
		void SetSelectedEntity(Entity entity);
		Entity GetSelectedEntity() const { return m_SelectionContext; }
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);

	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};

}

void ShowTimedPrompt(float duration);