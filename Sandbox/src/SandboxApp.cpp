#include <Teddy.h>

#include <imgui/imgui.h>

class ExampleLayer : public Teddy::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPos(0.0f)
	{
		m_VertexArray.reset(Teddy::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.8f, 0.0f, 0.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.0f, 0.0f, 1.0f
		};



		std::shared_ptr<Teddy::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Teddy::VertexBuffer::Create(vertices, sizeof(vertices)));
		Teddy::BufferLayout layout = {
			{ Teddy::ShaderDataType::Float3, "a_Position" },
			{ Teddy::ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<Teddy::IndexBuffer> indexBuffer;
		indexBuffer.reset(Teddy::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(Teddy::VertexArray::Create());

		float squareVertices[3 * 4] = {
			-0.75f, -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};

		std::shared_ptr<Teddy::VertexBuffer> squareVB;
		squareVB.reset(Teddy::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ Teddy::ShaderDataType::Float3, "a_Position" }
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Teddy::IndexBuffer> squareIB;
		squareIB.reset(Teddy::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		m_Shader.reset(new Teddy::Shader(vertexSrc, fragmentSrc));

		std::string blueShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		std::string blueShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

		m_BlueShader.reset(new Teddy::Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
	}

	void OnUpdate(Teddy::Timestep ts) override
	{

		if (Teddy::Input::IsKeyPressed(TD_KEY_RIGHT))
		{
			m_CameraPos.x -= m_CameraMoveSpeed * ts;
		}
		else if (Teddy::Input::IsKeyPressed(TD_KEY_LEFT))
		{
			m_CameraPos.x += m_CameraMoveSpeed * ts;
		}
		if (Teddy::Input::IsKeyPressed(TD_KEY_UP))
		{
			m_CameraPos.y -= m_CameraMoveSpeed * ts;
		}
		else if (Teddy::Input::IsKeyPressed(TD_KEY_DOWN))
		{
			m_CameraPos.y += m_CameraMoveSpeed * ts;
		}


		if (Teddy::Input::IsKeyPressed(TD_KEY_A))
		{
			m_CameraRotation -= m_CameraRotationSpeed * ts;
		}
		else if (Teddy::Input::IsKeyPressed(TD_KEY_D))
		{
			m_CameraRotation += m_CameraRotationSpeed * ts;
		}


		Teddy::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Teddy::RendererCommand::Clear();

		m_Camera.SetPosition(m_CameraPos);
		m_Camera.SetRotation(m_CameraRotation);

		Teddy::Renderer::BeginScene(m_Camera);

		Teddy::Renderer::Submit(m_BlueShader, m_SquareVA);
		Teddy::Renderer::Submit(m_Shader, m_VertexArray);

		Teddy::Renderer::EndScene();
	}

	void OnEvent(Teddy::Event& event) override
	{
	}


	virtual void OnImGuiRender() override
	{
	}

private:
	std::shared_ptr<Teddy::Shader> m_Shader;
	std::shared_ptr<Teddy::VertexArray> m_VertexArray;
	std::shared_ptr<Teddy::Shader> m_BlueShader;
	std::shared_ptr<Teddy::VertexArray> m_SquareVA;

	Teddy::	OrthographicCamera m_Camera;
	glm::vec3 m_CameraPos;
	float m_CameraMoveSpeed = 5.0f;

	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 180.0f;

};

class Sandbox : public Teddy::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		
	}

	~Sandbox()
	{

	}

};

Teddy::Application* Teddy::CreateApplication()
{
	return new Sandbox();
}