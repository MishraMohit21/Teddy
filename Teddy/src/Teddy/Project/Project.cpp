#include "tdpch.h"
#include "Project.h"

#include "ProjectSerializer.h"

namespace Teddy
{
	Ref<Project>& Project::GetActive()
	{
		return s_Active;
	}

	void Project::SetActive(const Ref<Project>& project)
	{
		s_Active = project;
	}

	
	std::filesystem::path Project::GetAbsolute(const std::filesystem::path& relative) const
	{
		return m_ProjectDirectory / relative;
	}
}
