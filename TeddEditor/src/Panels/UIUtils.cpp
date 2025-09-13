
#include "UIUtils.h"
#include <glm/gtc/type_ptr.hpp>

namespace Teddy::UI {

    static bool DrawCheckbox(const std::string& label, bool& value, float columnWidth)
    {
        bool modified = false;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        if (ImGui::Checkbox("##Value", &value))
            modified = true;

        ImGui::Columns(1);
        ImGui::PopID();
        return modified;
    }

    static bool DrawColorEdit4(const std::string& label, glm::vec4& value, float columnWidth)
    {
        bool modified = false;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        if (ImGui::ColorEdit4("##Value", glm::value_ptr(value)))
            modified = true;

        ImGui::Columns(1);
        ImGui::PopID();
        return modified;
    }

    static bool DrawSliderFloat(const std::string& label, float& value, float min, float max, float columnWidth)
    {
        bool modified = false;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushItemWidth(-1);
        if (ImGui::SliderFloat("##Value", &value, min, max))
            modified = true;
        ImGui::PopItemWidth();

        ImGui::Columns(1);
        ImGui::PopID();
        return modified;
    }

    static bool DrawDragFloat(const std::string& label, float& value, float speed, float min, float max, float columnWidth)
    {
        bool modified = false;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushItemWidth(-1);
        if (ImGui::DragFloat("##Value", &value, speed, min, max))
            modified = true;
        ImGui::PopItemWidth();

        ImGui::Columns(1);
        ImGui::PopID();
        return modified;
    }

    static bool DrawDragFloat2(const std::string& label, glm::vec2& value, float speed, float min, float max, float columnWidth)
    {
        bool modified = false;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushItemWidth(-1);
        if (ImGui::DragFloat2("##Value", glm::value_ptr(value), speed, min, max))
            modified = true;
        ImGui::PopItemWidth();

        ImGui::Columns(1);
        ImGui::PopID();
        return modified;
    }

    static bool DrawCombo(const std::string& label, int& currentItem, const char* const items[], int items_count, float columnWidth)
    {
        bool modified = false;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushItemWidth(-1);
        if (ImGui::Combo("##Value", &currentItem, items, items_count))
            modified = true;
        ImGui::PopItemWidth();

        ImGui::Columns(1);
        ImGui::PopID();
        return modified;
    }

    static bool DrawInputScalar(const std::string& label, ImGuiDataType data_type, void* p_data, const void* p_step, const void* p_step_fast, const char* format, float columnWidth)
    {
        bool modified = false;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushItemWidth(-1);
        if (ImGui::InputScalar("##Value", data_type, p_data, p_step, p_step_fast, format))
            modified = true;
        ImGui::PopItemWidth();

        ImGui::Columns(1);
        ImGui::PopID();
        return modified;
    }
}
