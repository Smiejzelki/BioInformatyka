#pragma once
#include "Math.hpp"

class Event;

class Camera
{
public:
	Camera(const float aspectRatio, const float scale) noexcept;
	~Camera() noexcept = default;

	void OnEvent(const Event& event, const bool shouldUpdate) noexcept;

	Vector2 GetPosition() const noexcept;
	void SetPosition(const Vector2 position) noexcept;

	void SetAspectRatio(const float aspectRatio) noexcept;
	void SetScale(const float scale) noexcept;

	float GetAspectRatio() const noexcept;
	float GetScale() const noexcept;

	void RecalculateProjection() noexcept;
	Matrix4x4 GetViewProjection() const noexcept;

	const Matrix4x4& GetViewMatrix() const noexcept;
	const Matrix4x4& GetProjectionMatrix() const noexcept;
private:
	float m_AspectRatio;
	float m_Scale;
	Vector2 m_CachedMousePosition;

	Vector2 m_Position;
	Matrix4x4 m_ViewMatrix;
	Matrix4x4 m_ProjectionMatrix;
};