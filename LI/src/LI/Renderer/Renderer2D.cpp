#include "pch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace LI {

	// -------------------------------------------------------
	// 每个顶点携带的数据（批处理的关键：变换预先算进 Position）
	// -------------------------------------------------------
	struct QuadVertex
	{
		glm::vec3 Position;    // 世界坐标
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float     TexIndex;    // 用哪张纹理（0 = 白色纯色）
		float     TilingFactor;//纹理重复次数
	};

	// -------------------------------------------------------
	// 批处理参数上限
	// -------------------------------------------------------
	struct Renderer2DData
	{
		static const uint32_t MaxQuads        = 10000;
		static const uint32_t MaxVertices     = MaxQuads * 4;
		static const uint32_t MaxIndices      = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32;

		Ref<VertexArray>  QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;  // 动态 VBO
		Ref<Texture2D>    WhiteTexture;
		Ref<Shader>       TextureShader;

		// CPU 端缓冲区：每帧往这里写，EndScene 时上传到 GPU
		QuadVertex* QuadVertexBufferBase = nullptr; // 缓冲区起始
		QuadVertex* QuadVertexBufferPtr  = nullptr; // 当前写入位置

		uint32_t QuadIndexCount = 0; // 本批次累计的索引数

		// 纹理槽：最多同时使用 MaxTextureSlots 张纹理
		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 槽 0 留给白色纹理

		// 单位 quad 的 4 个顶点本地坐标（左下→右下→右上→左上）
		glm::vec4 QuadVertexPositions[4];

		Renderer2D::Statistics Stats;
	};

	//static的作用：
	// 限制可见性，Rendere2DData在设计上仅能被该文件访问
	// 控制生命周期，程序启动时分配，退出时销毁。
	static Renderer2DData s_Data;

	// -------------------------------------------------------
	// Init：创建 VAO、动态 VBO、预填充 IBO、白色纹理、Shader
	// -------------------------------------------------------
	void Renderer2D::Init()
	{
		s_Data.QuadVertexArray = VertexArray::Create();

		// 动态 VBO：只分配空间，数据每帧通过 SetData 上传，只需要分配一次
		s_Data.QuadVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxVertices * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"     },
			{ ShaderDataType::Float4, "a_Color"        },
			{ ShaderDataType::Float2, "a_TexCoord"     },
			{ ShaderDataType::Float,  "a_TexIndex"     },
			{ ShaderDataType::Float,  "a_TilingFactor" }
		});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		// CPU 缓冲区：用 new 分配，Shutdown 时释放
		s_Data.QuadVertexBufferBase = new QuadVertex[Renderer2DData::MaxVertices];

		// 静态 IBO：所有 quad 的索引规律完全一样，提前填好
		// 在创建IBO时按最大规格创建，在DrawIndexed时不会将所有的IBO传入，只会传入实际使用的索引
		// quad 0: 0,1,2, 2,3,0   quad 1: 4,5,6, 6,7,4  ...
		uint32_t* quadIndices = new uint32_t[Renderer2DData::MaxIndices];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < Renderer2DData::MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;
			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;
			offset += 4;
		}
		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, Renderer2DData::MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		// 白色 1x1 纹理，用于纯色 quad（乘以白色 = 不改变颜色）
		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		// 初始化纹理采样器：告诉 shader 每个槽对应哪个纹理单元（实际就是上传了一个数组到shader中）
		int32_t samplers[Renderer2DData::MaxTextureSlots];
		for (uint32_t i = 0; i < Renderer2DData::MaxTextureSlots; i++)
			samplers[i] = i;

		s_Data.TextureShader = Shader::Create("assets/shaders/QuadVs.glsl", "assets/shaders/QuadFs.glsl");
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetIntArray("u_Textures", samplers, Renderer2DData::MaxTextureSlots);

		// 槽 0 固定绑定白色纹理
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		// 单位 quad 四个顶点的本地坐标（以原点为中心的 1x1 正方形）
		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	//s_Data的其他资源都由动态指针管理，程序结束时自动释放，因此只需要手动释放new的QuadVertex数组
	void Renderer2D::Shutdown()
	{
		delete[] s_Data.QuadVertexBufferBase;
	}

	// -------------------------------------------------------
	// BeginScene：绑定 shader，设置 ViewProjection
	// -------------------------------------------------------
	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		// 每次 BeginScene 重置缓冲区写入指针和索引计数
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureSlotIndex = 1;
	}

	// -------------------------------------------------------
	// Flush：把 CPU 缓冲区上传到 GPU，发出一次 Draw Call
	// -------------------------------------------------------
	void Renderer2D::Flush()
	{
		if (s_Data.QuadIndexCount == 0)
			return;

		// 计算本批次实际写入了多少字节
		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

		// 绑定所有用到的纹理
		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			s_Data.TextureSlots[i]->Bind(i);

		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
		s_Data.Stats.DrawCalls++;
	}

	// -------------------------------------------------------
	// EndScene：触发最后一批的 Flush
	// -------------------------------------------------------
	void Renderer2D::EndScene()
	{
		Flush();
	}

	// -------------------------------------------------------
	// FlushAndReset：缓冲区满时，先 Flush，再重置准备下一批
	// -------------------------------------------------------
	void Renderer2D::FlushAndReset()
	{
		Flush();
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureSlotIndex = 1;
	}

	// -------------------------------------------------------
	// DrawQuad 内部实现（vec3 版本，其他重载都转发到这里）
	// -------------------------------------------------------
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		// 缓冲区满了就先提交一次
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

		constexpr float texIndex    = 0.0f; // 槽 0 = 白色纹理
		constexpr float tilingFactor = 1.0f;
		constexpr glm::vec2 texCoords[] = { {0,0}, {1,0}, {1,1}, {0,1} };

		// 计算变换矩阵，将本地顶点坐标变换到世界坐标
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		// 写入 4 个顶点到 CPU 缓冲区
		for (int i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position    = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color       = color;
			s_Data.QuadVertexBufferPtr->TexCoord    = texCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex    = texIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor)
	{
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

		// 查找该纹理是否已经在槽里
		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (s_Data.TextureSlots[i].get() == texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		// 没找到：分配一个新槽
		if (textureIndex == 0.0f)
		{
			if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				FlushAndReset();

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		constexpr glm::vec2 texCoords[] = { {0,0}, {1,0}, {1,1}, {0,1} };

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (int i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position    = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color       = color;
			s_Data.QuadVertexBufferPtr->TexCoord    = texCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex    = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor);
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}
}
