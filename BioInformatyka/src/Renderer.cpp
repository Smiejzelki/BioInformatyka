#include "Renderer.hpp"
#include "Window.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <ft2build.h>
#include FT_FREETYPE_H

static std::unordered_map<char, CharacterGlyph> s_CharacterGlyphs;
static std::unordered_map<char, uint32_t> s_CharacterGlyphTextureIDs;

struct RendererInternalState
{
	struct
	{
		constexpr std::vector<float> InitialVertices() const noexcept
		{
			return std::vector<float>{};
		}

		constexpr std::initializer_list<VertexBuffer::EShaderDataType> PipelineLayout() const noexcept
		{
			return std::initializer_list<VertexBuffer::EShaderDataType>
			{
				VertexBuffer::EShaderDataType::Float4
			};
		}

		constexpr size_t MaxGlyphCount() const noexcept
		{
			return 15'000U;
		}

		std::unique_ptr<Shader>			    Shader{ nullptr };
		std::unique_ptr<VertexBuffer>		VertexBuffer{ nullptr };
		std::unique_ptr<VertexArrayObject>	VertexArray{ nullptr };

		constexpr static std::string_view VertexShaderSource
		{
		"#version 330 core\n"
		"layout(location = 0) in vec4 vertex;\n"
		"out vec2 TexCoords;\n"

		"uniform mat4 projection;\n"

		"void main()\n"
		"{\n"
		"gl_Position = projection * vec4(vertex.x * 0.001, vertex.y * 0.001, 0.0, 1.0);\n"
		"TexCoords = vertex.zw;\n"
		"}\0"
		};

		constexpr static std::string_view FragmentShaderSource
		{
		"#version 330 core\n"
		"in vec2 TexCoords;\n"
		"out vec4 color;\n"

		"uniform sampler2D text;\n"

		"void main()\n"
		"{\n"
		"vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
		"color = vec4(0.0, 0.0, 0.0, 1.0) * sampled;\n"
		"}\0"
		};
	} TextPipeline;
};

constinit static std::once_flag s_IsGLADInitialized;
Renderer::Renderer(
	const std::unique_ptr<Window>& windowContext)
	:
	m_InternalState([&]()
	{
		/* Create an off-screen renderer if desired */
		if (windowContext)
		{
			BIO_ASSERT(windowContext->GetHandle());
			glfwMakeContextCurrent(windowContext->GetHandle());
			glfwSwapInterval(windowContext->m_VSync);
		}

		try
		{
			std::call_once(s_IsGLADInitialized, [&]()
			{
				if (!windowContext)
					THROW_EXCEPTION("Must provide a valid window to initialize renderer");
				
				if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
					THROW_EXCEPTION("Failed to setup glad loader");

				glEnable(GL_MULTISAMPLE);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

				FT_Library ft;
				if (FT_Init_FreeType(&ft))
					THROW_EXCEPTION("Failed to initialize free type library");

				FT_Face face;
				if (FT_New_Face(ft, "res/fonts/OpenSans-Regular.ttf", 0, &face))
					THROW_EXCEPTION("Failed to load font (freetype)");

				FT_Set_Pixel_Sizes(face, 0, 48);
				for (uint8_t c{ 0U }; c < 128U; ++c)
				{
					if (FT_Load_Char(face, c, FT_LOAD_RENDER))
						continue;

					uint32_t glyphTextureID{ 0U };
					glGenTextures(
						1,
						&glyphTextureID);

					glBindTexture(
						GL_TEXTURE_2D,
						glyphTextureID);

					glTexImage2D(
						GL_TEXTURE_2D,
						0,
						GL_RED,
						face->glyph->bitmap.width,
						face->glyph->bitmap.rows,
						0,
						GL_RED,
						GL_UNSIGNED_BYTE,
						face->glyph->bitmap.buffer);

					glTexParameteri(
						GL_TEXTURE_2D,
						GL_TEXTURE_WRAP_S,
						GL_CLAMP_TO_EDGE);

					glTexParameteri(
						GL_TEXTURE_2D,
						GL_TEXTURE_WRAP_T,
						GL_CLAMP_TO_EDGE);

					glTexParameteri(
						GL_TEXTURE_2D,
						GL_TEXTURE_MIN_FILTER,
						GL_LINEAR);

					glTexParameteri(
						GL_TEXTURE_2D,
						GL_TEXTURE_MAG_FILTER,
						GL_LINEAR);

					s_CharacterGlyphTextureIDs[static_cast<char>(c)] =
					{
						glyphTextureID
					};

					s_CharacterGlyphs[static_cast<char>(c)] =
					{
						static_cast<uint32_t>(face->glyph->bitmap.width),
						static_cast<uint32_t>(face->glyph->bitmap.rows),
						static_cast<uint32_t>(face->glyph->bitmap_left),
						static_cast<uint32_t>(face->glyph->bitmap_top),
						static_cast<uint32_t>(face->glyph->advance.x)
					};
				}

				FT_Done_Face(face);
				FT_Done_FreeType(ft);
			});
		}
		catch (...)
		{
			HandleExceptions();
		}

		RendererInternalState* internalState{ new RendererInternalState };
		auto& textPipeline{ internalState->TextPipeline };
		textPipeline.Shader = std::make_unique<Shader>
		(
			textPipeline.VertexShaderSource,
			textPipeline.FragmentShaderSource
		);

		[[maybe_unused]] constexpr float GlyphTemplate[6U][4U]{ 0.0f };
		textPipeline.VertexBuffer = std::make_unique<VertexBuffer>(textPipeline.PipelineLayout(), sizeof(GlyphTemplate) * textPipeline.MaxGlyphCount());
		textPipeline.VertexArray = std::make_unique<VertexArrayObject>(textPipeline.VertexBuffer);
		
		return internalState;
	}())
{}

