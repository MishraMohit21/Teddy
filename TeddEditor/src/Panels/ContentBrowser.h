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

	private:
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture2D> m_DirectoryIcon, m_FileIcon;
	};
}