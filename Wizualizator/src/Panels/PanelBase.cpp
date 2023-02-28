#include "Panels/PanelBase.hpp"

PanelBase::PanelBase(const std::string_view panelName) noexcept
	:
	m_PanelName(panelName),
	m_IsEnabled(true)
{}

std::string_view PanelBase::GetPanelName() const
{
	return m_PanelName;
}

bool PanelBase::IsEnabled() const
{
	return m_IsEnabled;
}

void PanelBase::SetEnabled(const bool enabled) const
{
	m_IsEnabled = enabled;
}

void PanelBase::OnEvent(const Event&)
{}