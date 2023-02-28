#include "Panels/PropertiesPanel.hpp"
#include "GUI.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

PropertiesPanel::PropertiesPanel() noexcept
	:
	PanelBase("Properties Panel")
{}

std::string RemoveTrailingZeros(std::string outString)
{
	outString.erase(outString.find_last_not_of('0') + 1, std::string::npos);
	return outString;
}

void PropertiesPanel::OnGUIRender()
{
	const auto& colors{ ImGui::GetStyle().Colors };
	ImGui::PushStyleColor(ImGuiCol_WindowBg, colors[ImGuiCol_FrameBg]);
	ImGui::Begin("Properties");

	Project::OnSequenceSelected
	(
		[this](const Project::NucleotideSequenceCache& nucleotideSequenceCache)
		{
			if (ImGui::BeginTable("Properties Table", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_NoHostExtendY, { 270.0f, 64.0f }))
			{
				ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 150.0f);
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				GUI::Text("Sequence Length: ");
				GUI::Text("Length of amino chain: ");
				GUI::Text("Number of ORFs: ");
				
				ImGui::TableSetColumnIndex(1);
				GUI::Text(std::to_string(nucleotideSequenceCache.NucleotideSequence.size()));
				GUI::Text(std::to_string(nucleotideSequenceCache.AminoSequence.size()));
				GUI::Text(std::to_string(nucleotideSequenceCache.ProteinCandidates.size()));
				
				ImGui::EndTable();
			}
		},
		[this](const Project::NucleotideSequencePeptideCache& nucleotideSequencePeptideCache)
		{
			RenderDesiredSequenceProperties
			(
				"H-Sequence-OH",
				nucleotideSequencePeptideCache.MolecularWeight,
				nucleotideSequencePeptideCache.Formula,
				nucleotideSequencePeptideCache.NetCharge,
				nucleotideSequencePeptideCache.IsoeletricPoint,
				nucleotideSequencePeptideCache.ExtinctionCoefficient,
				nucleotideSequencePeptideCache.ExtinctionCoefficientReduced,
				nucleotideSequencePeptideCache.IsoeletricPointPlotData,
				nucleotideSequencePeptideCache.HydropathyPlotData
			);

		},
		[this](const Project::AminoSequenceCache& aminoSequenceCache)
		{
			RenderDesiredSequenceProperties
			(
				"H-Sequence-OH",
				aminoSequenceCache.MolecularWeight,
				aminoSequenceCache.Formula,
				aminoSequenceCache.NetCharge,
				aminoSequenceCache.IsoeletricPoint,
				aminoSequenceCache.ExtinctionCoefficient,
				aminoSequenceCache.ExtinctionCoefficientReduced,
				aminoSequenceCache.IsoeletricPointPlotData,
				aminoSequenceCache.HydropathyPlotData
			);
		},
		[this](const Project::AminoSequencePeptideCache& aminoSequencePeptideCache)
		{
			RenderDesiredSequenceProperties
			(
				"H-Sequence-OH",
				aminoSequencePeptideCache.MolecularWeight,
				aminoSequencePeptideCache.Formula,
				aminoSequencePeptideCache.NetCharge,
				aminoSequencePeptideCache.IsoeletricPoint,
				aminoSequencePeptideCache.ExtinctionCoefficient,
				aminoSequencePeptideCache.ExtinctionCoefficientReduced,
				aminoSequencePeptideCache.IsoeletricPointPlotData,
				aminoSequencePeptideCache.HydropathyPlotData
			);
		}
	);

	ImGui::End();
	ImGui::PopStyleColor();
}

