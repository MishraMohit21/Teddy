#include "tdpch.h"
#include "ProjectSerializer.h"
#include <yaml-cpp/yaml.h>
#include <fstream>

namespace Teddy {

    ProjectSerializer::ProjectSerializer(const Ref<Project>& project)
        : m_Project(project) {
    }

    bool ProjectSerializer::Serialize(const std::filesystem::path& filepath) {
        YAML::Emitter out;
        out << YAML::BeginMap;

        // --- Project ---
        out << YAML::Key << "Project" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "Name" << YAML::Value << m_Project->GetName();
        out << YAML::Key << "AssetDirectory" << YAML::Value << m_Project->GetAssetDirectory().string();
        out << YAML::Key << "AssetRegistry" << YAML::Value << m_Project->GetAssetRegistryPath().string();
        out << YAML::EndMap;

        // --- Runtime ---
        out << YAML::Key << "Runtime" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "MainScene" << YAML::Value << m_Project->GetMainScenePath().string();
        out << YAML::Key << "ScriptCorePath" << YAML::Value << m_Project->GetScriptCorePath().string();
        out << YAML::Key << "GameAssemblyPath" << YAML::Value << m_Project->GetGameAssemblyPath().string();
        out << YAML::Key << "ReloadAssemblyOnPlay" << YAML::Value << m_Project->ReloadAssemblyOnPlay();
        out << YAML::EndMap;

        // --- Scenes ---
        out << YAML::Key << "Scenes" << YAML::Value << YAML::BeginSeq;
        for (auto& scene : m_Project->GetSceneList())
            out << scene.string();
        out << YAML::EndSeq;

        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();

        return true;
    }

    bool ProjectSerializer::Deserialize(const std::filesystem::path& filepath) {
        YAML::Node data;
        try {
            data = YAML::LoadFile(filepath.string());
        }
        catch (...) {
            return false;
        }

        auto projectNode = data["Project"];
        if (!projectNode)
            return false;

        auto& project = *m_Project;

        // Use public setters instead of accessing private members
        project.SetName(projectNode["Name"].as<std::string>());
        project.SetAssetDirectory(projectNode["AssetDirectory"].as<std::string>());
        project.SetAssetRegistryPath(projectNode["AssetRegistry"].as<std::string>());

        auto runtimeNode = data["Runtime"];
        if (runtimeNode) {
            project.SetMainScenePath(runtimeNode["MainScene"].as<std::string>());
            project.SetScriptCorePath(runtimeNode["ScriptCorePath"].as<std::string>());
            project.SetGameAssemblyPath(runtimeNode["GameAssemblyPath"].as<std::string>());
            project.SetReloadAssemblyOnPlay(runtimeNode["ReloadAssemblyOnPlay"].as<bool>());
        }

        auto scenesNode = data["Scenes"];
        if (scenesNode) {
            project.ClearSceneList();
            for (auto s : scenesNode)
                project.AddSceneToList(s.as<std::string>());
        }

        project.SetProjectFilePath(filepath);
        project.SetProjectDirectory(filepath.parent_path());

        return true;
    }

}
