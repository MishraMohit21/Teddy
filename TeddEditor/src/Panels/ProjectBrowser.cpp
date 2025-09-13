#include "ProjectBrowser.h"
#include "Teddy/Utils/PlatformUtils.h"
#include "Teddy/Core/Application.h"

#include <imgui/imgui.h>

namespace Teddy
{

	static int LowercaseAlphanumericFilter(ImGuiInputTextCallbackData* data)
	{
		if (data->EventFlag == ImGuiInputTextFlags_CallbackCharFilter)
		{
			if (islower(data->EventChar) || isdigit(data->EventChar))
				return 0; // Accept character
			else
				return 1; // Reject character
		}
		return 0;
	}

	ProjectBrowser::ProjectBrowser()
	{
	}

    void ProjectBrowser::OnImGuiRender(bool* show)
    {
        // If show parameter exists and is false, don't render the window at all
        if (show && !*show)
            return;

        // Add window flags to prevent docking issues
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;

        // Always pass the show parameter if it exists
        if (show)
            ImGui::Begin("Project Browser", show, windowFlags);
        else
            ImGui::Begin("Project Browser", nullptr, windowFlags);

        if (ImGui::Button("New Project..."))
            ImGui::OpenPopup("New Project");

        if (ImGui::Button("Open Project..."))
        {
            std::string filepath = FileDialogs::OpenFile("Teddy Project (*.teddyproj)\0*.teddyproj\0");
            if (!filepath.empty())
            {
                if (m_OpenProjectCallback)
                    m_OpenProjectCallback(filepath);
            }
        }

        // New Project Dialog
        if (ImGui::BeginPopupModal("New Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Only lowercase letters and numbers are allowed.");
            ImGui::InputText("Name", m_NewProjectName, sizeof(m_NewProjectName), ImGuiInputTextFlags_CallbackCharFilter, LowercaseAlphanumericFilter);
            ImGui::Text("Location: %s", m_NewProjectLocation.c_str());
            ImGui::SameLine();
            if (ImGui::Button("Browse..."))
            {
                m_NewProjectLocation = FileDialogs::OpenFolder();
            }

            if (ImGui::Button("Create"))
            {
                if (strlen(m_NewProjectName) > 0 && !m_NewProjectLocation.empty())
                {
                    std::filesystem::path projectPath = std::filesystem::path(m_NewProjectLocation) / m_NewProjectName;
                    Application::Get().NewProject(projectPath);

                    if (m_OpenProjectCallback)
                        m_OpenProjectCallback(projectPath / (std::string(m_NewProjectName) + ".teddyproj"));
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::End();
    }
}
