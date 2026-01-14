#include "LI.h"

#include "imgui/imgui.h"



class RendererLayer : public LI::Layer 
{
public:
	RendererLayer()
		: Layer("Renderer"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
	{
	}

	virtual void OnAttach() override
	{
		m_VertexArray.reset(LI::VertexArray::Create());
		m_VertexArray->Bind();

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		m_VertexBuffer.reset(LI::VertexBuffer::Create(vertices, sizeof(vertices)));
		LI::BufferLayout layout = {
			{LI::ShaderDataType::Float3, "a_Postion"},
			{LI::ShaderDataType::Float4, "a_Color"}
		};
		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		m_IndexBuffer.reset(LI::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;
			uniform mat4 u_ViewProjection;
			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
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

		m_Shader.reset(new LI::Shader(vertexSrc, fragmentSrc));
	}

	virtual void OnUpdate() override
	{
		LI::RenderCommand::SetClearColor({ 0.1f,0.1f,0.1f,1 });
		LI::RenderCommand::Clear();

		m_Camera.SetPosition({ 0.5f, 0.5f, 0.0f });
		m_Camera.SetRotation(45.0f);

		LI::Renderer::BeginScene(m_Camera);
		m_Shader->Bind();
		LI::Renderer::Submit(m_Shader, m_VertexArray);
		LI::Renderer::EndScene();
	}

private:
	std::shared_ptr<LI::VertexArray> m_VertexArray;
	std::shared_ptr<LI::VertexBuffer> m_VertexBuffer;
	std::shared_ptr<LI::IndexBuffer> m_IndexBuffer;
	std::shared_ptr<LI::Shader> m_Shader;
	LI::OrthographicCamera m_Camera;
};


class SandboxApp : public LI::Application
{
	public:
	SandboxApp() 
	{

		PushLayer(std::make_unique<RendererLayer>());
	}
	~SandboxApp() {}
};

LI::Application* LI::CreateApplication()
{
	return new SandboxApp();
}