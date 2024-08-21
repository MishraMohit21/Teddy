#include <Teddy.h>
#include <Platform/OpenGL/OpenGLShader.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

class ExampleLayer : public Teddy::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f)
	{
		m_VertexArray.reset(Teddy::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		m_CameraPositionDefault = m_CameraPosition;

		Teddy::Ref<Teddy::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Teddy::VertexBuffer::Create(vertices, sizeof(vertices)));
		Teddy::BufferLayout layout = {
			{ Teddy::ShaderDataType::Float3, "a_Position" },
			{ Teddy::ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		Teddy::Ref<Teddy::IndexBuffer> indexBuffer;
		indexBuffer.reset(Teddy::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(Teddy::VertexArray::Create());

		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f,	1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		Teddy::Ref<Teddy::VertexBuffer> squareVB;
		squareVB.reset(Teddy::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ Teddy::ShaderDataType::Float3, "a_Position" },
			{ Teddy::ShaderDataType::Float2, "a_TexCoord" }

			});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Teddy::Ref<Teddy::IndexBuffer> squareIB;
		squareIB.reset(Teddy::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		m_Shader = Teddy::Shader::Create("./assests/Shader/m_Shader.glsl");


		std::string flatColorShaderVertexSrc = R"(
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

		std::string flatColorShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			
			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

		m_FlatColorShader = Teddy::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);


		//m_FlatColorShader = Teddy::Shader::Create("./assests/Shader/flatColorShader.glsl");
		//std::dynamic_pointer_cast<Teddy::OpenGLShader>(m_FlatColorShader)->Bind();


		m_TextureShader = Teddy::Shader::Create("./assests/Shader/Texture.glsl");

		m_Texture = Teddy::Texture2D::Create("./assests/textures/Checkerboard.png");

		m_ChernoLogo = Teddy::Texture2D::Create("./assests/textures/ladka.png");

		std::dynamic_pointer_cast<Teddy::OpenGLShader>(m_TextureShader)->Bind();
		std::dynamic_pointer_cast<Teddy::OpenGLShader>(m_TextureShader)->UploadUniformInt("u_Texture", 0);


		
	}

	void OnUpdate(Teddy::Timestep ts) override
	{
		if (Teddy::Input::IsKeyPressed(TD_KEY_LEFT))
			m_CameraPosition.x += m_CameraMoveSpeed * ts;
		else if (Teddy::Input::IsKeyPressed(TD_KEY_RIGHT))
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;

		if (Teddy::Input::IsKeyPressed(TD_KEY_UP))
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;
		else if (Teddy::Input::IsKeyPressed(TD_KEY_DOWN))
			m_CameraPosition.y += m_CameraMoveSpeed * ts;

		if (Teddy::Input::IsKeyPressed(TD_KEY_A))
			m_CameraRotation -= m_CameraRotationSpeed * ts;
		if (Teddy::Input::IsKeyPressed(TD_KEY_D))
			m_CameraRotation += m_CameraRotationSpeed * ts;

		Teddy::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Teddy::RendererCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Teddy::Renderer::BeginScene(m_Camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));


		std::dynamic_pointer_cast<Teddy::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);


		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Teddy::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}


		Teddy::Renderer::Submit(m_Shader, m_VertexArray, glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, 0.0f)));

		// Jo bhi  texture bind ho rakha hoga woh render hoga
		m_Texture->Bind();
		Teddy::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.1f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.5f, 0.0f)));
		


		m_ChernoLogo->Bind(); 
		Teddy::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.1f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.5f, 0.0f)));



		Teddy::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		if (ImGui::Button("Reset Rotation"))
		{
			m_CameraRotation = 0.0f;
		}

		if (ImGui::Button("Reset Transform"))
		{
			m_CameraPosition = m_CameraPositionDefault;
		}

		//ImGui::SliderFloat("Scale of Square", &scale, 0.0, 0.1f);
		ImGui::End();
	}

	void OnEvent(Teddy::Event& event) override
	{
	}

private:
	Teddy::Ref<Teddy::Shader> m_Shader;
	Teddy::Ref<Teddy::VertexArray> m_VertexArray;

	Teddy::Ref <Teddy::Texture2D> m_Texture, m_ChernoLogo;
	
	Teddy::Ref<Teddy::Shader> m_FlatColorShader, m_TextureShader;
	Teddy::Ref<Teddy::VertexArray> m_SquareVA;

	Teddy::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	glm::vec3 m_CameraPositionDefault;
	float m_CameraMoveSpeed = 5.0f;
	float scale = 0.1f;

	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 180.0f;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
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