Renderer::~Renderer() noexcept
{
	BIO_LIKELY
	if (m_InternalState)
		delete m_InternalState;
}

void Renderer::BeginFrame(
	const float clearColorRed,
	const float clearColorGreen,
	const float clearColorBlue,
	const bool clearDepthBuffer,
	const std::unique_ptr<Framebuffer>& framebuffer)
{
	/* Use default framebuffer if not provided */
	glBindFramebuffer(
		GL_FRAMEBUFFER, 
		framebuffer ? framebuffer->m_RendererHandle : 0);

	glClearColor
	(
		clearColorRed,
		clearColorGreen,
		clearColorBlue,
		1.0f
	);

	glClear
	(
		GL_COLOR_BUFFER_BIT |
		(clearDepthBuffer ? GL_DEPTH_BUFFER_BIT : 0)
	);
}

void Renderer::EndFrame()
{
	/* Switch back to default framebuffer */
	glBindFramebuffer(
		GL_FRAMEBUFFER,
		0);
}

void Renderer::DrawGeometryIndexed(
	const std::unique_ptr<Shader>& shader, 
	const std::unique_ptr<VertexArrayObject>& vertexArrayObject,
	const std::unique_ptr<IndexBuffer>& indexBuffer, 
	const ETopology topology, 
	const size_t count,
	const float lineWidth)
{
	BIO_ASSERT(shader && vertexArrayObject && indexBuffer);
	glUseProgram(shader->m_RendererHandle);
	glBindVertexArray(vertexArrayObject->m_RendererHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->m_RendererHandle);

	switch (topology)
	{
		case ETopology::Triangles:
		{
			glDrawElements(
				GL_TRIANGLES,
				static_cast<GLsizei>(count * 3U),
				GL_UNSIGNED_INT,
				nullptr);
		} break;

		case ETopology::Lines:
		{
			glLineWidth(
				lineWidth);

			glDrawElements(
				GL_LINES,
				static_cast<GLsizei>(count * 2U),
				GL_UNSIGNED_INT,
				nullptr);

			glLineWidth(
				1.0f);
		} break;

		default: BIO_ASSERT(false); break;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0U);
	glBindVertexArray(0U);
	glUseProgram(0U);
}

