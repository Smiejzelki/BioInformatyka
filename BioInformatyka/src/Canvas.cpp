#include "Canvas.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include "Camera.hpp"

constexpr std::string_view linePipelineShaderVertexShaderSource
{ 
"#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
"}\0" 
};

constexpr std::string_view linePipelineShaderFragmentShaderSource
{
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);\n"
"}\0" 
};

struct LineVertex { float x1{ 0.0f }; float y1{ 0.0f }; float x2{ 0.0f }; float y2{ 0.0f }; };
struct CharacterVertex { float Vertices[24U]{ 0.0f }; };

struct CanvasInternalState
{
	std::unique_ptr<Renderer>		OffscreenRenderer{ nullptr };
	std::unique_ptr<Framebuffer>	OffscreenFramebuffer{ nullptr };

	struct
	{
		[[nodiscard]] constexpr size_t MaxLineCount() const noexcept
		{
			size_t g_MaxLineCount{ 16'000'000U / sizeof(LineVertex) }; // 16KB (decimal)
			return g_MaxLineCount;
		}

		[[nodiscard]] constexpr size_t InitialSize() const noexcept
		{
			return sizeof(LineVertex) * MaxLineCount();
		}
		
		[[nodiscard]] constexpr std::initializer_list<VertexBuffer::EShaderDataType> PipelineLayout() const noexcept
		{
			return std::initializer_list<VertexBuffer::EShaderDataType>
			{
				VertexBuffer::EShaderDataType::Float2
			};
		}

		std::unique_ptr<Shader>				Shader{ nullptr };
		std::unique_ptr<VertexBuffer>		VertexBuffer{ nullptr };
		std::unique_ptr<IndexBuffer>		Indexbuffer{ nullptr };
		std::unique_ptr<VertexArrayObject>	VertexArray{ nullptr };
		std::unique_ptr<float[]>			BatchBuffer{ nullptr };
		float*								BatchBufferBase{ nullptr };
		size_t								LineCount{ 0U };

		void Reset()
		{
			LineCount = 0U;
		}
	} LinePipeline;

	struct
	{
		std::unordered_map<char, std::vector<CharacterVertex>> BatchBufferMap;

		void Reset()
		{
			BatchBufferMap.clear();
		}
	} TextPipeline;
};

Canvas::Canvas(
	const uint32_t width, 
	const uint32_t height, 
	const float clearColorR, 
	const float clearColorG, 
	const float clearColorB)
	:
	m_Width(width),
	m_Height(height),
	m_InternalState(new CanvasInternalState),
	ClearColor{ clearColorR, clearColorG, clearColorB }
{
	BIO_ASSERT(width > 0U && height > 0U);
	m_InternalState->OffscreenRenderer = std::make_unique<Renderer>(nullptr);
	m_InternalState->OffscreenFramebuffer = std::make_unique<Framebuffer>(m_Width, m_Height);

	/* Line pipeline */
	{
		auto& linePipeline{ m_InternalState->LinePipeline };
		std::vector<uint32_t> indices;
		indices.resize(linePipeline.MaxLineCount());
		
		for (size_t i{ 0U }; i < linePipeline.MaxLineCount(); i += 2U)
		{
			indices[i + 0U] = static_cast<uint32_t>(i + 0U);
			indices[i + 1U] = static_cast<uint32_t>(i + 1U);
		}

		linePipeline.Shader = std::make_unique<Shader>
		(
			linePipelineShaderVertexShaderSource, 
			linePipelineShaderFragmentShaderSource
		);

		linePipeline.VertexBuffer = std::make_unique<VertexBuffer>(linePipeline.PipelineLayout(), linePipeline.InitialSize());
		linePipeline.Indexbuffer = std::make_unique<IndexBuffer>(indices);
		linePipeline.VertexArray = std::make_unique<VertexArrayObject>(linePipeline.VertexBuffer);

		linePipeline.BatchBuffer = std::make_unique<float[]>(sizeof(LineVertex) * linePipeline.MaxLineCount());
		linePipeline.BatchBufferBase = &linePipeline.BatchBuffer[0U];
	}

	/* Text Pipeline */
	{
		auto& textPipeline{ m_InternalState->TextPipeline };
		textPipeline.BatchBufferMap.rehash(131U); // Bucket count should be 131?
	}
}

Canvas::~Canvas() noexcept
{
	BIO_LIKELY
	if(m_InternalState)
		delete m_InternalState;
}

bool Canvas::SetSize(const uint32_t width, const uint32_t height)
{
	if (m_Width != width || m_Height != height)
	{
		m_Width = width;
		m_Height = height;

		if (m_Width < 1U || m_Height < 1U)
			return false;
		
		m_InternalState->OffscreenFramebuffer->Invalidate(width, height);
		m_InternalState->OffscreenRenderer->SetViewportSize(width, height);
		
		return true;
	}

	return false;
}

