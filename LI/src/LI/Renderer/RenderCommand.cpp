#include "pch.h"

#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace LI {
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}