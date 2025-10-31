#include "ProjectBrowser.h"
#include "Teddy/Utils/PlatformUtils.h"
#include "Teddy/Core/Application.h"
#include <imgui/imgui.h>

namespace Teddy
{
	static int LowercaseAlphanumericFilter(ImGuiInputTextCallbackData* data)
	{
		if (data->EventFlag == ImGuiInputTextFlags_CallbackCharFilter)
		{
			if (islower(data->EventChar) || isdigit(data->EventChar))
				return 0; // Accept character
			else
				return 1; // Reject character
		}
		return 0;
	}

	ProjectBrowser::ProjectBrowser()
	{
		memset(m_NewProjectName, 0, sizeof(m_NewProjectName));
	}

	void ProjectBrowser::OpenNewProjectModal()
	{
		memset(m_NewProjectName, 0, sizeof(m_NewProjectName));
		m_NewProjectLocation.clear();
		ImGui::OpenPopup("NewProjectModal");
	}

	void ProjectBrowser::OnImGuiRender(bool* show)
	{
		if (show && !*show)
			return;

		bool openNewProjectModal = false;
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(900, 600), ImGuiCond_Once);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		if (show)
			ImGui::Begin("Project Browser", show, windowFlags);
		else
			ImGui::Begin("Project Browser", nullptr, windowFlags);

		ImGui::PopStyleVar();

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();

		// Sidebar background
		float sidebarWidth = 280.0f;
		drawList->AddRectFilled(
			windowPos,
			ImVec2(windowPos.x + sidebarWidth, windowPos.y + windowSize.y),
			IM_COL32(25, 25, 28, 255)
		);

