#pragma once
#include "Panels/PanelBase.hpp"

class PropertiesPanel final : public PanelBase
{
private:
public:
	explicit PropertiesPanel() noexcept;
	virtual ~PropertiesPanel() noexcept = default;

	virtual void OnGUIRender() override final;
private:
	void RenderDesiredSequenceProperties(
		const std::string_view inputSequence,
		const std::optional<const double> molecularWeight,
		const std::optional<const Bio::PeptideFormula> formula,
		const std::optional<const double> netCharge,
		const std::optional<const double> isoelectricPoint,
		const std::optional<const size_t> extinctionCoeffient,
		const std::optional<const size_t> extinctionCoeffientReduced,
		const std::optional<std::shared_ptr<Project::IsoelectricPointPlotData_t>> isoelectricPointPlotData = std::nullopt,
		const std::optional<std::shared_ptr<Project::HydropathyPlotData_t>> hydropathyPlotData = std::nullopt) noexcept;
};