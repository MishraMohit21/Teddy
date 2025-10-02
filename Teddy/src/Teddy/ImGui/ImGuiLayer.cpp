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
		auto& style = ImGui::GetStyle();
		auto& colors = ImGui::GetStyle().Colors;

		// Tech Futurist Palette
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.0588f, 0.0902f, 0.1647f, 1.0f };    // #0F172A (Base Background)
		colors[ImGuiCol_ChildBg] = ImVec4{ 0.1176f, 0.1608f, 0.2314f, 1.0f };    // #1E293B (Primary Surface)
		colors[ImGuiCol_PopupBg] = ImVec4{ 0.2f, 0.2549f, 0.3333f, 1.0f };        // #334155 (Elevated Surface)
		colors[ImGuiCol_Border] = ImVec4{ 0.2f, 0.2549f, 0.3333f, 1.0f };        // #334155 (Borders/Dividers)

		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.2549f, 0.3333f, 1.0f };        // #334155 (Elevated for Headers)
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.2784f, 0.3333f, 0.4118f, 1.0f }; // #475569 (Hover/Active)
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.1333f, 0.7961f, 0.9333f, 1.0f };   // #22D3EE (Accent for active states)

		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.2549f, 0.3333f, 1.0f };        // #334155 (Elevated for Buttons)
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.2784f, 0.3333f, 0.4118f, 1.0f }; // #475569 (Hover/Active)
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.1333f, 0.7961f, 0.9333f, 1.0f };   // #22D3EE (Accent for active states)

		colors[ImGuiCol_FrameBg] = ImVec4{ 0.0588f, 0.0902f, 0.1647f, 1.0f };    // #0F172A (Base for input fields)
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.1176f, 0.1608f, 0.2314f, 1.0f }; // #1E293B (Primary Surface on hover)
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.1176f, 0.1608f, 0.2314f, 1.0f }; // #1E293B

		colors[ImGuiCol_Tab] = ImVec4{ 0.1176f, 0.1608f, 0.2314f, 1.0f };        // #1E293B
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.2784f, 0.3333f, 0.4118f, 1.0f }; // #475569
		colors[ImGuiCol_TabActive] = ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive);
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.1176f, 0.1608f, 0.2314f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.2549f, 0.3333f, 1.0f };

		colors[ImGuiCol_TitleBg] = ImVec4{ 0.0588f, 0.0902f, 0.1647f, 1.0f };    // #0F172A
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.0588f, 0.0902f, 0.1647f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.0588f, 0.0902f, 0.1647f, 1.0f };

		colors[ImGuiCol_Separator] = ImVec4{ 0.2f, 0.2549f, 0.3333f, 1.0f }; // #334155
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.1333f, 0.7961f, 0.9333f, 1.0f); // #22D3EE
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.1333f, 0.7961f, 0.9333f, 1.0f); // #22D3EE

		colors[ImGuiCol_Text] = ImVec4{ 0.8863f, 0.9098f, 0.9412f, 1.0f };      // #E2E8F0 (High-Emphasis)
		colors[ImGuiCol_TextDisabled] = ImVec4{ 0.5804f, 0.6392f, 0.7216f, 1.0f }; // #94A3B8 (Medium-Emphasis)

		colors[ImGuiCol_CheckMark] = ImVec4{ 0.1333f, 0.7961f, 0.9333f, 1.0f };   // #22D3EE
		colors[ImGuiCol_SliderGrab] = ImVec4{ 0.1333f, 0.7961f, 0.9333f, 1.0f };  // #22D3EE
		colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.0549f, 0.4549f, 0.5647f, 1.0f }; // #0E7490 (Muted Accent)

		colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.1176f, 0.1608f, 0.2314f, 1.0f };    // #1E293B

		// Structure & Form
		style.WindowRounding = 8.0f;
		style.FrameRounding = 4.0f;
		style.ChildRounding = 8.0f;
		style.PopupRounding = 8.0f;
		style.GrabRounding = 4.0f;
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