#include "pch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace LI {

	Ref<Shader> Shader::Create(const std::string& vertexPath, const std::string& fragmentPath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    LI_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLShader>(vertexPath, fragmentPath);
		}

		LI_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			LI_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}

		LI_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		LI_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	LI::Ref<LI::Shader> ShaderLibrary::Load(const std::string& vertexPath, const std::string& fragmentPath)
	{
		auto shader = Shader::Create(vertexPath, fragmentPath);
		Add(shader);
		return shader;
	}

	LI::Ref<LI::Shader> ShaderLibrary::Load(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath)
	{
		auto shader = Shader::Create(name, vertexPath, fragmentPath);
		Add(name, shader);
		return shader;
	}

	LI::Ref<LI::Shader> ShaderLibrary::Get(const std::string& name)
	{
		LI_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}

}