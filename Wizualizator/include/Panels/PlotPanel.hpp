#pragma once
#include "Panels/PanelBase.hpp"

class PlotPanel final : public PanelBase
{
private:
public:
	explicit PlotPanel() noexcept;
	virtual ~PlotPanel() noexcept = default;

	virtual void OnGUIRender() override final;
private:
	void DrawOpenReadingFrames(const std::vector<uint32_t>& residueSizes, const std::string& sequenceName);
};