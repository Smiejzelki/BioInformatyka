#pragma once
#include "Core.hpp"
#include "Event.hpp"

struct GLFWwindow;

class Window
{
private:
	NON_COPYABLE(Window)
public:
	explicit Window(
		const std::string_view name, 
		const uint32_t width, 
		const uint32_t height, 
		const bool vSync);
	
	~Window() noexcept;

	void PollEvents() const;
	void AcquireNextSwapchainImage() const;
	void SetName(const std::string_view name);
	void SetEventCallback(const EventCallbackFunction callback);

	GLFWwindow* GetHandle();
private:
	GLFWwindow* m_Handle;
	const char* m_Name;
	uint32_t m_Width;
	uint32_t m_Height;
	bool m_VSync;

	EventCallbackFunction m_Callback;
private:
	friend class Renderer;
};