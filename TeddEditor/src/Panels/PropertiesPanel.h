#pragma once

#include "Teddy/Scene/Entity.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Teddy
{

	class PropertiesPanel
	{
	public:
		PropertiesPanel() = default;
		void SetContext(Entity entity);
		void OnImGuiRender();
		void SetFocusTextEditor(bool focus) { m_FocusTextEditor = focus; }
	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void DrawComponents(Entity entity);

		template<typename T, typename UIFunction>
		static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
		{
			const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
			if (entity.HasComponent<T>())
			{
				auto& component = entity.GetComponent<T>();
				ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
				float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
				ImGui::Separator();
				bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
				ImGui::PopStyleVar();
				ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
				if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
				{
					ImGui::OpenPopup("ComponentSettings");
				}

				bool removeComponent = false;
				if (ImGui::BeginPopup("ComponentSettings"))
				{
					if (ImGui::MenuItem("Remove component"))
						removeComponent = true;

					ImGui::EndPopup();
				}

				if (open)
				{
					uiFunction(component);
					ImGui::TreePop();
				}

				if (removeComponent)
					entity.RemoveComponent<T>();
			}
		}

	private:
		Entity m_SelectionContext;
		bool m_FocusTextEditor = false;
	};

}
