#include "pch.h"

#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace LI {
	Scope<RendererAPI> RenderCommand::s_RendererAPI = CreateScope<OpenGLRendererAPI>();
}