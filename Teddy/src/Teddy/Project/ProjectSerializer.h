#pragma once
#include <Teddy/Project/Project.h>

namespace Teddy {

	class ProjectSerializer
	{
    public:
        ProjectSerializer(const Ref<Project>& project);

        bool Serialize(const std::filesystem::path& filepath);
        bool Deserialize(const std::filesystem::path& filepath);

    private:
        Ref<Project> m_Project;

	};

}