void Renderer::DrawCharacters(const char character, const size_t count, float* vertices)
{
	auto& textPipeline{ m_InternalState->TextPipeline };

	const size_t glyphCount{ count < textPipeline.MaxGlyphCount() ? count : textPipeline.MaxGlyphCount() };
	textPipeline.VertexBuffer->SetData(vertices, sizeof(float) * 24U * glyphCount);
	
	glUseProgram(
		textPipeline.Shader->m_RendererHandle);

	glBindVertexArray(
		textPipeline.VertexArray->m_RendererHandle);

	glActiveTexture(
		GL_TEXTURE0);

	glBindTexture(
		GL_TEXTURE_2D, 
		s_CharacterGlyphTextureIDs[character]);

	glDrawArrays(
		GL_TRIANGLES, 
		0, 
		static_cast<GLsizei>(glyphCount) * 6);
}

void Renderer::SetViewportSize(
	const uint32_t width,
	const uint32_t height,
	const int32_t xOffset,
	const int32_t yOffset)
{
	BIO_ASSERT(width > 0U && height > 0U);

	glViewport
	(
		static_cast<GLint>(xOffset),
		static_cast<GLint>(yOffset),
		static_cast<GLsizei>(width),
		static_cast<GLsizei>(height)
	);
}

void Renderer::SetTextProjection(const Matrix4x4& projection)
{
	m_InternalState->TextPipeline.Shader->SetUniformMatrix4x4("projection", projection.raw);
}

const CharacterGlyph& Renderer::GetCharacterGlyph(const char character) const
{
	BIO_ASSERT(s_CharacterGlyphs.find(character) != s_CharacterGlyphs.end());
	return s_CharacterGlyphs[character];
}

VertexBuffer::VertexBuffer(
	const std::initializer_list<EShaderDataType> dataLayout,
	const size_t reservedSize)
	:
	m_RendererHandle(0U),
	m_ReservedSize(reservedSize),
	m_ShaderLayout(dataLayout)
{
	glGenBuffers(1, &m_RendererHandle);
	BIO_ASSERT(m_RendererHandle && m_RendererHandle > 0U);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererHandle);
	glBufferData(GL_ARRAY_BUFFER, reservedSize, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0U);
}

VertexBuffer::~VertexBuffer() noexcept
{
	BIO_LIKELY
	if (m_RendererHandle)
		glDeleteBuffers(
			1U,
			&m_RendererHandle);
}

void VertexBuffer::SetData(float* data, const size_t size, const size_t offset)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererHandle);
	if (m_ReservedSize >= (size + offset))
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
	else
		fprintf(stderr, "Vertex buffer error: Requested offset + size is greater than available size\n");
}

IndexBuffer::IndexBuffer(
	const std::vector<uint32_t>& indices)
	:
	m_RendererHandle(0U)
{
	glGenBuffers(1, &m_RendererHandle);
	BIO_ASSERT(m_RendererHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererHandle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0U);
}

IndexBuffer::~IndexBuffer() noexcept
{
	BIO_LIKELY
	if (m_RendererHandle)
		glDeleteBuffers(
			1U,
			&m_RendererHandle);
}

VertexArrayObject::VertexArrayObject(
	const std::unique_ptr<VertexBuffer>& vertexBuffer)
	:
	m_RendererHandle(0U)
{
	BIO_ASSERT(vertexBuffer && vertexBuffer->m_RendererHandle);
	glGenVertexArrays(1, &m_RendererHandle);
	BIO_ASSERT(m_RendererHandle);
	glBindVertexArray(m_RendererHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->m_RendererHandle);

	using EShaderDataType = VertexBuffer::EShaderDataType;

	GLuint iterator{ 0U };
	size_t offset{ 0U };
	for (const EShaderDataType dataType : vertexBuffer->m_ShaderLayout)
	{
		const GLint dataTypeElementSize
		{
			[](const EShaderDataType type) -> GLint
			{
				switch (type)
				{
					case EShaderDataType::Float1: return 1;
					case EShaderDataType::Float2: return 2;
					case EShaderDataType::Float3: return 3;
					case EShaderDataType::Float4: return 4;

					default: break;
				}

				BIO_ASSERT(false);
				return sizeof(float);
			}(dataType)
		};

		const GLenum dataTypeGLID
		{
			[](const EShaderDataType type) -> GLenum
			{
				switch (type)
				{
					case EShaderDataType::Float1:
					case EShaderDataType::Float2:
					case EShaderDataType::Float3:
					case EShaderDataType::Float4:
						return GL_FLOAT;

					default: break;
				}

				BIO_ASSERT(false);
				return sizeof(float);
			}(dataType)
		};

		const GLuint stride
		{
			[](const EShaderDataType type) -> GLuint
			{
				switch (type)
				{
					case EShaderDataType::Float1: return 1U * sizeof(float);
					case EShaderDataType::Float2: return 2U * sizeof(float);
					case EShaderDataType::Float3: return 3U * sizeof(float);
					case EShaderDataType::Float4: return 4U * sizeof(float);
					default: break;
				}

				BIO_ASSERT(false);
				return sizeof(float);
			}(dataType)
		};

		glVertexAttribPointer(
			iterator,
			dataTypeElementSize,
			dataTypeGLID,
			GL_FALSE,
			stride,
			reinterpret_cast<const void*>(offset));

		glEnableVertexAttribArray(iterator);
		++iterator;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0U);
	glBindVertexArray(0U);
}

