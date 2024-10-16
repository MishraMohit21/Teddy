#include "EditorLayer.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace Teddy
{

    EditorLayer::EditorLayer()
        : Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f)
    {
    }

    void EditorLayer::OnAttach()
    {
        TD_PROFILE_FUNCTION();

        m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");


        FrameBufferSpecification m_specs;
        m_specs.Height = 720.0f;
        m_specs.Width = 1280.0f;
        m_FrameBuffer = FrameBuffer::Create(m_specs);

        m_ActiveScene = CreateRef<Scene>();

        auto square = m_ActiveScene->CreateEntity();

        m_ActiveScene->getReg().emplace<TransformComponent>(square);
        m_ActiveScene->getReg().emplace<SpriteRendererComponent>(square, glm::vec4{ 0.0f, 1.0f, 1.0f, 1.0f });
        
        m_SquareEntity = square;
    }

    void EditorLayer::OnDetach()
    {
        TD_PROFILE_FUNCTION();
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        TD_PROFILE_FUNCTION();

        // Update
        if (m_ViewportFoc)
        {
            m_CameraController.OnUpdate(ts);
        }


        // Render
        Renderer2D::ResetStats();
        m_FrameBuffer->bind();
        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        RenderCommand::Clear();
        

        
        Renderer2D::BeginScene(m_CameraController.GetCamera());

        m_ActiveScene->OnUpdate(ts);

        Renderer2D::EndScene();

        m_FrameBuffer->unBind();

    }

    void EditorLayer::OnImGuiRender()
    {
        TD_PROFILE_FUNCTION();



        static bool dock_enable = true;
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->GetWorkPos());
            ImGui::SetNextWindowSize(viewport->GetWorkSize());
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dock_enable, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.


                if (ImGui::MenuItem("Eixt")) Application::Get().Close();

                ImGui::EndMenu();
            }


            ImGui::EndMenuBar();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("ViewPort");

        m_ViewportFoc = ImGui::IsWindowFocused();
        m_ViewportHov = ImGui::IsWindowHovered();

        Application::Get().GetImGuiLayer()->BlockEvent(!m_ViewportFoc || !m_ViewportHov);
        

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

        if (m_ViewPortSize != *((glm::vec2*)&viewportPanelSize))
        {
            m_ViewPortSize = { viewportPanelSize.x, viewportPanelSize.y };
            m_FrameBuffer->NewSize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
            m_CameraController.OnResize(viewportPanelSize.x, viewportPanelSize.y);
        }

        auto textureID = m_FrameBuffer->GetColorAttachmentRendererID();
        ImGui::Image((void*)textureID, ImVec2{ m_ViewPortSize.x, m_ViewPortSize.y }, {0, 1}, {1, 0});
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::Begin("Settings");

        auto stats = Renderer2D::GetStats();
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %d", stats.GetTotalIndexCount());


        auto& squareColor = m_ActiveScene->getReg().get<SpriteRendererComponent>(m_SquareEntity).Color;
        ImGui::ColorEdit4("Square Color", glm::value_ptr(squareColor));



        ImGui::End();


        ImGui::End();


    }

    void EditorLayer::OnEvent(Event& e)
    {
        m_CameraController.OnEvent(e);
    }

}