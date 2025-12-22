#include "LI.h"

class ExampleLayer : public LI::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{

	}

	void OnEvent(LI::Event& event) override
	{
	}
};

class SandboxApp : public LI::Application
{
	public:
	SandboxApp() 
	{
		PushLayer(std::make_unique<ExampleLayer>());
		PushOverlay(std::make_unique<LI::ImGuiLayer>());
	}
	~SandboxApp() {}
};

LI::Application* LI::CreateApplication()
{
	return new SandboxApp();
}