VertexArrayObject::~VertexArrayObject() noexcept
{
	BIO_LIKELY
	if (m_RendererHandle)
		glDeleteVertexArrays(
			1, 
			&m_RendererHandle);
}

Shader::Shader(
	const std::string_view vertexSource, 
	const std::string_view fragmentSource)
	:
	m_RendererHandle(0U),
	m_UniformLocationCache{}
{
	BIO_ASSERT(!vertexSource.empty() && !fragmentSource.empty());
	uint32_t vertexShader{ 0U }, fragmentShader{ 0U };

	TryCompileShader(vertexSource.data(), EShaderType::Vertex, vertexShader);
	TryCompileShader(fragmentSource.data(), EShaderType::Fragment, fragmentShader);

	const uint32_t shaderProgram{ glCreateProgram() };
	glAttachShader(
		shaderProgram, 
		vertexShader);
	
	glAttachShader(
		shaderProgram, 
		fragmentShader);

	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	char buffer[1024U]{ '\0' };
	int32_t success{ 0U };
	glGetProgramiv(
		shaderProgram, 
		GL_LINK_STATUS, 
		&success);
	
	if (!success) 
	{
		glGetProgramInfoLog(
			shaderProgram, 
			static_cast<GLsizei>(1024U), 
			nullptr, 
			buffer);

		LOG("Shader linking failed:");
		fprintf(stderr, buffer);
		THROW_EXCEPTION("Shader linking failure");
	}

	glUseProgram(0);
	m_RendererHandle = shaderProgram;
}

Shader::~Shader() noexcept
{
	BIO_LIKELY
	if(m_RendererHandle)
		glDeleteProgram(m_RendererHandle);
}

void Shader::SetUniformFloat3(const std::string_view uniformName, const float value1, const float value2, const float value3)
{
	glUseProgram(m_RendererHandle);
	if (!m_UniformLocationCache.contains(uniformName))
	{
		const int32_t location{ glGetUniformLocation(m_RendererHandle, uniformName.data()) };
		if (location == -1)
		{
			fprintf(stdout, "Failed to find location for uniform %s\n.", uniformName.data());
			return;
		}

		m_UniformLocationCache[uniformName] = location;
	}

	glUniform3f(m_UniformLocationCache[uniformName], value1, value2, value3);
}

void Shader::SetUniformMatrix4x4(const std::string_view uniformName, const float* matrix)
{
	glUseProgram(m_RendererHandle);
	if (!m_UniformLocationCache.contains(uniformName))
	{
		const int32_t location{ glGetUniformLocation(m_RendererHandle, uniformName.data()) };
		if (location == -1)
		{
			fprintf(stdout, "Failed to find location for uniform %s\n.", uniformName.data());
			return;
		}

		m_UniformLocationCache[uniformName] = location;
	}

	glUniformMatrix4fv(m_UniformLocationCache[uniformName], 1, GL_FALSE, reinterpret_cast<const GLfloat*>(matrix));
}