		// Main content background gradient
		drawList->AddRectFilledMultiColor(
			ImVec2(windowPos.x + sidebarWidth, windowPos.y),
			ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y),
			IM_COL32(35, 35, 38, 255),
			IM_COL32(30, 30, 33, 255),
			IM_COL32(30, 30, 33, 255),
			IM_COL32(35, 35, 38, 255)
		);

		// Sidebar divider line
		drawList->AddLine(
			ImVec2(windowPos.x + sidebarWidth, windowPos.y),
			ImVec2(windowPos.x + sidebarWidth, windowPos.y + windowSize.y),
			IM_COL32(60, 60, 65, 255),
			1.5f
		);

		// === SIDEBAR ===
		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::BeginChild("Sidebar", ImVec2(sidebarWidth, 0), false, ImGuiWindowFlags_NoScrollbar);

		ImGui::SetCursorPosY(40);
		ImGui::Indent(25);

		// Logo/Title
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		ImGui::SetWindowFontScale(1.5f);
		ImGui::Text("TEDDY ENGINE");
		ImGui::SetWindowFontScale(1.0f);
		ImGui::PopStyleColor();

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.65f, 1.0f));
		ImGui::Text("Project Manager");
		ImGui::PopStyleColor();

		ImGui::Unindent(25);

		ImGui::SetCursorPosY(140);
		ImGui::Separator();
		ImGui::Spacing();

		// Sidebar Menu Items
		ImGui::Indent(20);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 10));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 8));

		// Projects section
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.55f, 1.0f));
		ImGui::Text("QUICK ACTIONS");
		ImGui::PopStyleColor();
		ImGui::Spacing();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.15f, 0.15f, 0.18f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.13f, 1.0f));

		if (ImGui::Button("  New Project", ImVec2(sidebarWidth - 40, 0)))
		{
			memset(m_NewProjectName, 0, sizeof(m_NewProjectName));
			m_NewProjectLocation.clear();
			openNewProjectModal = true;
		}

		if (ImGui::Button("  Open Project", ImVec2(sidebarWidth - 40, 0)))
		{
			std::string filepath = FileDialogs::OpenFile("Teddy Project (*.teddyproj)\0*.teddyproj\0");
			if (!filepath.empty())
			{
				if (m_OpenProjectCallback)
					m_OpenProjectCallback(filepath);
			}
		}

		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(2);

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.55f, 1.0f));
		ImGui::Text("RECENT PROJECTS");
		ImGui::PopStyleColor();
		ImGui::Spacing();

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.4f, 0.45f, 1.0f));
		ImGui::TextWrapped("No recent projects found");
		ImGui::PopStyleColor();

		ImGui::Unindent(20);

		// Footer in sidebar
		ImGui::SetCursorPosY(windowSize.y - 50);
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Indent(20);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.4f, 0.45f, 1.0f));
		ImGui::Text("Version 1.0.0");
		ImGui::PopStyleColor();
		ImGui::Unindent(20);

		ImGui::EndChild();

		// === MAIN CONTENT AREA ===
		ImGui::SetCursorPos(ImVec2(sidebarWidth, 0));
		ImGui::BeginChild("MainContent", ImVec2(0, 0), false);

		ImGui::SetCursorPos(ImVec2(60, 60));
		ImGui::BeginGroup();

		// Welcome Section
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.95f, 1.0f));
		ImGui::SetWindowFontScale(1.8f);
		ImGui::Text("Welcome Back");
		ImGui::SetWindowFontScale(1.0f);
		ImGui::PopStyleColor();

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.65f, 1.0f));
		ImGui::Text("Get started by creating a new project or opening an existing one");
		ImGui::PopStyleColor();

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		// Action Cards
		float cardWidth = 260.0f;
		float cardHeight = 180.0f;
		float cardSpacing = 30.0f;

		ImVec2 cardStartPos = ImGui::GetCursorScreenPos();

		// New Project Card
		ImVec2 card1Pos = cardStartPos;
		bool hoveredCard1 = ImGui::IsMouseHoveringRect(
			card1Pos,
			ImVec2(card1Pos.x + cardWidth, card1Pos.y + cardHeight)
		);

		ImU32 card1Color = hoveredCard1 ? IM_COL32(50, 90, 140, 255) : IM_COL32(45, 80, 125, 255);

		drawList->AddRectFilled(
			card1Pos,
			ImVec2(card1Pos.x + cardWidth, card1Pos.y + cardHeight),
			card1Color,
			8.0f
		);

		// Add accent line on top of card
		drawList->AddRectFilled(
			card1Pos,
			ImVec2(card1Pos.x + cardWidth, card1Pos.y + 4),
			IM_COL32(60, 140, 230, 255),
			8.0f,
			ImDrawCornerFlags_Top
		);

		// Icon background circle
		ImVec2 iconCenter1 = ImVec2(card1Pos.x + cardWidth / 2, card1Pos.y + 60);
		drawList->AddCircleFilled(iconCenter1, 30, IM_COL32(60, 140, 230, 100), 32);

		// Draw plus icon
		float iconSize = 16;
		drawList->AddLine(
			ImVec2(iconCenter1.x - iconSize, iconCenter1.y),
			ImVec2(iconCenter1.x + iconSize, iconCenter1.y),
			IM_COL32(60, 140, 230, 255),
			3.0f
		);
		drawList->AddLine(
			ImVec2(iconCenter1.x, iconCenter1.y - iconSize),
			ImVec2(iconCenter1.x, iconCenter1.y + iconSize),
			IM_COL32(60, 140, 230, 255),
			3.0f
		);

		ImGui::SetCursorScreenPos(ImVec2(card1Pos.x + 20, card1Pos.y + 105));
		ImGui::BeginGroup();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		ImGui::SetWindowFontScale(1.2f);
		ImGui::Text("New Project");
		ImGui::SetWindowFontScale(1.0f);
		ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.75f, 1.0f));
		ImGui::PushTextWrapPos(card1Pos.x + cardWidth - 20);
		ImGui::Text("Start fresh with a new game project");
		ImGui::PopTextWrapPos();
		ImGui::PopStyleColor();
		ImGui::EndGroup();

		// Make card clickable
		ImGui::SetCursorScreenPos(card1Pos);
		ImGui::InvisibleButton("##NewProjectCard", ImVec2(cardWidth, cardHeight));
		if (ImGui::IsItemClicked())
		{
			memset(m_NewProjectName, 0, sizeof(m_NewProjectName));
			m_NewProjectLocation.clear();
			openNewProjectModal = true;
		}

		// Open Project Card
		ImVec2 card2Pos = ImVec2(cardStartPos.x + cardWidth + cardSpacing, cardStartPos.y);
		bool hoveredCard2 = ImGui::IsMouseHoveringRect(
			card2Pos,
			ImVec2(card2Pos.x + cardWidth, card2Pos.y + cardHeight)
		);

		ImU32 card2Color = hoveredCard2 ? IM_COL32(70, 70, 75, 255) : IM_COL32(60, 60, 65, 255);

		drawList->AddRectFilled(
			card2Pos,
			ImVec2(card2Pos.x + cardWidth, card2Pos.y + cardHeight),
			card2Color,
			8.0f
		);

		// Accent line
		drawList->AddRectFilled(
			card2Pos,
			ImVec2(card2Pos.x + cardWidth, card2Pos.y + 4),
			IM_COL32(120, 120, 130, 255),
			8.0f,
			ImDrawCornerFlags_Top
		);

		// Icon background
		ImVec2 iconCenter2 = ImVec2(card2Pos.x + cardWidth / 2, card2Pos.y + 60);
		drawList->AddCircleFilled(iconCenter2, 30, IM_COL32(120, 120, 130, 100), 32);

		// Draw folder icon (simplified)
		float folderSize = 20;
		drawList->AddRectFilled(
			ImVec2(iconCenter2.x - folderSize, iconCenter2.y - 5),
			ImVec2(iconCenter2.x + folderSize, iconCenter2.y + folderSize),
			IM_COL32(120, 120, 130, 255),
			3.0f
		);
		drawList->AddRectFilled(
			ImVec2(iconCenter2.x - folderSize, iconCenter2.y - 10),
			ImVec2(iconCenter2.x, iconCenter2.y - 5),
			IM_COL32(120, 120, 130, 255),
			3.0f,
			ImDrawCornerFlags_TopLeft
		);

		ImGui::SetCursorScreenPos(ImVec2(card2Pos.x + 20, card2Pos.y + 105));
		ImGui::BeginGroup();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		ImGui::SetWindowFontScale(1.2f);
		ImGui::Text("Open Project");
		ImGui::SetWindowFontScale(1.0f);
		ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.75f, 1.0f));
		ImGui::PushTextWrapPos(card2Pos.x + cardWidth - 20);
		ImGui::Text("Continue working on an existing project");
		ImGui::PopTextWrapPos();
		ImGui::PopStyleColor();
		ImGui::EndGroup();

		// Make card clickable
		ImGui::SetCursorScreenPos(card2Pos);
		ImGui::InvisibleButton("##OpenProjectCard", ImVec2(cardWidth, cardHeight));
		if (ImGui::IsItemClicked())
		{
			std::string filepath = FileDialogs::OpenFile("Teddy Project (*.teddyproj)\0*.teddyproj\0");
			if (!filepath.empty())
			{
				if (m_OpenProjectCallback)
					m_OpenProjectCallback(filepath);
			}
		}

		ImGui::EndGroup();

		ImGui::EndChild();

		if (openNewProjectModal)
			ImGui::OpenPopup("NewProjectModal");

		// === NEW PROJECT MODAL ===
		ImVec2 modalSize = ImVec2(600, 400);
		ImGui::SetNextWindowSize(modalSize, ImGuiCond_Always);
		ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(30, 30));
		ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0.0f, 0.0f, 0.0f, 0.7f));

		if (ImGui::BeginPopupModal("NewProjectModal", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
		{
			// Modal Header
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.95f, 1.0f));
			ImGui::SetWindowFontScale(1.4f);
			ImGui::Text("Create New Project");
			ImGui::SetWindowFontScale(1.0f);
			ImGui::PopStyleColor();

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.65f, 1.0f));
			ImGui::Text("Set up your project configuration");
			ImGui::PopStyleColor();

			ImGui::Spacing();
			ImGui::Spacing();

			ImDrawList* modalDrawList = ImGui::GetWindowDrawList();
			ImVec2 lineStart = ImGui::GetCursorScreenPos();
			modalDrawList->AddLine(
				lineStart,
				ImVec2(lineStart.x + modalSize.x - 60, lineStart.y),
				IM_COL32(60, 60, 65, 255),
				1.0f
			);

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();

			// Project Name Input
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.85f, 1.0f));
			ImGui::Text("Project Name");
			ImGui::PopStyleColor();
			ImGui::Spacing();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 10));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.18f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.2f, 0.2f, 0.23f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 0.25f, 0.28f, 1.0f));

			ImGui::SetNextItemWidth(-1);
			ImGui::InputTextWithHint("##ProjectName", "myawesomegame", m_NewProjectName, sizeof(m_NewProjectName),
				ImGuiInputTextFlags_CallbackCharFilter, LowercaseAlphanumericFilter);

			ImGui::PopStyleColor(3);
			ImGui::PopStyleVar();

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.55f, 1.0f));
			ImGui::Text("Only lowercase letters and numbers");
			ImGui::PopStyleColor();

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();

			// Project Location
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.85f, 1.0f));
			ImGui::Text("Project Location");
			ImGui::PopStyleColor();
			ImGui::Spacing();

			ImGui::BeginGroup();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 10));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.18f, 1.0f));

			char locationBuffer[512];
			strncpy(locationBuffer, m_NewProjectLocation.c_str(), sizeof(locationBuffer) - 1);
			locationBuffer[sizeof(locationBuffer) - 1] = '\0';

			ImGui::SetNextItemWidth(modalSize.x - 170);
			ImGui::InputText("##Location", locationBuffer, sizeof(locationBuffer), ImGuiInputTextFlags_ReadOnly);

			ImGui::PopStyleColor();
			ImGui::PopStyleVar();

			ImGui::SameLine();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 10));
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.25f, 0.28f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.33f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.23f, 1.0f));

			if (ImGui::Button("Browse", ImVec2(90, 0)))
			{
				std::string folder = FileDialogs::OpenFolder();
				if (!folder.empty())
					m_NewProjectLocation = folder;
			}

			ImGui::PopStyleColor(3);
			ImGui::PopStyleVar();

			ImGui::EndGroup();

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();

			// Preview Section
			ImVec2 previewBoxStart = ImGui::GetCursorScreenPos();
			ImVec2 previewBoxSize = ImVec2(modalSize.x - 60, 80);

			bool hasValidInput = strlen(m_NewProjectName) > 0 && !m_NewProjectLocation.empty();

			ImU32 previewBgColor = hasValidInput ? IM_COL32(30, 60, 40, 255) : IM_COL32(60, 40, 40, 255);
			modalDrawList->AddRectFilled(
				previewBoxStart,
				ImVec2(previewBoxStart.x + previewBoxSize.x, previewBoxStart.y + previewBoxSize.y),
				previewBgColor,
				6.0f
			);

			ImGui::SetCursorScreenPos(ImVec2(previewBoxStart.x + 20, previewBoxStart.y + 15));
			ImGui::BeginGroup();

			if (hasValidInput)
			{
				std::filesystem::path fullPath = std::filesystem::path(m_NewProjectLocation) / m_NewProjectName;
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.9f, 0.5f, 1.0f));
				ImGui::Text("Project will be created at:");
				ImGui::PopStyleColor();
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.85f, 1.0f));
				ImGui::PushTextWrapPos(previewBoxStart.x + previewBoxSize.x - 20);
				ImGui::TextWrapped("%s", fullPath.string().c_str());
				ImGui::PopTextWrapPos();
				ImGui::PopStyleColor();
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.5f, 0.5f, 1.0f));
				ImGui::Text("Missing Information");
				ImGui::PopStyleColor();
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.75f, 1.0f));
				ImGui::Text("Please provide both project name and location");
				ImGui::PopStyleColor();
			}

			ImGui::EndGroup();
			ImGui::SetCursorScreenPos(ImVec2(previewBoxStart.x, previewBoxStart.y + previewBoxSize.y + 20));

			// Action Buttons
			ImGui::Spacing();
			float buttonWidth = 120.0f;
			float availWidth = modalSize.x - 60;
			ImGui::SetCursorPosX((modalSize.x - (buttonWidth * 2 + 15)) / 2);

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 12));

			if (hasValidInput)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.3f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.7f, 0.4f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.55f, 0.25f, 1.0f));
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 0.6f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.6f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 0.6f));
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 0.7f));
			}

			if (ImGui::Button("Create Project", ImVec2(buttonWidth, 0)) && hasValidInput)
			{
				std::filesystem::path projectPath = std::filesystem::path(m_NewProjectLocation) / m_NewProjectName;
				Application::Get().NewProject(projectPath);
				if (m_OpenProjectCallback)
					m_OpenProjectCallback(projectPath / (std::string(m_NewProjectName) + ".teddyproj"));
				ImGui::CloseCurrentPopup();
			}

			ImGui::PopStyleColor(hasValidInput ? 3 : 4);

			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.43f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.53f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.35f, 0.35f, 0.38f, 1.0f));

			if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::PopStyleColor(3);
			ImGui::PopStyleVar();

			ImGui::EndPopup();
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);

		ImGui::End();
	}
}