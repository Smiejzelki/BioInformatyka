#pragma once
#include "Core.hpp"

#define EVENT_TYPE(event_) \
	static constexpr EEventType GetEventTypeStatic() noexcept { static_assert(std::is_base_of<Event, event_##Event>::value, "Class is not derived from Event!"); return EEventType::event_; } \
	inline EEventType GetEventType() const noexcept override final { return GetEventTypeStatic(); }

enum class EEventType
{
	/* Window events */
	WindowClosed, WindowResized,
	/* Mouse events */
	MouseScrolled, MouseCursorMoved,
};

class Event
{
public:
	constexpr explicit Event() noexcept = default;
	constexpr virtual ~Event() noexcept = default;

	virtual EEventType GetEventType() const noexcept = 0;
public:
	mutable bool m_IsHandled{ false };
};

template<typename T>
const T& EventCast(const Event& event)
{
	return static_cast<const T&>(event);
}

class WindowEvent : public Event
{
public:
	explicit WindowEvent(const class Window* const window) noexcept;
	virtual ~WindowEvent() noexcept = default;
protected:
	virtual const class Window* GetWindow() const final;
protected:
	const class Window* const m_Window;
};

class WindowClosedEvent final : public WindowEvent
{
public:
	explicit WindowClosedEvent(const class Window* const window) noexcept;
	virtual ~WindowClosedEvent() noexcept = default;

	EVENT_TYPE(WindowClosed)
private:
};

class WindowResizedEvent final : public WindowEvent
{
public:
	explicit WindowResizedEvent(const class Window* const window, const uint32_t width, const uint32_t height) noexcept;
	virtual ~WindowResizedEvent() noexcept = default;

	std::pair<uint32_t, uint32_t> GetResize() const noexcept;

	EVENT_TYPE(WindowResized)
private:
	const uint32_t m_Width;
	const uint32_t m_Height;
};

class MouseScrolledEvent final : public Event 
{
public:
	explicit MouseScrolledEvent(const int16_t xDelta, const int16_t yDelta) noexcept;
	virtual ~MouseScrolledEvent() noexcept = default;

	std::pair<int16_t, int16_t> GetScrollDelta() const noexcept;

	EVENT_TYPE(MouseScrolled)
private:
	struct
	{
		const int16_t x;
		const int16_t y;
	} const m_Delta;
};

class MouseCursorMovedEvent final : public Event
{
public:
	explicit MouseCursorMovedEvent(const float xPosition, const float yPosition) noexcept;
	virtual ~MouseCursorMovedEvent() noexcept = default;

	std::pair<float, float> GetPosition() const noexcept;

	EVENT_TYPE(MouseCursorMoved)
private:
	const float m_XPosition;
	const float m_YPosition;
};

using EventCallbackFunction = std::function<void(const Event& event)>;