#pragma once

#include <filesystem>
#include <functional>

namespace Teddy
{
	class ProjectBrowser
	{
	public:
		ProjectBrowser();

		void OnImGuiRender(bool* show = nullptr);

		void SetOpenProjectCallback(const std::function<void(const std::filesystem::path&)>& callback) { m_OpenProjectCallback = callback; }
		void OpenNewProjectModal();

	private:
		std::function<void(const std::filesystem::path&)> m_OpenProjectCallback;
		char m_NewProjectName[256];
		std::string m_NewProjectLocation;
	};
}
