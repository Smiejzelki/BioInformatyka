#pragma once
#include "Core.hpp"
#include "Math.hpp"

class Camera;
using ImageRendererID = uint64_t;
using Vector2Param = Vector2;

class Canvas
{
private:
	NON_COPYABLE(Canvas)
public:
	explicit Canvas(const uint32_t width, const uint32_t height, const float clearColorR = 1.0f, const float clearColorG = 1.0f, const float clearColorB = 1.0f);
	~Canvas() noexcept;

	bool SetSize(const uint32_t width, const uint32_t height);
	void BeginDraw(const std::unique_ptr<Camera>& camera);
	void EndDraw();

	void DrawLine(const Vector2Param beginPoint, const Vector2Param endPoint, const float rotation);
	void DrawLine(const Vector2Param beginPoint, const Vector2Param endPoint, const float rotation, const Vector2 pivot);
	void DrawText(const std::string_view text, const Vector2Param position, const float scale = 1.0f);
	 
	ImageRendererID GetImage() const noexcept;
private:
	uint32_t m_Width, m_Height;
	struct
	{
		float R, G, B;
	} ClearColor;
	struct CanvasInternalState* m_InternalState;
};