void PropertiesPanel::RenderDesiredSequenceProperties(
	const std::string_view inputSequence, 
	const std::optional<const double> molecularWeight,
	const std::optional<const Bio::PeptideFormula> formula,
	const std::optional<const double> netCharge,
	const std::optional<const double> isoelectricPoint,
	const std::optional<const size_t> extinctionCoeffient,
	const std::optional<const size_t> extinctionCoeffientReduced,
	const std::optional<std::shared_ptr<Project::IsoelectricPointPlotData_t>> isoelectricPointPlotData,
	const std::optional<std::shared_ptr<Project::HydropathyPlotData_t>> hydropathyPlotData) noexcept
{
	if (ImGui::BeginTable("Properties Table", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_NoHostExtendY, { 270.0f, 195.0f }))
	{
		ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 117.5f);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		ImGui::Text("PH Level");
		ImGui::SameLine();

		float netChargePHCopy{ Project::GetCalculationContext().NetCharge.PH };
		if (ImGui::InputFloat("##phSlider", &netChargePHCopy, 0.0f, 0.0f, "%.1f"))
		{
			netChargePHCopy = netChargePHCopy > 14.0f ? 14.0f : netChargePHCopy;
			netChargePHCopy = netChargePHCopy < 0.1f ? 0.1f : netChargePHCopy;

			Project::GetCalculationContext().NetCharge.PH = netChargePHCopy;
			Project::RecalculateNetCharge();
		}

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		GUI::Text("Input sequence:");
		GUI::Text("Molecular Weight:");
		GUI::Text("Formula:");
		GUI::Text("Net Charge:");
		GUI::Text("Status:");
		GUI::Text("Isoelectric Point:");
		GUI::Text("Ext. Coeff.:");
		GUI::Text("Ext. Coeff. Reduced:");
		
		ImGui::TableSetColumnIndex(1);
		GUI::Text(inputSequence);

		BIO_LIKELY
		if (molecularWeight)
		{
			std::ostringstream precisionConverter;
			precisionConverter.precision(6);

			precisionConverter << molecularWeight.value();
			const std::string molecularWeightString{ precisionConverter.str() };
			precisionConverter.clear();

			GUI::Text(molecularWeightString + " g/mol");
		}
		else
			GUI::Text("None");

		if (formula.has_value())
		{
			{
				constexpr float subscriptTextScale{ 0.85f };
				const ImVec2 cachedPosition{ ImGui::GetCursorPos() };
				ImGui::BeginChild("XDDDDD", { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y });

				ImGui::SetWindowFontScale(subscriptTextScale);
				if (formula->Carbon)
				{
					ImGui::SetCursorPos({ ImGui::GetCursorPosX() + ImGui::CalcTextSize("C").x, ImGui::GetCursorPosY() + 10.0f });
					ImGui::Text(std::to_string(formula->Carbon).c_str());
					ImGui::SameLine();
				}
				if (formula->Hydrogen)
				{
					ImGui::SetCursorPos({ ImGui::GetCursorPosX() + ImGui::CalcTextSize("H").x - 5.0f, ImGui::GetCursorPosY() });
					ImGui::Text(std::to_string(formula->Hydrogen).c_str());
					ImGui::SameLine();
				}
				if (formula->Nitrogen)
				{
					ImGui::SetCursorPos({ ImGui::GetCursorPosX() + ImGui::CalcTextSize("N").x - 5.0f, ImGui::GetCursorPosY() });
					ImGui::Text(std::to_string(formula->Nitrogen).c_str());
					ImGui::SameLine();
				}
				if (formula->Oxygen)
				{
					ImGui::SetCursorPos({ ImGui::GetCursorPosX() + ImGui::CalcTextSize("O").x - 5.0f, ImGui::GetCursorPosY() });
					ImGui::Text(std::to_string(formula->Oxygen).c_str());
					ImGui::SameLine();
				}
				if (formula->Sulphur)
				{
					ImGui::SetCursorPos({ ImGui::GetCursorPosX() + ImGui::CalcTextSize("S").x - 7.0f, ImGui::GetCursorPosY() });
					ImGui::Text(std::to_string(formula->Sulphur).c_str());
				}

				ImGui::EndChild();
				ImGui::SetCursorPos(cachedPosition);

				if (formula->Carbon)
				{
					GUI::Text("C");
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f + ImGui::CalcTextSize(std::to_string(formula->Carbon).c_str()).x * subscriptTextScale);
				}
				if (formula->Hydrogen)
				{
					GUI::Text("H");
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f + ImGui::CalcTextSize(std::to_string(formula->Hydrogen).c_str()).x * subscriptTextScale);
				}
				if (formula->Nitrogen)
				{
					GUI::Text("N");
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f + ImGui::CalcTextSize(std::to_string(formula->Nitrogen).c_str()).x * subscriptTextScale);
				}
				if (formula->Oxygen)
				{
					GUI::Text("O");
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f + ImGui::CalcTextSize(std::to_string(formula->Oxygen).c_str()).x * subscriptTextScale);
				}
				if (formula->Sulphur)
				{
					GUI::Text("S");
				}
				else
					ImGui::NewLine();
			}
		}
		else
			GUI::Text("None");

		if (netCharge.has_value())
		{
			const std::string netChargeString{ std::to_string(static_cast<int64_t>(netCharge.value())) };
			GUI::Text(netChargeString);
		}
		else
			GUI::Text("None");

		if (netCharge.has_value())
		{
			if (netCharge.value() > 0.0)
				GUI::Text("Basic");
			else if (netCharge.value() == 0.0)
				GUI::Text("Neutral");
			else if (netCharge.value() < 0.0)
				GUI::Text("Acidic");
		}

		if (isoelectricPoint.has_value())
		{
			std::ostringstream precisionConverter;
			precisionConverter.precision(6);

			precisionConverter << isoelectricPoint.value();
			const std::string isoelectricPointString{ precisionConverter.str() };
			precisionConverter.clear();

			GUI::Text(isoelectricPointString);
		}
		else
			GUI::Text("None");

		if (extinctionCoeffient.has_value())
			GUI::Text(std::to_string(extinctionCoeffient.value()) + " M-1 cm-1");
		else
			GUI::Text("None");

		if (extinctionCoeffientReduced.has_value())
			GUI::Text(std::to_string(extinctionCoeffientReduced.value()) + " M-1 cm-1");
		else
			GUI::Text("None");
		
		ImGui::EndTable();
	}

	/* Render plots */
	constexpr ImPlotFlags dataPlotFlags
	{
		ImPlotFlags_NoLegend |		// Disable plot legend	
		ImPlotFlags_NoInputs |		// Disable inputs
		ImPlotFlags_NoMouseText		// Disable mouse text when hovering
	};

	constexpr ImGuiTableFlags plotTableFlags
	{
		ImGuiTableFlags_BordersOuter |
		ImGuiTableFlags_BordersInner |
		ImGuiTableFlags_BordersV |
		ImGuiTableFlags_RowBg
	};

	constexpr ImVec2 dataPlotSize{ 270.0f, 270.0f };

	if (ImGui::BeginTable("IsoelectricPoint##lptable", 1, plotTableFlags, dataPlotSize))
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		if (isoelectricPointPlotData.has_value())
		{
			/* Isoelectric point plot */
			if (ImPlot::BeginPlot("Isoelectric point", dataPlotSize, dataPlotFlags))
			{
				ImPlot::SetupAxes(
					"pH",
					"Net Charge",
					ImPlotAxisFlags_AutoFit,
					ImPlotAxisFlags_AutoFit);

				ImPlot::PlotLine(
					"Values",
					isoelectricPointPlotData.value()->TestValues.data(),
					isoelectricPointPlotData.value()->TestResults.data(),
					static_cast<int>(isoelectricPointPlotData.value()->TestResults.size()));
				ImPlot::EndPlot();
			}

			if (ImGui::IsItemHovered() && ImGui::GetIO().MouseWheel)
			{
				const float scroll_speed = ImFloor(ImGui::GetCurrentWindow()->CalcFontSize() * 100.0f * ImGui::GetIO().DeltaTime * 7.5f);
				ImGui::SetScrollY(ImGui::GetScrollY() - ImGui::GetIO().MouseWheel * scroll_speed);
			}
		}

		ImGui::EndTable();
	}

	/* Hydropathy plot table */
	if(ImGui::BeginTable("Hydropathy##hptable", 1, plotTableFlags, dataPlotSize))
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		static int value[1]{ 3 };		
		ImGui::Text("Window Size: ");
		ImGui::SameLine();

		
		int casted{ static_cast<int>(Project::GetCalculationContext().Hydropathy.WindowSize) };
		if (ImGui::InputInt("##windowSlider", &casted, 0, 0))
		{
			casted = casted < 3 ? 3 : casted;
			casted = casted % 2 == 0 ? casted + 1 : casted;

			Project::GetCalculationContext().Hydropathy.WindowSize = static_cast<size_t>(casted);
			Project::RecalculateHydropathy();
		}

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		if (hydropathyPlotData.has_value())
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::GetStyle().FramePadding.x);
			
			/* Hydropathy plot */
			if (ImPlot::BeginPlot("Hydropathy (Kyte & Doolittle)", dataPlotSize, dataPlotFlags))
			{
				std::vector<double> indices;
				size_t magicNumber{ Project::GetCalculationContext().Hydropathy.WindowSize / 2U };
				
				for ([[maybe_unused]] const auto _ : hydropathyPlotData.value()->HydropathyIndices)
					indices.push_back(static_cast<double>(++magicNumber));

				ImPlot::SetupAxes(
					"Position",
					"Score",
					ImPlotAxisFlags_AutoFit,
					ImPlotAxisFlags_AutoFit);

				ImPlot::PlotLine(
					"HP",
					indices.data(),
					hydropathyPlotData.value()->HydropathyIndices.data(),
					static_cast<int>(hydropathyPlotData.value()->HydropathyIndices.size()));

				ImPlot::EndPlot();
				if (ImGui::IsItemHovered() && ImGui::GetIO().MouseWheel)
				{
					const float scrollSpeed{ ImFloor(ImGui::GetCurrentWindow()->CalcFontSize() * 100.0f * ImGui::GetIO().DeltaTime * 7.5f) };
					ImGui::SetScrollY(ImGui::GetScrollY() - ImGui::GetIO().MouseWheel * scrollSpeed);
				}
			}
		}
		else
		{
			ImGui::NewLine();
			GUI::TextCentered("Sequence is too short for speficied window size");
		}

		ImGui::EndTable();
	}

	/* Atomic composition plot table */
	if (ImGui::BeginTable("AtomicComp##table", 1, plotTableFlags, dataPlotSize))
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		if (formula.has_value())
		{
			ImPlot::PushColormap(ImPlotColormap_Pastel);
			/* Atomic composition plot */
			ImGui::SetCursorPosX(dataPlotSize.x * 0.2f);
			if (ImPlot::BeginPlot("Atomic composition", { dataPlotSize.x * 0.80f, dataPlotSize.y }, (dataPlotFlags & ~ImPlotFlags_NoLegend)))
			{
				std::vector<const char*> labels;
				std::vector<size_t> values;

				const auto& atomicFormula{ formula.value() };
				if (atomicFormula.Carbon)
				{
					labels.push_back("C");
					values.push_back(atomicFormula.Carbon);
				}

				if (atomicFormula.Hydrogen)
				{
					labels.push_back("H");
					values.push_back(atomicFormula.Hydrogen);
				}

				if (atomicFormula.Nitrogen)
				{
					labels.push_back("N");
					values.push_back(atomicFormula.Nitrogen);
				}

				if (atomicFormula.Oxygen)
				{
					labels.push_back("O");
					values.push_back(atomicFormula.Oxygen);
				}

				if (atomicFormula.Sulphur)
				{
					labels.push_back("S");
					values.push_back(atomicFormula.Sulphur);
				}

				ImPlot::SetupAxes(
					"Amount",
					"Element",
					ImPlotAxisFlags_NoDecorations | ImPlotAxisFlags_AutoFit,
					ImPlotAxisFlags_NoDecorations | ImPlotAxisFlags_AutoFit);

				ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Horizontal | ImPlotLegendFlags_Outside | ImPlotLegendFlags_NoButtons);
				ImPlot::PlotPieChart(labels.data(), values.data(), static_cast<int>(values.size()), 10.0, 10.0, 10, "%.0f", 90.0, ImPlotPieChartFlags_Normalize);
				ImPlot::EndPlot();

				if (ImGui::IsItemHovered() && ImGui::GetIO().MouseWheel)
				{
					const float scrollSpeed{ ImFloor(ImGui::GetCurrentWindow()->CalcFontSize() * 100.0f * ImGui::GetIO().DeltaTime * 7.5f) };
					ImGui::SetScrollY(ImGui::GetScrollY() - ImGui::GetIO().MouseWheel * scrollSpeed);
				}
			}

			ImPlot::PopColormap();
		}

		ImGui::EndTable();
	}
}