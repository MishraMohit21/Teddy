#include "tdpch.h"
#include "ScriptCompiler.h"
#include "Teddy/Core/Log.h"

#include <windows.h>

namespace Teddy
{
    bool ScriptCompiler::Compile(const Ref<Project>& project)
    {
        TD_CORE_INFO("ScriptCompiler::Compile called");
        if (!IsSolutionReady(project))
        {
            TD_CORE_INFO("Solution not found, generating...");
            if (!GenerateSolution(project))
            {
                TD_CORE_ERROR("Failed to generate solution!");
                return false;
            }
        }
        else
        {
            TD_CORE_INFO("Solution already exists.");
        }

        std::filesystem::path solutionPath = project->GetProjectDirectory() / (project->GetName() + ".sln");
        TD_CORE_INFO("Compiling scripts at {}...", solutionPath.string());

        std::string msbuildPath = project->GetMSBuildPath().string();
        if (msbuildPath.empty())
        {
            // TODO: Make this configurable
            msbuildPath = "C:/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe";
            TD_CORE_WARN("MSBuild path not specified in project settings. Using default path: {}", msbuildPath);
        }

        std::string command = msbuildPath + " " + solutionPath.string() + " /p:Configuration=Debug /p:Platform=x64";
        TD_CORE_INFO("Executing command: {}", command);

        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        std::filesystem::path projectPath = project->GetProjectDirectory();
        if (!CreateProcessA(NULL, (char*)command.c_str(), NULL, NULL, FALSE, 0, NULL, projectPath.string().c_str(), &si, &pi))
        {
            TD_CORE_ERROR("Failed to create process for MSBuild!");
            return false;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        if (exitCode != 0)
        {
            TD_CORE_ERROR("MSBuild failed with exit code {}!", exitCode);
            return false;
        }

        TD_CORE_INFO("Scripts compiled successfully!");
        return true;
    }

    bool ScriptCompiler::IsSolutionReady(const Ref<Project>& project)
    {
        std::filesystem::path solutionPath = project->GetProjectDirectory() / (project->GetName() + ".sln");
        return std::filesystem::exists(solutionPath);
    }

    bool ScriptCompiler::GenerateSolution(const Ref<Project>& project)
    {
        std::filesystem::path projectPath = project->GetProjectDirectory();
        TD_CORE_INFO("Generating solution in: {}", projectPath.string());

        // Create premake5.lua
        std::filesystem::path templatePath = "Resources/Templates/premake5.lua.tpl";
        TD_CORE_INFO("Reading premake template from: {}", templatePath.string());
        std::ifstream premakeTemplate(templatePath);
        if (!premakeTemplate.is_open())
        {
            TD_CORE_ERROR("Failed to open premake template file!");
            return false;
        }
        std::stringstream premakeContent;
        premakeContent << premakeTemplate.rdbuf();
        premakeTemplate.close();

        std::string premakeFile = premakeContent.str();
        TD_CORE_INFO("Premake template content:\n{}", premakeFile);

        std::string projectName = project->GetName();
        size_t pos = premakeFile.find("%{prj.name}");
        while (pos != std::string::npos)
        {
            premakeFile.replace(pos, strlen("%{prj.name}"), projectName);
            pos = premakeFile.find("%{prj.name}", pos + projectName.length());
        }

        std::filesystem::path premakePath = projectPath / "premake5.lua";
        TD_CORE_INFO("Creating premake file at: {}", premakePath.string());
        std::ofstream premake(premakePath);
        premake << premakeFile;
        premake.close();

        // Run premake
        std::string premakeCommand = "..\\vendor\\bin\\premake\\premake5.exe vs2022";
        TD_CORE_INFO("Running premake command: {}", premakeCommand);
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        if (CreateProcessA(NULL, (char*)premakeCommand.c_str(), NULL, NULL, FALSE, 0, NULL, projectPath.string().c_str(), &si, &pi))
        {
            WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            TD_CORE_INFO("Premake executed successfully.");
            return true;
        }
        else
        {
            TD_CORE_ERROR("Failed to run premake!");
            return false;
        }
    }
}