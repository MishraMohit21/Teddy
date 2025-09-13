#include "tdpch.h"
#include "ContentBrowser.h"
#include "Teddy/Project/Project.h"
#include "Teddy/Scene/Scene.h"
#include "Teddy/Scene/SceneSerializer.h"


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

	ContentBrowser::ContentBrowser()
	{
		m_DirectoryIcon = Texture2D::Create("external/Icon/diricon.png");
		m_FileIcon = Texture2D::Create("external/Icon/fileicon.png");

		memset(m_NewFolderName, 0, sizeof(m_NewFolderName));
		memset(m_NewScriptName, 0, sizeof(m_NewScriptName));
		memset(m_NewSceneName, 0, sizeof(m_NewSceneName));
	}

	void ContentBrowser::SetContext(const Ref<Project>& project)
	{
		m_Project = project;
		m_CurrentDirectory = m_Project->GetProjectDirectory();
	}

	void ContentBrowser::OnImGuiRender(bool& show)
	{

		ImGui::Begin("Content Browser", &show);

		if (m_CurrentDirectory != m_Project->GetAssetDirectory())
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		static float padding = 17.0f;
		static float thumbnailSize = 57.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, m_Project->GetAssetDirectory());
			std::string filenameString = relativePath.filename().string();

			ImGui::PushID(filenameString.c_str());

			Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });


			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();

			}
			filenameString[0] = std::toupper(filenameString[0]);

			ImGui::TextWrapped(filenameString.c_str());

			ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::Columns(1);

		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("New Folder"))
			{
				m_ShowNewFolderPopup = true;
			}

			if (ImGui::MenuItem("New Script"))
			{
				m_ShowNewScriptPopup = true;
			}

			if (ImGui::MenuItem("New Scene"))
			{
				m_ShowNewScenePopup = true;
			}

			ImGui::EndPopup();
		}

		//ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
		//ImGui::SliderFloat("Padding", &padding, 0, 32);

		// TODO: status bar

		if (m_ShowNewFolderPopup)
		{
			ImGui::OpenPopup("New Folder");
			m_ShowNewFolderPopup = false;
		}

		if (ImGui::BeginPopupModal("New Folder", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Only lowercase letters and numbers are allowed.");
			ImGui::InputText("Name", m_NewFolderName, sizeof(m_NewFolderName), ImGuiInputTextFlags_CallbackCharFilter, LowercaseAlphanumericFilter);

			if (ImGui::Button("Create"))
			{
				if (strlen(m_NewFolderName) > 0)
				{
					std::filesystem::create_directory(m_CurrentDirectory / m_NewFolderName);
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

		if (m_ShowNewScriptPopup)
		{
			ImGui::OpenPopup("New Script");
			m_ShowNewScriptPopup = false;
		}

		if (ImGui::BeginPopupModal("New Script", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Only lowercase letters and numbers are allowed.");
			ImGui::InputText("Name", m_NewScriptName, sizeof(m_NewScriptName), ImGuiInputTextFlags_CallbackCharFilter, LowercaseAlphanumericFilter);

			if (ImGui::Button("Create"))
			{
				if (strlen(m_NewScriptName) > 0)
				{
					std::ofstream scriptFile(m_CurrentDirectory / (std::string(m_NewScriptName) + ".cs"));
					scriptFile << "using System;\nusing Teddy;\n\npublic class " << m_NewScriptName << " : Entity\n{\n    void OnCreate()\n    {\n    }\n\n    void OnUpdate(float ts)\n    {\n    }\n}";
					scriptFile.close();
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

		if (m_ShowNewScenePopup)
		{
			ImGui::OpenPopup("New Scene");
			m_ShowNewScenePopup = false;
		}

		if (ImGui::BeginPopupModal("New Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Only lowercase letters and numbers are allowed.");
			ImGui::InputText("Name", m_NewSceneName, sizeof(m_NewSceneName), ImGuiInputTextFlags_CallbackCharFilter, LowercaseAlphanumericFilter);

			if (ImGui::Button("Create"))
			{
				if (strlen(m_NewSceneName) > 0)
				{
					Ref<Teddy::Scene> newScene = CreateRef<Teddy::Scene>();
					Teddy::SceneSerializer serializer(newScene);
					serializer.Serialize((m_CurrentDirectory / (std::string(m_NewSceneName) + ".tddy")).string());
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
