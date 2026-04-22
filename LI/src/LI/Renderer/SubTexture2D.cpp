#include "pch.h"
#include "SubTexture2D.h"

namespace LI {

	SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max)
		: m_Texture(texture)
	{
		m_TexCoords[0] = { min.x, min.y }; // 左下
		m_TexCoords[1] = { max.x, min.y }; // 右下
		m_TexCoords[2] = { max.x, max.y }; // 右上
		m_TexCoords[3] = { min.x, max.y }; // 左上
	}

	Ref<SubTexture2D> SubTexture2D::CreateFromCoords(
		const Ref<Texture2D>& texture,
		const glm::vec2& coords,
		const glm::vec2& cellSize,
		const glm::vec2& spriteSize)
	{
		float texWidth  = (float)texture->GetWidth();
		float texHeight = (float)texture->GetHeight();

		glm::vec2 min = { (coords.x * cellSize.x) / texWidth,
		                  (coords.y * cellSize.y) / texHeight };
		glm::vec2 max = { ((coords.x + spriteSize.x) * cellSize.x) / texWidth,
		                  ((coords.y + spriteSize.y) * cellSize.y) / texHeight };

		return CreateRef<SubTexture2D>(texture, min, max);
	}

}
