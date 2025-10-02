#include "UIUtils.h"


namespace Teddy::UI {

    bool DrawCheckbox(const std::string& label, bool& value, float columnWidth)
    {
        bool modified = false;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
        if (ImGui::Checkbox("##Value", &value))
            modified = true;
		if (ImGui::IsItemFocused())
			ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(34, 211, 238, 255));
		ImGui::PopStyleVar();

        ImGui::Columns(1);
        ImGui::PopID();
        return modified;
    }

    bool DrawColorEdit4(const std::string& label, glm::vec4& value, float columnWidth)
    {
        bool modified = false;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
        if (ImGui::ColorEdit4("##Value", glm::value_ptr(value)))
            modified = true;
		if (ImGui::IsItemFocused())
			ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(34, 211, 238, 255));
		ImGui::PopStyleVar();

        ImGui::Columns(1);
        ImGui::PopID();
        return modified;
    }

    bool DrawSliderFloat(const std::string& label, float& value, float min, float max, float columnWidth)
    {
        bool modified = false;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushItemWidth(-1);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
        if (ImGui::SliderFloat("##Value", &value, min, max))
            modified = true;
		if (ImGui::IsItemFocused())
			ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(34, 211, 238, 255));
		ImGui::PopStyleVar();
        ImGui::PopItemWidth();

        ImGui::Columns(1);
        ImGui::PopID();
        return modified;
    }

    bool DrawDragFloat(const std::string& label, float& value, float speed, float min, float max, float columnWidth)
    {
        bool modified = false;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushItemWidth(-1);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
        if (ImGui::DragFloat("##Value", &value, speed, min, max))
            modified = true;
		if (ImGui::IsItemFocused())
			ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(34, 211, 238, 255));
		ImGui::PopStyleVar();
        ImGui::PopItemWidth();

        ImGui::Columns(1);
        ImGui::PopID();
        return modified;
    }

    bool DrawDragFloat2(const std::string& label, ::glm::vec2& value, float speed, float min, float max, float columnWidth)
    {
        bool modified = false;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
 
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushItemWidth(-1);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
        if (ImGui::DragFloat2("##Value", ::glm::value_ptr(value), speed, min, max))
            modified = true;
		if (ImGui::IsItemFocused())
			ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(34, 211, 238, 255));
		ImGui::PopStyleVar();
        ImGui::PopItemWidth();

        ImGui::Columns(1);
        ImGui::PopID();
        return modified;
    }

    bool DrawCombo(const std::string& label, int& currentItem, const char* const items[], int items_count, float columnWidth)
    {
        bool modified = false;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushItemWidth(-1);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
        if (ImGui::Combo("##Value", &currentItem, items, items_count))
            modified = true;
		if (ImGui::IsItemFocused())
			ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(34, 211, 238, 255));
		ImGui::PopStyleVar();
        ImGui::PopItemWidth();

        ImGui::Columns(1);
        ImGui::PopID();
        return modified;
    }

    bool DrawInputScalar(const std::string& label, ImGuiDataType data_type, void* p_data, const void* p_step, const void* p_step_fast, const char* format, float columnWidth)
    {
        bool modified = false;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushItemWidth(-1);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
        if (ImGui::InputScalar("##Value", data_type, p_data, p_step, p_step_fast, format))
            modified = true;
		if (ImGui::IsItemFocused())
			ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(34, 211, 238, 255));
		ImGui::PopStyleVar();
        ImGui::PopItemWidth();

        ImGui::Columns(1);
        ImGui::PopID();
        return modified;
    }

	bool DrawVec3ScriptField(const std::string& label, glm::vec3& values, const glm::vec3& resetValue, float columnWidth)
	{
		bool modified = false;
		ImGui::PushID(label.c_str());

		float dragFloatWidth = 75.0f;

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { 20.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue.x;
			modified = true;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(dragFloatWidth);
		if (ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f"))
			modified = true;
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue.y;
			modified = true;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(dragFloatWidth);
		if (ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f"))
			modified = true;
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		if (ImGui::Button("Z", buttonSize))
		{
			values.z = resetValue.z;
			modified = true;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(dragFloatWidth);
		if (ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f"))
			modified = true;
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return modified;
	}

	bool DrawVec2ScriptField(const std::string& label, glm::vec2& values, const glm::vec2& resetValue, float columnWidth)
	{
		bool modified = false;
		ImGui::PushID(label.c_str());

		float dragFloatWidth = 75.0f;

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { 20.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue.x;
			modified = true;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(dragFloatWidth);
		if (ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f"))
			modified = true;
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue.y;
			modified = true;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(dragFloatWidth);
		if (ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f"))
			modified = true;
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return modified;
	}

	bool DrawVec4ScriptField(const std::string& label, glm::vec4& values, const glm::vec4& resetValue, float columnWidth)
	{
		bool modified = false;
		ImGui::PushID(label.c_str());

		float dragFloatWidth = 75.0f;

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { 20.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue.x;
			modified = true;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(dragFloatWidth);
		if (ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f"))
			modified = true;
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue.y;
			modified = true;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(dragFloatWidth);
		if (ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f"))
			modified = true;
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		if (ImGui::Button("Z", buttonSize))
		{
			values.z = resetValue.z;
			modified = true;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(dragFloatWidth);
		if (ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f"))
			modified = true;
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.9f, 0.6f, 0.1f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 1.0f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.9f, 0.6f, 0.1f, 1.0f });
		if (ImGui::Button("W", buttonSize))
		{
			values.w = resetValue.w;
			modified = true;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(dragFloatWidth);
		if (ImGui::DragFloat("##W", &values.w, 0.1f, 0.0f, 0.0f, "%.2f"))
			modified = true;
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return modified;
	}

	bool DrawFloatScriptField(const std::string& label, float& value, float resetValue, float columnWidth)
	{
		bool modified = false;
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		if (ImGui::Button("R"))
		{
			value = resetValue;
			modified = true;
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat("##Value", &value, 0.1f))
			modified = true;
		ImGui::PopItemWidth();

		ImGui::Columns(1);
		ImGui::PopID();
		return modified;
	}

	bool DrawDoubleScriptField(const std::string& label, double& value, double resetValue, float columnWidth)
	{
		bool modified = false;
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		if (ImGui::Button("R"))
		{
			value = resetValue;
			modified = true;
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::DragScalar("##Value", ImGuiDataType_Double, &value, 0.1f))
			modified = true;
		ImGui::PopItemWidth();

		ImGui::Columns(1);
		ImGui::PopID();
		return modified;
	}

	bool DrawIntScriptField(const std::string& label, int& value, int resetValue, float columnWidth)
	{
		bool modified = false;
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		if (ImGui::Button("R"))
		{
			value = resetValue;
			modified = true;
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::DragInt("##Value", &value, 0.1f))
			modified = true;
		ImGui::PopItemWidth();

		ImGui::Columns(1);
		ImGui::PopID();
		return modified;
	}

	bool DrawBoolScriptField(const std::string& label, bool& value, bool resetValue, float columnWidth)
	{
		bool modified = false;
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		if (ImGui::Button("R"))
		{
			value = resetValue;
			modified = true;
		}

		ImGui::SameLine();
		if (ImGui::Checkbox("##Value", &value))
			modified = true;

		ImGui::Columns(1);
		ImGui::PopID();
		return modified;
	}
}