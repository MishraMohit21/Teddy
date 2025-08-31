#pragma once
#include <filesystem>
#include <Teddy/Core/Core.h>
#include <Teddy/Core/UUID.h>

namespace Teddy {

    class Project {
    public:
        Project() = default;

        // --- Static Active Project ---
        static Ref<Project>& GetActive();
        static void SetActive(const Ref<Project>& project);

        // --- Core Identity ---
        const std::string& GetName() const { return m_Name; }
        const std::filesystem::path& GetProjectDirectory() const { return m_ProjectDirectory; }
        const std::filesystem::path& GetProjectFilePath() const { return m_ProjectFilePath; }

        // --- Path Management ---
        const std::filesystem::path& GetAssetDirectory() const { return m_AssetDirectory; }
        const std::filesystem::path& GetAssetRegistryPath() const { return m_AssetRegistryPath; }
        const std::filesystem::path& GetMainScenePath() const { return m_MainScenePath; }
        const std::vector<std::filesystem::path>& GetSceneList() const { return m_SceneList; }


        std::filesystem::path GetAbsolute(const std::filesystem::path& relative) const;

        // --- Runtime Settings ---
        const std::filesystem::path& GetScriptCorePath() const { return m_ScriptCorePath; }
        const std::filesystem::path& GetGameAssemblyPath() const { return m_GameAssemblyPath; }
        bool ReloadAssemblyOnPlay() const { return m_ReloadAssemblyOnPlay; }

        // Setters
        void SetName(const std::string& name) { m_Name = name; }
        void SetProjectDirectory(const std::filesystem::path& path) { m_ProjectDirectory = path; }
        void SetProjectFilePath(const std::filesystem::path& path) { m_ProjectFilePath = path; }
        void SetAssetDirectory(const std::filesystem::path& path) { m_AssetDirectory = path; }
        void SetAssetRegistryPath(const std::filesystem::path& path) { m_AssetRegistryPath = path; }
        void SetMainScenePath(const std::filesystem::path& path) { m_MainScenePath = path; }
        void AddSceneToList(const std::filesystem::path& scene) { m_SceneList.push_back(scene); }
        void ClearSceneList() { m_SceneList.clear(); }
        void SetScriptCorePath(const std::filesystem::path& path) { m_ScriptCorePath = path; }
        void SetGameAssemblyPath(const std::filesystem::path& path) { m_GameAssemblyPath = path; }
        void SetReloadAssemblyOnPlay(bool reload) { m_ReloadAssemblyOnPlay = reload; }

    private:
        inline static Ref<Project> s_Active;

        // Core Identity
        std::string m_Name;
        std::filesystem::path m_ProjectDirectory;
        std::filesystem::path m_ProjectFilePath;

        // Path Management
        std::filesystem::path m_AssetDirectory = "Assets";
        std::filesystem::path m_AssetRegistryPath = "Assets/AssetRegistry.yaml";

        std::filesystem::path m_MainScenePath;
        std::vector<std::filesystem::path> m_SceneList;


        // Runtime Settings
        std::filesystem::path m_ScriptCorePath;
        std::filesystem::path m_GameAssemblyPath;
        bool m_ReloadAssemblyOnPlay = true;
    };

}
