#include <LI.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public LI::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_CameraController(1280.0f / 720.0f)
	{
		m_VertexArray.reset(LI::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		LI::Ref<LI::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(LI::VertexBuffer::Create(vertices, sizeof(vertices)));
		LI::BufferLayout layout = {
			{ LI::ShaderDataType::Float3, "a_Position" },
			{ LI::ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<LI::IndexBuffer> indexBuffer;
		indexBuffer.reset(LI::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(LI::VertexArray::Create());

		float squareVertices1[3 * 4] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};

		float squareVertices2[2 * 4] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f
		};

		LI::Ref<LI::VertexBuffer> squareVB1, squareVB2;
		squareVB1.reset(LI::VertexBuffer::Create(squareVertices1, sizeof(squareVertices1)));
		squareVB2.reset(LI::VertexBuffer::Create(squareVertices2, sizeof(squareVertices2)));

		squareVB1->SetLayout({
			{ LI::ShaderDataType::Float3, "a_Position" },
			});

		squareVB2->SetLayout({
			{ LI::ShaderDataType::Float2, "a_TexCoord" }
			});

		m_SquareVA->AddVertexBuffer(squareVB1);
		m_SquareVA->AddVertexBuffer(squareVB2);


		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<LI::IndexBuffer> squareIB;
		squareIB.reset(LI::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		m_TextureShader = LI::Shader::Create("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");

		m_Texture = LI::Texture2D::Create("assets/textures/Checkerboard.png");

		m_LILogoTexture = LI::Texture2D::Create("assets/textures/blueL.png");


		std::dynamic_pointer_cast<LI::OpenGLShader>(m_TextureShader)->Bind();
	}

	void OnUpdate(LI::Timestep ts) override
	{
		m_CameraController.OnUpdate(ts);

		LI::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		LI::RenderCommand::Clear();


		LI::Renderer::BeginScene(m_CameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));


		m_Texture->Bind(0); //TODO:into Submit
		LI::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		m_LILogoTexture->Bind(0);
		LI::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		LI::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(LI::Event& event) override
	{
		m_CameraController.OnEvent(event);
	}
private:
	LI::ShaderLibrary m_ShaderLibrary;
	LI::Ref<LI::Shader> m_Shader;
	LI::Ref<LI::VertexArray> m_VertexArray;
	LI::Ref<LI::Shader> m_TextureShader;
	LI::Ref<LI::VertexArray> m_SquareVA;

	LI::OrthographicCameraController m_CameraController;


	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };

	LI::Ref<LI::Texture2D> m_Texture, m_LILogoTexture;
};

class Sandbox : public LI::Application
{
public:
	Sandbox()
	{
		PushLayer(std::make_unique<ExampleLayer>());
	}

	~Sandbox()
	{

	}

};

LI::Application* LI::CreateApplication()
{
	return new Sandbox();
}