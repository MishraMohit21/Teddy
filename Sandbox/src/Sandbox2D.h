#pragma once

#include "Teddy.h"

class Sandbox2D : public Teddy::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(Teddy::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Teddy::Event& e) override;
private:
	Teddy::OrthographicCameraController m_CameraController;
	
	// Temp
	Teddy::Ref<Teddy::VertexArray> m_SquareVA;
	Teddy::Ref<Teddy::Shader> m_FlatColorShader;

	Teddy::Ref<Teddy::Texture2D> m_CheckerboardTexture;
	Teddy::Ref<Teddy::Texture2D> m_SpriteSheet;
	Teddy::Ref<Teddy::FrameBuffer> m_FrameBuffer;


	float m_RotationAngle = 45.0;
	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };


};