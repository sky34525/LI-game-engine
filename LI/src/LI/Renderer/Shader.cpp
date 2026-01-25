#include "pch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace LI {

	Shader* Shader::CreateFromFiles(const std::string& vertexPath, const std::string& fragmentPath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    LI_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return new OpenGLShader(vertexPath, fragmentPath, true);
		}

		LI_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Shader* Shader::CreateFromSource(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			LI_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return new OpenGLShader(vertexSrc, fragmentSrc);
		}

		LI_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}