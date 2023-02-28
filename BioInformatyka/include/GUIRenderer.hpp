#pragma once
#include "Core.hpp"

class Window;
class Event;

class GUIRenderer
{
public:
	explicit GUIRenderer(const std::unique_ptr<Window>& windowContext);
	~GUIRenderer() noexcept;

	void BeginFrame();
	void EndFrame();
	void OnEvent(const Event& event);
private:
};