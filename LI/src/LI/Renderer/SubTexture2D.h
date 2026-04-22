#pragma once

#include "Texture.h"
#include <glm/glm.hpp>

namespace LI {

	class SubTexture2D
	{
	public:
		// min/max 为归一化 UV 坐标（0~1）
		SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);

		const Ref<Texture2D>& GetTexture() const { return m_Texture; }
		const glm::vec2* GetTexCoords() const { return m_TexCoords; }

		// coords    : sprite 在图集中的网格坐标（列, 行），从左下角起
		// cellSize  : 单个格子的像素尺寸
		// spriteSize: sprite 占几个格子（默认 1x1）
		static Ref<SubTexture2D> CreateFromCoords(
			const Ref<Texture2D>& texture,
			const glm::vec2& coords,
			const glm::vec2& cellSize,
			const glm::vec2& spriteSize = { 1.0f, 1.0f });

	private:
		Ref<Texture2D> m_Texture;
		glm::vec2 m_TexCoords[4]; // 左下 → 右下 → 右上 → 左上
	};

}
