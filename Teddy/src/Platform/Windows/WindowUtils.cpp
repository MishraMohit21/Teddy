#include <tdpch.h>
#include "Teddy/Utils/PlatformUtils.h"

#include <commdlg.h>
#include <shobjidl.h> // For IFileDialog
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <Teddy/Core/Application.h>


namespace Teddy
{

	std::string FileDialogs::OpenFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
		return std::string();
	}
	std::string FileDialogs::SaveFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.lpstrDefExt = strchr(filter, '\0') + 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetSaveFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
		return std::string();
	}

	std::string FileDialogs::OpenFolder()
	{
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (SUCCEEDED(hr))
		{
			IFileOpenDialog* pfd;
			hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pfd));
			if (SUCCEEDED(hr))
			{
				DWORD dwOptions;
				pfd->GetOptions(&dwOptions);
				pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);

				hr = pfd->Show(glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow()));
				if (SUCCEEDED(hr))
				{
					IShellItem* psi;
					hr = pfd->GetResult(&psi);
					if (SUCCEEDED(hr))
					{
						PWSTR pszFilePath;
						hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
						if (SUCCEEDED(hr))
						{
							std::wstring wide(pszFilePath);
							std::string result(wide.begin(), wide.end());
							CoTaskMemFree(pszFilePath);
							psi->Release();
							pfd->Release();
							CoUninitialize();
							return result;
						}
						psi->Release();
					}
				}
				pfd->Release();
			}
			CoUninitialize();
		}
		return std::string();
	}

}
