#include "tdpch.h"
#include "Teddy/ImGui/ImGuiLayer.h"

#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include <ImGuizmo.h>

#include "Teddy/Core/Application.h"

// TEMPORARY
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Teddy {

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	void ImGuiLayer::SetDarkThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;

		// --- Dark Mode ---

		// Base: Deep space black with a hint of blue for depth
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.03f, 0.03f, 0.04f, 1.0f };

		// Headers: Subtle blue undertone for a futuristic feel
		colors[ImGuiCol_Header] = ImVec4{ 0.08f, 0.08f, 0.1f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.12f, 0.12f, 0.15f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.05f, 0.05f, 0.07f, 1.0f };

		// Buttons: Introducing a vibrant blue accent
		colors[ImGuiCol_Button] = ImVec4{ 0.1f, 0.1f, 0.12f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.0f, 0.2f, 0.4f, 1.0f }; // Electric blue
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.0f, 0.15f, 0.3f, 1.0f }; // Deeper blue

		// Frame BG: Blends with the background
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.08f, 0.08f, 0.1f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.15f, 0.15f, 0.2f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.05f, 0.05f, 0.07f, 1.0f };

		// Tabs:  Using a contrasting purple accent
		colors[ImGuiCol_Tab] = ImVec4{ 0.07f, 0.07f, 0.09f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.3f, 0.1f, 0.4f, 1.0f }; // Vivid purple
		colors[ImGuiCol_TabActive] = ImVec4{ 0.2f, 0.05f, 0.3f, 1.0f }; // Deeper purple
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.04f, 0.04f, 0.05f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.08f, 0.08f, 0.1f, 1.0f };

		colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);

		// Title: Subtle and unobtrusive
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.07f, 0.07f, 0.09f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.07f, 0.07f, 0.09f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.04f, 0.04f, 0.05f, 1.0f };

		// --- Accent Colors ---
		colors[ImGuiCol_Text] = ImVec4{ 0.8f, 0.8f, 0.82f, 1.0f }; // Slightly off-white for readability
		colors[ImGuiCol_CheckMark] = ImVec4{ 0.0f, 0.6f, 1.0f, 1.0f }; // Bright cyan for checkmarks
		colors[ImGuiCol_SliderGrab] = ImVec4{ 0.0f, 0.4f, 0.8f, 1.0f }; // Blue slider grab
		colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.0f, 0.3f, 0.7f, 1.0f }; // Slightly darker when active

		// Assuming your menu bar uses these ImGui elements:
		colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.13f, 0.13f, 0.14f, 1.0f }; // Match the main window background
		//colors[/*ImGuiCol_MenuBarItem*/] = ImVec4{ 0.8f, 0.8f, 0.82f, 1.0f }; // Slightly off-white for menu items

		// If you have a border around your menu bar:
		colors[ImGuiCol_Border] = ImVec4{ 0.1f, 0.1f, 0.12f, 1.0f }; // Subtle blue tint

		// --- Additional Styling ---

		// Rounding: Slightly rounded corners for a softer look
		ImGui::GetStyle().WindowRounding = 3.0f;
		ImGui::GetStyle().FrameRounding = 2.0f;
		ImGui::GetStyle().GrabRounding = 2.0f;

		// Window Padding: A bit more breathing room
		ImGui::GetStyle().WindowPadding = ImVec2(8.0f, 8.0f);

		// Frame Padding: Adjust for better element spacing
		ImGui::GetStyle().FramePadding = ImVec2(5.0f, 3.0f);

		
	}

	void ImGuiLayer::OnAttach()
	{
		TD_PROFILE_FUNCTION();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
		
		io.Fonts->AddFontFromFileTTF("assets/Fonts/openSans/OpenSans-Bold.ttf", 16.0f);
		io.Fonts->AddFontFromFileTTF("assets/Fonts/openSans/OpenSans-Bold.ttf", 18.0f);

		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/Fonts/openSans/OpenSans-Medium.ttf", 16.0f);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();
		//ImGui::StyleColorsLight();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.AntiAliasedFill = true;
			style.AntiAliasedLines = true;
			style.AntiAliasedLinesUseTex = true;   style.WindowPadding = ImVec2(12, 12);
			style.FramePadding = ImVec2(6, 6);
			//style.P = ImVec2(12, 6);
			style.ItemSpacing = ImVec2(6, 6);
			style.ItemInnerSpacing = ImVec2(6, 6);
			style.TouchExtraPadding = ImVec2(0, 0);
			style.IndentSpacing = 25;
			style.ScrollbarSize = 12;
			style.GrabMinSize = 10;

			// Borders
			style.WindowBorderSize = 1;
			style.ChildBorderSize = 1;
			style.PopupBorderSize = 1;
			style.FrameBorderSize = 1;
			style.TabBorderSize = 1;

			// Rounding
			style.WindowRounding = 4;
			style.ChildRounding = 4;
			style.FrameRounding = 4;
			style.PopupRounding = 4;
			style.ScrollbarRounding = 4;
			style.GrabRounding = 4;
			style.TabRounding = 4;
			//style.WindowMinSize.x = 10.0f;
			// Alignment
			style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
			style.WindowMenuButtonPosition = ImGuiDir_Right;
			style.ColorButtonPosition = ImGuiDir_Right;
			style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
			style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

			// Anti-aliasing
			style.AntiAliasedLines = true;
			style.AntiAliasedFill = true;
			//style.Colors[ImGuiCol_WindowBg].w = 0.1f;
		}

		//ImGui::ShowDemoWindow();
		SetDarkThemeColors();

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}



	void ImGuiLayer::OnDetach()
	{
		TD_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	
	void ImGuiLayer::Begin()
	{
		TD_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End()
	{
		TD_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

}