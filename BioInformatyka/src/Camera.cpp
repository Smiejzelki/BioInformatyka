#include "Camera.hpp"
#include "Event.hpp"

Camera::Camera(const float aspectRatio, const float scale) noexcept
	:
	m_AspectRatio(aspectRatio),
	m_Scale(scale),
	m_CachedMousePosition{ 0.0f, 0.0f },
	m_Position{},
	m_ViewMatrix{ 1.0f },
	m_ProjectionMatrix{ 1.0f }
{
	RecalculateProjection();
}

void Camera::OnEvent(const Event& event, const bool shouldUpdate) noexcept
{
	switch (event.GetEventType())
	{
		case EEventType::MouseScrolled:
		{
			if (!shouldUpdate)
				break;

			const MouseScrolledEvent& mouseScrolledEvent{ EventCast<MouseScrolledEvent>(event) };
			const auto [scrollDeltaX, scrollDeltaY] { mouseScrolledEvent.GetScrollDelta()};

			constexpr float g_ZoomSpeed{ 0.05f };
			m_Scale = m_Scale - (scrollDeltaY * g_ZoomSpeed * (m_Scale * 0.5f));

			m_Scale = m_Scale > 0.5f ? m_Scale : 0.5f;
			m_Scale = m_Scale < 150.0f ? m_Scale : 150.0f;
		} break;

		case EEventType::MouseCursorMoved:
		{
			const MouseCursorMovedEvent& mouseScrolledEvent{ EventCast<MouseCursorMovedEvent>(event) };
			const auto [xPosition, yPosition] { mouseScrolledEvent.GetPosition()};

			if (shouldUpdate)
			{
				constexpr float g_MoveSpeed{ 0.005f };
				const Vector2 delta
				{
					m_CachedMousePosition.x - xPosition,
					m_CachedMousePosition.y - yPosition
				};

				m_Position += delta * g_MoveSpeed * (m_Scale * 0.33f);
			}

			m_CachedMousePosition.x = xPosition;
			m_CachedMousePosition.y = yPosition;
		} break;

		default: break;
	}
}

Vector2 Camera::GetPosition() const noexcept
{
	return m_Position;
}

void Camera::SetPosition(const Vector2 position) noexcept
{
	m_Position = position;
}

void Camera::SetAspectRatio(const float aspectRatio) noexcept
{
	m_AspectRatio = aspectRatio;
	RecalculateProjection();
}

void Camera::SetScale(const float scale) noexcept
{
	m_Scale = scale > 0.5f ? scale : 0.5f;
	m_Scale = m_Scale < 7.0f ? m_Scale : 7.0f;
	RecalculateProjection();
}

float Camera::GetAspectRatio() const noexcept
{
	return m_AspectRatio;
}

float Camera::GetScale() const noexcept
{
	return m_Scale;
}

void Camera::RecalculateProjection() noexcept
{
	const float orthoLeft{ -m_Scale * m_AspectRatio * 0.5f };
	const float orthoRight{ m_Scale * m_AspectRatio * 0.5f };
	const float orthoBottom{ -m_Scale * 0.5f };
	const float orthoTop{ m_Scale * 0.5f };

	m_ProjectionMatrix = OrthographicProjection
	(
		orthoLeft + m_Position.x, 
		orthoRight + m_Position.x, 
		orthoBottom - m_Position.y, 
		orthoTop - m_Position.y
	);
}

Matrix4x4 Camera::GetViewProjection() const noexcept
{
	return m_ViewMatrix * m_ProjectionMatrix;
}

const Matrix4x4& Camera::GetViewMatrix() const noexcept
{
	return m_ViewMatrix;
}

const Matrix4x4& Camera::GetProjectionMatrix() const noexcept
{
	return m_ProjectionMatrix;
}