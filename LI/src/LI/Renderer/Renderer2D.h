#pragma once

#include "OrthographicCamera.h"
#include "Texture.h"

namespace LI {

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void Flush();

		// 纯色 quad
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);

		// 纹理 quad（tilingFactor 控制贴图重复次数，默认 1.0）
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f);

		// 统计信息（用于性能调试）
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
		};
		static Statistics GetStats();
		static void ResetStats();

	private:
		static void FlushAndReset();
	};
}
