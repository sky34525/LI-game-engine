#pragma once

#include "LI/Renderer/Shader.h"

#include <glm/glm.hpp>


// TODO: REMOVE!
typedef unsigned int GLenum;

namespace LI {
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexfile, const std::string& fragmentfile, bool fromFile);
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~OpenGLShader();
		void Bind() const;
		void UnBind() const;

		void UploadUniformInt(const std::string& name, int value);

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
	private:
		std::string ReadFile(const std::string& filepath);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
	private:
		uint32_t m_RendererID;
	};
}
