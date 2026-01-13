#include "pch.h"

#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace LI {
	RendererAPI* RenderCommand::m_RendererAPI = new OpenGLRendererAPI;
}