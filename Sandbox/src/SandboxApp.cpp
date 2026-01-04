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
		if (LI::Input::IsKeyPressed(HZ_KEY_TAB))
			LI_TRACE("Tab key is pressed (poll)!");
	}

	void OnEvent(LI::Event& event) override
	{
		if (event.GetEventType() == LI::EventType::KeyPressed)
		{			
			LI::KeyPressedEvent& e = (LI::KeyPressedEvent&)event;
			if (e.GetKeyCode() == HZ_KEY_TAB)
				LI_TRACE("Tab key is pressed (event)!");
			LI_TRACE("{0}", (char)e.GetKeyCode());
		}
	}
};

class SandboxApp : public LI::Application
{
	public:
	SandboxApp() 
	{
		PushLayer(std::make_unique<ExampleLayer>());
	}
	~SandboxApp() {}
};

LI::Application* LI::CreateApplication()
{
	return new SandboxApp();
}