#ifdef BIO_PLATFORM_WINDOWS
#include "Window.hpp"

#ifdef APIENTRY
#undef APIENTRY
#endif

#include "glad/glad.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

constinit static std::atomic<bool>		s_IsGLFWInitialized{ false };
constinit static std::once_flag			s_GLFWInitializedFlag;
constinit static std::atomic<uint32_t>	s_WindowInstanceCounter;
static std::mutex						s_WindowDestructionMutex;

Window::Window(
	const std::string_view name, 
	const uint32_t width, 
	const uint32_t height, 
	const bool vSync)
	:
	m_Handle
	(
		[=]() -> GLFWwindow* 
		{
			std::call_once(s_GLFWInitializedFlag, []()
			{
				BIO_UNLIKELY
				if(!glfwInit())
					THROW_EXCEPTION("Failed initializing glfw");

				s_IsGLFWInitialized = true;
			});

			while (!s_IsGLFWInitialized)
				Sleep(50);
				
			++s_WindowInstanceCounter;
			return glfwCreateWindow(width, height, name.data(), nullptr, nullptr);
		}()
	),
	m_Name(name.data()),
	m_Width(width),
	m_Height(height),
	m_VSync(vSync),
	m_Callback(nullptr)
{
	BIO_UNLIKELY
	if (!m_Handle)
		THROW_EXCEPTION("Failed to create glfw window");
}

Window::~Window() noexcept
{
	const std::scoped_lock<std::mutex> windowDestructionLock{ s_WindowDestructionMutex };
	--s_WindowInstanceCounter;

	BIO_LIKELY
	if(m_Handle)
		glfwDestroyWindow(m_Handle);

	if (s_WindowInstanceCounter < 1U)
		glfwTerminate();
}

void Window::PollEvents() const
{
	glfwPollEvents();
}

void Window::AcquireNextSwapchainImage() const
{
	BIO_ASSERT(m_Handle);
	glfwSwapBuffers(m_Handle);
}

void Window::SetName(const std::string_view name)
{
	BIO_ASSERT(m_Handle);
	glfwSetWindowTitle(
		m_Handle, 
		name.data());
}

void Window::SetEventCallback(const EventCallbackFunction callback)
{
	BIO_ASSERT(callback);
	m_Callback = callback;
	
	glfwSetWindowUserPointer(m_Handle, this);
	glfwSetWindowCloseCallback(m_Handle, [](GLFWwindow* const window)
	{
		const Window* const userWindow{ reinterpret_cast<const Window* const>(glfwGetWindowUserPointer(window)) };
		userWindow->m_Callback(WindowClosedEvent{ userWindow });
	});

	glfwSetFramebufferSizeCallback(m_Handle, [](GLFWwindow* const window, const int width, const int height) 
	{
		const Window* const userWindow{ reinterpret_cast<const Window* const>(glfwGetWindowUserPointer(window)) };
		userWindow->m_Callback(WindowResizedEvent{ userWindow, static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
	});

	glfwSetScrollCallback(m_Handle, [](GLFWwindow* const window, const double xOffset, const double yOffset)
	{
		const Window* const userWindow{ reinterpret_cast<const Window* const>(glfwGetWindowUserPointer(window)) };
		userWindow->m_Callback(MouseScrolledEvent{ static_cast<int16_t>(xOffset), static_cast<int16_t>(yOffset) });
	});

	glfwSetCursorPosCallback(m_Handle, [](GLFWwindow* const window, const double xPosition, const double yPosition)
	{
		const Window* const userWindow{ reinterpret_cast<const Window* const>(glfwGetWindowUserPointer(window)) };
		userWindow->m_Callback(MouseCursorMovedEvent{ static_cast<float>(xPosition), static_cast<float>(yPosition) });
	});
}

GLFWwindow* Window::GetHandle()
{
	BIO_ASSERT(m_Handle);
	return m_Handle;
}
#endif