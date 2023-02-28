#include "Event.hpp"

WindowEvent::WindowEvent(const class Window* const window) noexcept
	:
	m_Window(window)
{
	BIO_ASSERT(m_Window);
}

const Window* WindowEvent::GetWindow() const
{
	return m_Window;
}

WindowClosedEvent::WindowClosedEvent(const class Window* const window) noexcept
	:
	WindowEvent(window)
{}

WindowResizedEvent::WindowResizedEvent(const class Window* const window, const uint32_t width, const uint32_t height) noexcept
	:
	WindowEvent(window),
	m_Width(width),
	m_Height(height)
{}

std::pair<uint32_t, uint32_t> WindowResizedEvent::GetResize() const noexcept
{
	return std::pair<uint32_t, uint32_t>{ m_Width, m_Height };
}

MouseScrolledEvent::MouseScrolledEvent(const int16_t xDelta, const int16_t yDelta) noexcept
	:
	Event(),
	m_Delta{ xDelta, yDelta }
{}

std::pair<int16_t, int16_t> MouseScrolledEvent::GetScrollDelta() const noexcept
{
	return std::pair<int16_t, int16_t>{ m_Delta.x, m_Delta.y };
}

MouseCursorMovedEvent::MouseCursorMovedEvent(const float xPosition, const float yPosition) noexcept
	:
	Event(),
	m_XPosition(xPosition),
	m_YPosition(yPosition)
{}

std::pair<float, float> MouseCursorMovedEvent::GetPosition() const noexcept
{
	return std::pair<float, float>{ m_XPosition, m_YPosition };
}