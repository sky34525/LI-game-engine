#include <LI.h>
#include <LI/Core/EntryPoint.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"
class Sandbox : public LI::Application
{
public:
	Sandbox()
	{
		PushLayer(std::make_unique<Sandbox2D>());
	}

	~Sandbox()
	{

	}

};

LI::Application* LI::CreateApplication()
{
	return new Sandbox();
}