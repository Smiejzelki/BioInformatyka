#pragma once
#include "Core.hpp"

class Event;
class Window;
class Renderer;
class GUIRenderer;

class Application
{
private:
	NON_COPYABLE(Application)
public:
	explicit Application(
		CommandLineArguments&& arguments, 
		const std::string_view windowName, 
		const uint32_t windowWidth, 
		const uint32_t windowHeight, 
		const bool vSync);
		
	virtual ~Application() noexcept;

	void TryInitialize();
	void TryRun();
	void TryShutdown();
	
	virtual void OnGUIRender() = 0;
	virtual void OnEvent(const Event& event) = 0;
protected:
	CommandLineArguments m_Arguments;
	std::unique_ptr<Window> m_Window;
	std::unique_ptr<Renderer> m_Renderer;
	std::unique_ptr<GUIRenderer> m_GUIRenderer;
	bool m_IsRunning;
	bool m_IsMinimized;
};

#define REGISTER_APPLICATION(application)													\
Application* CreateApplication(CommandLineArguments&& arguments) noexcept					\
{																							\
	return new application(std::move(arguments));											\
}