void Canvas::BeginDraw(const std::unique_ptr<Camera>& camera)
{
	/* Reset line pipeline */
	m_InternalState->LinePipeline.Reset();
	/* Reset text pipeline */
	m_InternalState->TextPipeline.Reset();

	
	const Matrix4x4 viewProjection{ camera->GetViewProjection() };
	m_InternalState->LinePipeline.Shader->SetUniformMatrix4x4("projection", viewProjection.raw);

	const auto& linePipelineFramebuffer{ m_InternalState->OffscreenFramebuffer };
	m_InternalState->OffscreenRenderer->SetTextProjection(camera->GetViewProjection());
	m_InternalState->OffscreenRenderer->BeginFrame(
		ClearColor.R, 
		ClearColor.G, 
		ClearColor.B, 
		false, /* Do not clear depth buffer */
		linePipelineFramebuffer);
}

void Canvas::EndDraw()
{
	const auto& linePipeline{ m_InternalState->LinePipeline };

	const size_t lineCount{ linePipeline.LineCount < linePipeline.MaxLineCount() ? linePipeline.LineCount : linePipeline.MaxLineCount() };
	linePipeline.VertexBuffer->SetData(linePipeline.BatchBufferBase, lineCount * sizeof(LineVertex));
	
	m_InternalState->OffscreenRenderer->DrawGeometryIndexed
	(
		linePipeline.Shader,
		linePipeline.VertexArray,
		linePipeline.Indexbuffer,
		Renderer::ETopology::Lines,
		linePipeline.LineCount,
		2.0f
	);
	
	for (auto& [character, vertices] : m_InternalState->TextPipeline.BatchBufferMap)
		m_InternalState->OffscreenRenderer->DrawCharacters(character, vertices.size(), reinterpret_cast<float*>(vertices.data()));

	m_InternalState->OffscreenRenderer->EndFrame();
}

#define SIN(x) sinf(x * 3.141592653589f / 180.0f)
#define COS(x) cosf(x * 3.141592653589f / 180.0f)

// To rotate an object
void rotate(float* a, float x_pivot, float y_pivot,
	float angle)
{
	float x_shifted = a[0] - x_pivot;
	float y_shifted = a[1] - y_pivot;

	a[0] = x_pivot
		+ (x_shifted * COS(angle)
			- y_shifted * SIN(angle));
	a[1] = y_pivot
			+ (x_shifted * SIN(angle)
				+ y_shifted * COS(angle));
}

void Canvas::DrawLine(const Vector2Param beginPoint, const Vector2Param endPoint, const float rotation)
{
	const Vector2 pivot = { endPoint.x - (endPoint.x - beginPoint.x) * 0.5f, endPoint.y - (endPoint.y - beginPoint.y) * 0.5f };
	DrawLine(beginPoint, endPoint, rotation, pivot);
}

void Canvas::DrawLine(const Vector2Param beginPoint, const Vector2Param endPoint, const float rotation, Vector2 pivot)
{
	BIO_UNLIKELY
	if (m_InternalState->LinePipeline.LineCount == m_InternalState->LinePipeline.MaxLineCount())
		return;

	auto& linePipelineBatch{ m_InternalState->LinePipeline.BatchBuffer };
	auto& lineCount{ m_InternalState->LinePipeline.LineCount };

	linePipelineBatch[0U + 4U * lineCount] = beginPoint.x;
	linePipelineBatch[1U + 4U * lineCount] = beginPoint.y;
	linePipelineBatch[2U + 4U * lineCount] = endPoint.x;
	linePipelineBatch[3U + 4U * lineCount] = endPoint.y;

	if (rotation > 0.0f)
	{
		rotate(&linePipelineBatch[0U + 4U * lineCount], pivot.x, pivot.y, rotation);
		rotate(&linePipelineBatch[2U + 4U * lineCount], pivot.x, pivot.y, rotation);
	}

	++lineCount;
}

void Canvas::DrawText(const std::string_view text, const Vector2Param position, const float scale)
{
	float x{ position.x * 1000.0f };
	float y{ position.y * 1000.0f };
	for (const char character : text)
	{
		/* TODO: make it adjustable? */
		const CharacterGlyph& characterGlyph{ m_InternalState->OffscreenRenderer->GetCharacterGlyph(character) };

		const float xpos = x + characterGlyph.BearingX * scale;
		const float ypos = y - ((float)characterGlyph.Height - (float)characterGlyph.BearingY) * scale;

		const float w = characterGlyph.Width * scale;
		const float h = characterGlyph.Height * scale;

		const float vertices[6][4]
		{
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};

		x += (characterGlyph.Advance >> 6) * scale;

		{
			auto& glyphBatch{ m_InternalState->TextPipeline.BatchBufferMap[character].emplace_back() };
			memcpy(glyphBatch.Vertices, vertices, sizeof(float) * 24);
		}
	}
}

ImageRendererID Canvas::GetImage() const noexcept
{
	const auto& linePipelineFramebuffer{ m_InternalState->OffscreenFramebuffer };
	return static_cast<ImageRendererID>(linePipelineFramebuffer->GetColorAttachmentRendererID());
}