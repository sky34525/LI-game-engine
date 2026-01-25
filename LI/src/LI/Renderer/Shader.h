#pragma once

#include <string>
#include <glm/glm.hpp>

namespace LI {
	class Shader
	{
	public:
		virtual ~Shader() = default;
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		static Shader* CreateFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
		static Shader* CreateFromSource(const std::string& vertexSrc, const std::string& fragmentSrc);
	};
}

