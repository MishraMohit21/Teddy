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

		// THEME: Midnight Cool v2

		// CONSTANTS
		const ImVec4 color_primary_surface = {0.047f, 0.090f, 0.137f, 1.0f}; // #0C1723
		const ImVec4 color_secondary_surface = {0.078f, 0.129f, 0.188f, 1.0f}; // #142130
		const ImVec4 color_panel_header = {0.118f, 0.184f, 0.271f, 1.0f}; // #1E2F45
		const ImVec4 color_accent_cyan = {0.0f, 0.549f, 0.729f, 1.0f}; // #008CBA
		const ImVec4 color_accent_cyan_hover = {0.0f, 0.663f, 0.878f, 1.0f}; // #00A9E0
		const ImVec4 color_accent_cyan_active = {0.0f, 0.482f, 0.604f, 1.0f}; // #007B9A
		const ImVec4 color_accent_magenta = {0.694f, 0.451f, 1.0f, 1.0f}; // #B173FF
		const ImVec4 color_text_primary = {0.902f, 0.914f, 0.925f, 1.0f}; // #E6E9EC
		const ImVec4 color_text_disabled = {0.431f, 0.482f, 0.533f, 1.0f}; // #6E7B88
		const ImVec4 color_border = { 0.118f, 0.184f, 0.271f, 1.0f }; // #1E2F45
		const ImVec4 color_frame_bg = { 0.1f, 0.15f, 0.2f, 1.0f }; // #1A2633
		const ImVec4 color_frame_bg_hover = {0.118f, 0.184f, 0.271f, 0.8f};
		const ImVec4 color_frame_bg_active = {0.118f, 0.184f, 0.271f, 1.0f};

		// STYLE
		style.Alpha = 1.0f;
		style.WindowPadding = ImVec2(10.0f, 10.0f);
		style.WindowMinSize = ImVec2(32.0f, 32.0f);
		style.WindowRounding = 6.0f;
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_Right;
		style.ChildRounding = 6.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupRounding = 6.0f;
		style.PopupBorderSize = 1.0f;
		style.FramePadding = ImVec2(8.0f, 6.0f);
		style.FrameRounding = 6.0f;
		style.FrameBorderSize = 1.0f;
		style.ItemSpacing = ImVec2(8.0f, 8.0f);
		style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
		style.IndentSpacing = 25.0f;
		style.ScrollbarSize = 12.0f;
		style.ScrollbarRounding = 8.0f;
		style.GrabMinSize = 12.0f;
		style.GrabRounding = 6.0f;
		style.TabRounding = 6.0f;
		style.TabBorderSize = 1.0f;
		style.TabMinWidthForUnselectedCloseButton = 0.0f;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
		style.AntiAliasedLines = true;
		style.AntiAliasedFill = true;
		style.CurveTessellationTol = 1.25f;
		style.CircleSegmentMaxError = 1.6f;

		// COLORS
		colors[ImGuiCol_Text] = color_text_primary;
		colors[ImGuiCol_TextDisabled] = color_text_disabled;
		colors[ImGuiCol_WindowBg] = color_primary_surface;
		colors[ImGuiCol_ChildBg] = color_primary_surface;
		colors[ImGuiCol_PopupBg] = color_secondary_surface;
		colors[ImGuiCol_Border] = color_border;
		colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_FrameBg] = color_frame_bg;
		colors[ImGuiCol_FrameBgHovered] = color_frame_bg_hover;
		colors[ImGuiCol_FrameBgActive] = color_frame_bg_active;
		colors[ImGuiCol_TitleBg] = color_primary_surface;
		colors[ImGuiCol_TitleBgActive] = color_panel_header;
		colors[ImGuiCol_TitleBgCollapsed] = color_primary_surface;
		colors[ImGuiCol_MenuBarBg] = color_secondary_surface;
		colors[ImGuiCol_ScrollbarBg] = color_secondary_surface;
		colors[ImGuiCol_ScrollbarGrab] = color_panel_header;
		colors[ImGuiCol_ScrollbarGrabHovered] = color_frame_bg_hover;
		colors[ImGuiCol_ScrollbarGrabActive] = color_frame_bg_active;
		colors[ImGuiCol_CheckMark] = color_accent_cyan;
		colors[ImGuiCol_SliderGrab] = color_accent_magenta;
		colors[ImGuiCol_SliderGrabActive] = color_accent_cyan_hover;
		colors[ImGuiCol_Button] = color_accent_cyan;
		colors[ImGuiCol_ButtonHovered] = color_accent_cyan_hover;
		colors[ImGuiCol_ButtonActive] = color_accent_cyan_active;
		colors[ImGuiCol_Header] = color_panel_header;
		colors[ImGuiCol_HeaderHovered] = color_accent_cyan_hover;
		colors[ImGuiCol_HeaderActive] = color_accent_cyan;
		colors[ImGuiCol_Separator] = color_border;
		colors[ImGuiCol_SeparatorHovered] = color_accent_cyan_hover;
		colors[ImGuiCol_SeparatorActive] = color_accent_cyan_active;
		colors[ImGuiCol_ResizeGrip] = color_accent_cyan;
		colors[ImGuiCol_ResizeGripHovered] = color_accent_cyan_hover;
		colors[ImGuiCol_ResizeGripActive] = color_accent_cyan_active;
		colors[ImGuiCol_Tab] = color_panel_header;
		colors[ImGuiCol_TabHovered] = color_accent_cyan_hover;
		colors[ImGuiCol_TabActive] = color_accent_cyan;
		colors[ImGuiCol_TabUnfocused] = color_panel_header;
		colors[ImGuiCol_TabUnfocusedActive] = color_secondary_surface;
		colors[ImGuiCol_PlotLines] = color_accent_cyan;
		colors[ImGuiCol_PlotLinesHovered] = color_accent_cyan_hover;
		colors[ImGuiCol_PlotHistogram] = color_accent_magenta;
		colors[ImGuiCol_PlotHistogramHovered] = color_accent_cyan_hover;
		colors[ImGuiCol_TextSelectedBg] = color_accent_cyan;
		colors[ImGuiCol_DragDropTarget] = color_accent_cyan_hover;
		colors[ImGuiCol_NavHighlight] = color_accent_cyan;
		colors[ImGuiCol_NavWindowingHighlight] = color_accent_cyan_hover;
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.2f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.35f);
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

		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/Fonts/openSans/OpenSans-Bold.ttf", 16.0f);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

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