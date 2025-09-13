#pragma once

#include <filesystem>
#include <imgui/imgui.h>
#include <Teddy/Renderer/Texture.h>

namespace Teddy
{
	class ContentBrowser
	{
	public:
		ContentBrowser();

		void OnImGuiRender(bool& show);

		void SetContext(const Ref<Project>& project);

		void ShowNewScenePopup() { m_ShowNewScenePopup = true; }

	private:
		Ref<Project> m_Project;
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture2D> m_DirectoryIcon, m_FileIcon;

		char m_NewFolderName[256];
		char m_NewScriptName[256];
		char m_NewSceneName[256];

		bool m_ShowNewFolderPopup = false;
		bool m_ShowNewScriptPopup = false;
		bool m_ShowNewScenePopup = false;
	};
}