#include "Application.hpp"
#include "Event.hpp"
#include "Window.hpp"
#include "Renderer.hpp"
#include "GUIRenderer.hpp"

Application::Application(
	CommandLineArguments&& arguments, 
	const std::string_view windowName,
	const uint32_t windowWidth,
	const uint32_t windowHeight,
	const bool vSync)
	:
	m_Arguments(std::move(arguments)),
	m_Window(std::make_unique<Window>(windowName, windowWidth, windowHeight, vSync)),
	m_Renderer(std::make_unique<Renderer>(m_Window)),
	m_GUIRenderer(std::make_unique<GUIRenderer>(m_Window)),
	m_IsRunning(true),
	m_IsMinimized(false)
{
	m_Window->SetEventCallback(
		[this](const Event& event)
		{
			switch (event.GetEventType())
			{
				case EEventType::WindowClosed:
				{
					m_IsRunning = false;
					break;
				}

				case EEventType::WindowResized:
				{
					const WindowResizedEvent& windowResizedEvent{ EventCast<WindowResizedEvent>(event) };
					const auto [width, height] { windowResizedEvent.GetResize() };

					m_IsMinimized = (width < 1U) || (height < 1U);

					BIO_LIKELY
					if (!m_IsMinimized)
						m_Renderer->SetViewportSize(width, height);

					break;
				}

				case EEventType::MouseScrolled:
					break;

				case EEventType::MouseCursorMoved:
					break;

				default:
				{
					LOG("Unhandled event!");
					break;
				}
			}

			m_GUIRenderer->OnEvent(event);
			OnEvent(event);
		});
}

Application::~Application() noexcept
{
	/* Explicit order */
	m_GUIRenderer.reset();
	m_Renderer.reset();
	m_Window.reset();
}

void Application::TryInitialize()
{
	LOG("Specified command line arguments:");
	for (const auto argc : m_Arguments)
		LOG(argc);
}

void Application::TryRun()
{
	BIO_LIKELY
	while (m_IsRunning)
	{
		m_Window->PollEvents();	

		BIO_LIKELY
		if (!m_IsMinimized)
		{
			m_Renderer->BeginFrame(0.1f, 0.1f, 0.1f, 1.0f);
			{
				m_GUIRenderer->BeginFrame();
				{	
					OnGUIRender();
				}
				m_GUIRenderer->EndFrame();
			}
			m_Renderer->EndFrame();
		}
		
		m_Window->AcquireNextSwapchainImage();	
	}
}

void Application::TryShutdown()
{}