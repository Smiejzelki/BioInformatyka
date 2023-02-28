#pragma once
#include "Core.hpp"
#include "Math.hpp"

#define BEFRIEND_RENDERERS		\
friend class Renderer;			\
friend class GUIRenderer;		\
friend class Canvas


class Window;

class VertexBuffer
{
private:
	NON_COPYABLE(VertexBuffer)
public:
	enum class EShaderDataType
	{
		Float1 = 1U,
		Float2 = 2U,
		Float3 = 3U,
		Float4 = 4U,
	};
public:
	explicit VertexBuffer(const std::initializer_list<EShaderDataType> dataLayout, const size_t reservedSize);
	~VertexBuffer() noexcept;
	
	void SetData(float* data, const size_t size, const size_t offset = 0U);
private:
	uint32_t m_RendererHandle;
	size_t m_ReservedSize;
	std::vector<EShaderDataType> m_ShaderLayout;
private:
	friend class VertexArrayObject;
	BEFRIEND_RENDERERS;
};

class IndexBuffer
{
private:
	NON_COPYABLE(IndexBuffer)
public:
	explicit IndexBuffer(const std::vector<uint32_t>& indices);
	~IndexBuffer() noexcept;
private:
	uint32_t m_RendererHandle;
private:
	BEFRIEND_RENDERERS;
};

class VertexArrayObject
{
private:
	NON_COPYABLE(VertexArrayObject)
public:
	explicit VertexArrayObject(const std::unique_ptr<VertexBuffer>& vertexBuffer);
	~VertexArrayObject() noexcept;
private:
	uint32_t m_RendererHandle;
private:
	BEFRIEND_RENDERERS;
};

class Shader
{
private:
	NON_COPYABLE(Shader)

	enum class EShaderType
	{
		Vertex,
		Fragment,
	};
public:
	explicit Shader(const std::string_view vertexSource, const std::string_view fragmentSource);
	~Shader() noexcept;

	void SetUniformFloat3(const std::string_view uniformName, const float value1, const float value2, const float value3);
	void SetUniformMatrix4x4(const std::string_view uniformName, const float* matrix);
private:
	void TryCompileShader(const char* source, const EShaderType shaderType, uint32_t& outShaderDescriptor);
private:
	uint32_t m_RendererHandle;
	std::unordered_map<std::string_view, uint32_t> m_UniformLocationCache;
private:
	BEFRIEND_RENDERERS;
};

class Framebuffer
{
private:
	NON_COPYABLE(Framebuffer)
public:
	explicit Framebuffer(const uint32_t width, const uint32_t height);
	~Framebuffer() noexcept;

	void Invalidate(const uint32_t width, const uint32_t height);
	uint32_t GetColorAttachmentRendererID() const;
	std::pair<uint32_t, uint32_t> GetSize() const;
private:
	uint32_t m_RendererHandle;
	uint32_t m_ColorAttachmentRendererHandle;
	uint32_t m_Width, m_Height;
private:
	BEFRIEND_RENDERERS;
};

struct CharacterGlyph
{
	uint32_t Width, Height;
	uint32_t BearingX, BearingY;
	uint32_t Advance;
};

class Renderer
{
private:
	NON_COPYABLE(Renderer)
public:
	enum class ETopology
	{
		Lines,
		Triangles,
	};
public:
	explicit Renderer(const std::unique_ptr<Window>& windowContext = nullptr); 
	~Renderer() noexcept;

	void BeginFrame(const float clearColorRed, const float clearColorGreen, const float clearColorBlue, const bool clearDepthBuffer = true, const std::unique_ptr<Framebuffer>& framebuffer = nullptr);
	void EndFrame();

	void DrawGeometryIndexed(const std::unique_ptr<Shader>& shader, const std::unique_ptr<VertexArrayObject>& vertexArrayObject, const std::unique_ptr<IndexBuffer>& indexBuffer, const ETopology topology, const size_t count, const float lineWidth = 1.0f);
	void DrawCharacters(const char character, const size_t count, float* vertices);
	
	void SetViewportSize(const uint32_t width, const uint32_t height, const int32_t xOffset = 0, const int32_t yOffset = 0);
	void SetTextProjection(const Matrix4x4& projection);
	
	const CharacterGlyph& GetCharacterGlyph(const char character) const;
private:
	struct RendererInternalState* m_InternalState;
};