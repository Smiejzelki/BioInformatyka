#pragma once
#include "Core.hpp"
#include "Project.hpp"

class Event;

class PanelBase
{
private:
	NON_COPYABLE(PanelBase)
public:
	explicit PanelBase(const std::string_view panelName) noexcept;
	virtual ~PanelBase() noexcept = default;

	std::string_view GetPanelName() const;
	bool IsEnabled() const;
	void SetEnabled(const bool enabled) const;

	virtual void OnGUIRender() = 0;
	virtual void OnEvent(const Event& event);
private:
	const std::string_view m_PanelName;
	mutable bool m_IsEnabled;
};