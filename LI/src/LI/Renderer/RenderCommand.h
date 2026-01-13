#pragma once

#include "RendererAPI.h"

namespace LI {

	class RenderCommand
	{
	public:
		inline static void SetClearColor(const glm::vec4& color)
		{
			m_RendererAPI->SetClearColor(color);
		}

		inline static void Clear()
		{
			m_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
		{
			m_RendererAPI->DrawIndexed(vertexArray);
		}

	private:
		static RendererAPI* m_RendererAPI;
	};
}