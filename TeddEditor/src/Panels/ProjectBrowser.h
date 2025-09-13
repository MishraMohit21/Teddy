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

	private:
		char m_NewProjectName[128] = {};
		std::string m_NewProjectLocation;

		std::function<void(const std::filesystem::path&)> m_OpenProjectCallback;
	};
}