void Shader::TryCompileShader(
	const char* source, 
	const EShaderType shaderType, 
	uint32_t& outShaderDescriptor)
{
	outShaderDescriptor = 0U;
	const GLenum shaderTypeEnum
	{
		[](const EShaderType shaderType)
		{
			switch (shaderType)
			{
				case EShaderType::Vertex:	return static_cast<GLenum>(GL_VERTEX_SHADER);
				case EShaderType::Fragment: return static_cast<GLenum>(GL_FRAGMENT_SHADER);

				default:
					break;
			}

			BIO_ASSERT(false);
			return static_cast<GLenum>(GL_VERTEX_SHADER);
		}(shaderType)
	};

	const uint32_t shaderDescriptor{ glCreateShader(shaderTypeEnum) };
	glShaderSource(
		shaderDescriptor, 
		1, 
		&source, 
		nullptr);

	glCompileShader(shaderDescriptor);
	
	int32_t success{ 0U };
	glGetShaderiv(
		shaderDescriptor,
		GL_COMPILE_STATUS, 
		&success);

	BIO_UNLIKELY
	if (!success)
	{
		constexpr size_t g_BufferSize{ 1024U };
		char buffer[g_BufferSize]{ '\0' };
		glGetShaderInfoLog(
			shaderDescriptor,
			static_cast<GLsizei>(g_BufferSize), 
			nullptr, 
			buffer);

		LOG("Shader compilation failed:");
		fprintf(stderr, buffer);
		fprintf(stderr, "Shader source: \n%s\n", source);

		THROW_EXCEPTION("Shader compilation failure");
	}

	outShaderDescriptor = shaderDescriptor;
}

Framebuffer::Framebuffer(
	const uint32_t width, 
	const uint32_t height)
	:
	m_RendererHandle(0U),
	m_ColorAttachmentRendererHandle(0U),
	m_Width(width),
	m_Height(height)
{
	BIO_ASSERT(width > 0U && height > 0U);
	Invalidate(width, height);
}

Framebuffer::~Framebuffer() noexcept
{
	BIO_LIKELY
	if (m_RendererHandle)
		glDeleteFramebuffers(
			1, 
			&m_RendererHandle);

	BIO_LIKELY
	if (m_ColorAttachmentRendererHandle)
		glDeleteTextures(
			1,
			&m_ColorAttachmentRendererHandle);
}

void Framebuffer::Invalidate(
	const uint32_t width, 
	const uint32_t height)
{
	BIO_ASSERT(width > 0U && height > 0U);
	m_Width = width;
	m_Height = height;

	BIO_LIKELY
	if (m_RendererHandle)
		glDeleteFramebuffers(
			1,
			&m_RendererHandle);

	BIO_LIKELY
	if (m_ColorAttachmentRendererHandle)
		glDeleteTextures(
			1,
			&m_ColorAttachmentRendererHandle);

	glGenFramebuffers(
		1,
		&m_RendererHandle);

	glBindFramebuffer(
		GL_FRAMEBUFFER,
		m_RendererHandle);

	glGenTextures(
		1,
		&m_ColorAttachmentRendererHandle);

	const GLenum textureDestination
	{
		static_cast<GLenum>(GL_TEXTURE_2D) 
	};

	glEnable(textureDestination);
	glBindTexture(
		textureDestination,
		m_ColorAttachmentRendererHandle);

	glTexParameteri(
		textureDestination,
		GL_TEXTURE_WRAP_S,
		GL_REPEAT);

	glTexParameteri(
		textureDestination,
		GL_TEXTURE_WRAP_T,
		GL_REPEAT);

	glTexParameteri(
		textureDestination,
		GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);

	glTexParameteri(
		textureDestination,
		GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);

	glTexImage2D(
		textureDestination,
		0,
		GL_RGBA8,
		static_cast<GLsizei>(width),
		static_cast<GLsizei>(height),
		0,
		GL_BGRA,
		GL_UNSIGNED_BYTE,
		nullptr);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		textureDestination,
		m_ColorAttachmentRendererHandle,
		0);

	BIO_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

uint32_t Framebuffer::GetColorAttachmentRendererID() const
{
	BIO_ASSERT(m_ColorAttachmentRendererHandle);
	return m_ColorAttachmentRendererHandle;
}

std::pair<uint32_t, uint32_t> Framebuffer::GetSize() const
{
	return { m_Width, m_Height };
}