#include "Sandbox2D.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
	TD_PROFILE_FUNCTION();

	m_CheckerboardTexture = Teddy::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
	TD_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Teddy::Timestep ts)
{
	TD_PROFILE_FUNCTION();

	// Update
	m_CameraController.OnUpdate(ts);

	// Render
	{
		TD_PROFILE_SCOPE("Renderer Prep");
		Teddy::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Teddy::RenderCommand::Clear();
	}

	{
		TD_PROFILE_SCOPE("Renderer Draw");
		Teddy::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Teddy::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, m_SquareColor );
		Teddy::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, m_SquareColor );
		Teddy::Renderer2D::DrawRotatedQuad({ 1.5f, -0.5f }, { 0.5f, 0.5f }, 45.0, m_SquareColor);
		Teddy::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture, 10.0f /*, { 0.2f, 0.3f, 0.8f, 0.3f }*/);
		Teddy::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	TD_PROFILE_FUNCTION();

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Teddy::Event& e)
{
	m_CameraController.OnEvent(e);
}