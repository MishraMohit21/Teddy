// Teddy\src\Teddy\Renderer\Renderer2D.cpp
#include "tdpch.h"
#include "Teddy/Renderer/Renderer2D.h"

#include "Trex/Atlas.hpp"
#include "Trex/TextShaper.hpp"

#include "Teddy/Renderer/VertexArray.h"
#include "Teddy/Renderer/Shader.h"
#include "Teddy/Renderer/UniformBuffer.h"
#include "Teddy/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Teddy {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;

		// Editor-only
		int EntityID;
	};

	struct CircleVertex
	{
		glm::vec3 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;

		// Editor-only
		int EntityID;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		// Editor-only
		int EntityID;
	};

	struct TextVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;

		// Editor-only
		int EntityID;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps


		// Quad Data
		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> QuadShader;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		// Circle Data
		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;
		Ref<Shader> CircleShader;

		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;

		//// Line Data
		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;
		uint32_t LineVertexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;
		float LineWidth = 5.0f;

		// Text Data
		Ref<VertexArray> TextVertexArray;
		Ref<VertexBuffer> TextVertexBuffer;
		Ref<Shader> TextShader;
		uint32_t TextIndexCount = 0;
		TextVertex* TextVertexBufferBase = nullptr;
		TextVertex* TextVertexBufferPtr = nullptr;

		Ref<Texture2D> WhiteTexture;
		Ref<Font> DefaultFont;
		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		glm::vec4 QuadVertexPositions[4];

		Renderer2D::Statistics Stats;

		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
	};

	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		TD_PROFILE_FUNCTION();

		s_Data.QuadVertexArray = VertexArray::Create();

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"     },
			{ ShaderDataType::Float4, "a_Color"        },
			{ ShaderDataType::Float2, "a_TexCoord"     },
			{ ShaderDataType::Float,  "a_TexIndex"     },
			{ ShaderDataType::Float,  "a_TilingFactor" },
			{ ShaderDataType::Int,    "a_EntityID"     }
			});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		// Cirlces Data UploadSetup
		s_Data.CircleVertexArray = VertexArray::Create();

		s_Data.CircleVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex));
		s_Data.CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_WorldPosition"     },
			{ ShaderDataType::Float3, "a_LocalPosition"     },
			{ ShaderDataType::Float4, "a_Color"        },
			{ ShaderDataType::Float,  "a_Thickness"     },
			{ ShaderDataType::Float,  "a_Fade" },
			{ ShaderDataType::Int,    "a_EntityID"     }
			});
		s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);
		s_Data.CircleVertexArray->SetIndexBuffer(quadIB);
		s_Data.CircleVertexBufferBase = new CircleVertex[s_Data.MaxVertices];


		//Line Data Setup
		s_Data.LineVertexArray = VertexArray::Create();
		s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(LineVertex));
		s_Data.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    },
			{ ShaderDataType::Int,    "a_EntityID" }
			});
		s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
		s_Data.LineVertexBufferBase = new LineVertex[s_Data.MaxVertices];

		// Text Data
		s_Data.TextVertexArray = VertexArray::Create();
		s_Data.TextVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(TextVertex));
		s_Data.TextVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"     },
			{ ShaderDataType::Float4, "a_Color"        },
			{ ShaderDataType::Float2, "a_TexCoord"     },
			{ ShaderDataType::Float,  "a_TexIndex"     },
			{ ShaderDataType::Int,    "a_EntityID"     }
		});
		s_Data.TextVertexArray->AddVertexBuffer(s_Data.TextVertexBuffer);
		s_Data.TextVertexArray->SetIndexBuffer(quadIB); // Reuse quad index buffer
		s_Data.TextVertexBufferBase = new TextVertex[s_Data.MaxVertices];


		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;

		s_Data.TextShader = Shader::Create("assets/shaders/TextShader.glsl");
		s_Data.TextShader->Bind();
		s_Data.TextShader->SetIntArray("u_FontAtlases", samplers, s_Data.MaxTextureSlots);

		s_Data.QuadShader = Shader::Create("assets/shaders/QuadShader.glsl");
		s_Data.CircleShader = Shader::Create("assets/shaders/CircleShader.glsl");
		s_Data.LineShader = Shader::Create("assets/shaders/LineShader.glsl");

		// Set first texture slot to 0
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);

		s_Data.DefaultFont = Font::Create("assets/Fonts/openSans/OpenSans-Regular.ttf");
	}

	void Renderer2D::Shutdown()
	{
		TD_PROFILE_FUNCTION();

		delete[] s_Data.QuadVertexBufferBase;
		delete[] s_Data.TextVertexBufferBase;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		TD_PROFILE_FUNCTION();

		s_Data.QuadShader->Bind();
		s_Data.QuadShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		StartBatch();
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		TD_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		TD_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		TD_PROFILE_FUNCTION();

		Flush();
	}

	void Renderer2D::StartBatch()
	{
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		std::fill(s_Data.QuadVertexBufferBase, s_Data.QuadVertexBufferBase + s_Data.MaxVertices, QuadVertex{});

		s_Data.CircleIndexCount = 0;
		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;
		std::fill(s_Data.CircleVertexBufferBase, s_Data.CircleVertexBufferBase + s_Data.MaxVertices, CircleVertex{});

		s_Data.LineVertexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;
		std::fill(s_Data.LineVertexBufferBase, s_Data.LineVertexBufferBase + s_Data.MaxVertices, LineVertex{});

		s_Data.TextIndexCount = 0;
		s_Data.TextVertexBufferPtr = s_Data.TextVertexBufferBase;
		std::fill(s_Data.TextVertexBufferBase, s_Data.TextVertexBufferBase + s_Data.MaxVertices, TextVertex{});

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::Flush()
	{
		if (s_Data.QuadIndexCount)
		{
			uint32_t fullDataSize = s_Data.MaxVertices * sizeof(QuadVertex);
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, fullDataSize);

			// Bind textures
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
				s_Data.TextureSlots[i]->Bind(i);

			s_Data.QuadShader->Bind();
			RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
			s_Data.Stats.DrawCalls++;

		}
		if (s_Data.CircleIndexCount)
		{
			uint32_t fullDataSize = s_Data.MaxVertices * sizeof(CircleVertex);
			s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase, fullDataSize);

			s_Data.CircleShader->Bind();

			RenderCommand::DrawIndexed(s_Data.CircleVertexArray, s_Data.CircleIndexCount);
			s_Data.Stats.DrawCalls++;

		}

		if (s_Data.LineVertexCount)
		{
			uint32_t fullDataSize = s_Data.MaxVertices * sizeof(LineVertex);
			s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, fullDataSize);
			s_Data.LineShader->Bind();
			RenderCommand::SetLineWidth(s_Data.LineWidth);
			RenderCommand::DrawLines(s_Data.LineVertexArray, s_Data.LineVertexCount);
			s_Data.Stats.DrawCalls++;
		}

		if (s_Data.TextIndexCount)
		{
			uint32_t fullDataSize = s_Data.MaxVertices * sizeof(TextVertex);
			s_Data.TextVertexBuffer->SetData(s_Data.TextVertexBufferBase, fullDataSize);

			// Bind all textures used in the batch (including font atlases)
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
				s_Data.TextureSlots[i]->Bind(i);

			s_Data.TextShader->Bind();
			RenderCommand::DrawIndexed(s_Data.TextVertexArray, s_Data.TextIndexCount);
			s_Data.Stats.DrawCalls++;
		}
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		TD_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		TD_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		TD_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float tilingFactor = 1.0f;

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec2* texCoords, float tilingFactor, const glm::vec4& tintColor, int entityID)
	{
		TD_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = tintColor;
			s_Data.QuadVertexBufferPtr->TexCoord = texCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, int entityID)
	{
		TD_PROFILE_FUNCTION();

		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		DrawQuad(transform, texture, textureCoords, tilingFactor, tintColor, entityID);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		TD_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		TD_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID)
	{
		s_Data.LineVertexBufferPtr->Position = p0;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr->EntityID = entityID;
		s_Data.LineVertexBufferPtr++;
		s_Data.LineVertexBufferPtr->Position = p1;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr->EntityID = entityID;
		s_Data.LineVertexBufferPtr++;
		s_Data.LineVertexCount += 2;
	}
	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
	{
		glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		DrawLine(p0, p1, color);
		DrawLine(p1, p2, color);
		DrawLine(p2, p3, color);
		DrawLine(p3, p0, color);
	}
	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * s_Data.QuadVertexPositions[i];
		DrawLine(lineVertices[0], lineVertices[1], color);
		DrawLine(lineVertices[1], lineVertices[2], color);
		DrawLine(lineVertices[2], lineVertices[3], color);
		DrawLine(lineVertices[3], lineVertices[0], color);
	}

	void Renderer2D::DrawString(const glm::mat4& transform, const TextComponent& component, int entityID)
	{
		TD_PROFILE_FUNCTION();

		Ref<Font> font = component.FontAsset ? component.FontAsset : s_Data.DefaultFont;
		if (!font || component.TextString.empty())
			return;

		auto atlas = font->GetAtlas();
		auto texture = font->GetAtlasTexture();

		// Find texture index in batch
		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		// Atlas accessors
		auto& atlasBitmap = atlas->GetBitmap();
		Trex::FontMetrics metrics = atlas->GetFont()->GetMetrics();

		float atlasW = (float)atlasBitmap.Width();
		float atlasH = (float)atlasBitmap.Height();

		float scale = 1.0f;
		if (metrics.height > 0)
			scale = component.Size / (float)metrics.height;

		Trex::TextShaper shaper(*atlas);
		Trex::ShapedGlyphs shapedGlyphs = shaper.ShapeAscii(component.TextString);

		        float penX = 0.0f;
				float penY = 0.0f;
		
				for (const auto& shapedGlyph : shapedGlyphs)
				{
					if (shapedGlyph.info.codepoint == '\n')
					{
						penX = 0.0f;
						penY -= (metrics.height * scale) + component.LineSpacing;
						continue;
					}
		
					const Trex::Glyph& glyph = shapedGlyph.info;
					float gW = (float)glyph.width;
					float gH = (float)glyph.height;
		
					// Direct vertex calculation to fix baseline alignment
					float x0 = penX + shapedGlyph.xOffset * scale;
					float y0 = penY + shapedGlyph.yOffset * scale;
					float x1 = x0 + gW * scale;
					float y1 = y0 - gH * scale;
		
					glm::vec3 p0 = { x0, y1, 0.0f }; // Bottom-Left
					glm::vec3 p1 = { x1, y1, 0.0f }; // Bottom-Right
					glm::vec3 p2 = { x1, y0, 0.0f }; // Top-Right
					glm::vec3 p3 = { x0, y0, 0.0f }; // Top-Left
		
					// Texture coordinates from atlas (origin top-left)
					float u0 = glyph.x / atlasW;
					float v0 = glyph.y / atlasH;
					float u1 = (glyph.x + glyph.width) / atlasW;
					float v1 = (glyph.y + glyph.height) / atlasH;
		
					if (s_Data.TextIndexCount >= Renderer2DData::MaxIndices)
						NextBatch();
		
					// Submit 4 vertices with Y-Flipped texture coordinates to fix inversion
					s_Data.TextVertexBufferPtr->Position = transform * glm::vec4(p0.x, p0.y, p0.z + 0.001f, 1.0f);
					s_Data.TextVertexBufferPtr->TexCoord = { u0, v0 };
					s_Data.TextVertexBufferPtr->Color = component.Color;
					s_Data.TextVertexBufferPtr->TexIndex = textureIndex;
					s_Data.TextVertexBufferPtr->EntityID = entityID;
					s_Data.TextVertexBufferPtr++;
		
					s_Data.TextVertexBufferPtr->Position = transform * glm::vec4(p1.x, p1.y, p1.z + 0.001f, 1.0f);
					s_Data.TextVertexBufferPtr->TexCoord = { u1, v0 };
					s_Data.TextVertexBufferPtr->Color = component.Color;	
					s_Data.TextVertexBufferPtr->TexIndex = textureIndex;
					s_Data.TextVertexBufferPtr->EntityID = entityID;
					s_Data.TextVertexBufferPtr++;
		
					s_Data.TextVertexBufferPtr->Position = transform * glm::vec4(p2.x, p2.y, p2.z + 0.001f, 1.0f);
					s_Data.TextVertexBufferPtr->TexCoord = { u1, v1 };
					s_Data.TextVertexBufferPtr->Color = component.Color;
					s_Data.TextVertexBufferPtr->TexIndex = textureIndex;
					s_Data.TextVertexBufferPtr->EntityID = entityID;
					s_Data.TextVertexBufferPtr++;
		
					s_Data.TextVertexBufferPtr->Position = transform * glm::vec4(p3.x, p3.y, p3.z + 0.001f, 1.0f);
					s_Data.TextVertexBufferPtr->TexCoord = { u0, v1 };
					s_Data.TextVertexBufferPtr->Color = component.Color;
					s_Data.TextVertexBufferPtr->TexIndex = textureIndex;
					s_Data.TextVertexBufferPtr->EntityID = entityID;
					s_Data.TextVertexBufferPtr++;
		
					s_Data.TextIndexCount += 6;
					s_Data.Stats.QuadCount++;
		
					penX += (shapedGlyph.xAdvance * scale) + component.Kerning;
				}
			}

	void Renderer2D::DrawSprite(const glm::mat4& transform, glm::vec4& src, int entityID)
	{
		DrawQuad(transform, src, entityID);
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness /*= 1.0f*/, float fade /*= 0.005f*/, int entityID /*= -1*/)
	{
		TD_PROFILE_FUNCTION();
		// TODO: implement for circles
		 if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		 	NextBatch();
		for (size_t i = 0; i < 4; i++)
		{
			s_Data.CircleVertexBufferPtr->WorldPosition = transform * s_Data.QuadVertexPositions[i];
			s_Data.CircleVertexBufferPtr->LocalPosition = s_Data.QuadVertexPositions[i] * 2.0f;
			s_Data.CircleVertexBufferPtr->Color = color;
			s_Data.CircleVertexBufferPtr->Thickness = thickness;
			s_Data.CircleVertexBufferPtr->Fade = fade;
			s_Data.CircleVertexBufferPtr->EntityID = entityID;
			s_Data.CircleVertexBufferPtr++;
		}
		s_Data.CircleIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}


	float Renderer2D::GetLineWidth()
	{
		return s_Data.LineWidth;
	}
	void Renderer2D::SetLineWidth(float width)
	{
		s_Data.LineWidth = width;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}

}