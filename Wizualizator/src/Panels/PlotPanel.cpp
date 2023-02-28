#include "Panels/PlotPanel.hpp"
#include "GUI.hpp"
#include "implot.h"

static std::vector<std::string> s_Container;
static std::vector<const char*> s_ORFLabels;
constexpr uint32_t g_MaxOpenReadingFramesCount{ 1 << 18U }; /* 2^18 */ 

static bool SetupORFLabels
{
	[]()
	{
		s_Container = std::vector<std::string>();
		s_ORFLabels = std::vector<const char*>();
		s_Container.resize(g_MaxOpenReadingFramesCount);
		s_ORFLabels.resize(g_MaxOpenReadingFramesCount);

		for (uint32_t i{ 0U }; i < g_MaxOpenReadingFramesCount - 1U; ++i)
		{
			s_Container[i] = "ORF-" + std::to_string(i);
			s_ORFLabels[i] = (s_Container[i].c_str());
		}

		s_Container[g_MaxOpenReadingFramesCount - 1U] = "LIMIT:" + std::to_string(g_MaxOpenReadingFramesCount);
		s_ORFLabels[g_MaxOpenReadingFramesCount - 1U] = s_Container[g_MaxOpenReadingFramesCount - 1U].c_str();
		return true;
	}() 
};

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

PlotPanel::PlotPanel() noexcept
	:
	PanelBase("ORF Panel")
{}

void PlotPanel::OnGUIRender()
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.1f, 0.1f, 0.1f, 1.0f });
	ImGui::Begin("Open reading frames##ORF", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	
	if (!Project::OnSequenceSelected
		(
			[this](const Project::NucleotideSequenceCache& nucleotideSequenceCache)
			{
				DrawOpenReadingFrames(nucleotideSequenceCache.ProteinCandidateLengths, nucleotideSequenceCache.SequenceName);
			},
			[this](const Project::NucleotideSequencePeptideCache& nucleotideSequencePeptideCache)
			{
				DrawOpenReadingFrames(nucleotideSequencePeptideCache.ParentCache.ProteinCandidateLengths, nucleotideSequencePeptideCache.ParentCache.SequenceName);
			},
			[this](const Project::AminoSequenceCache& aminoSequenceCache)
			{
				DrawOpenReadingFrames(aminoSequenceCache.ProteinCandidateLengths, aminoSequenceCache.SequenceName);
			},
			[this](const Project::AminoSequencePeptideCache& aminoSequencePeptideCache)
			{
				DrawOpenReadingFrames(aminoSequencePeptideCache.ParentCache.ProteinCandidateLengths, aminoSequencePeptideCache.ParentCache.SequenceName);
			}
		)
	)
	{
		GUI::TextCentered("No sequence selected");
	}

	ImGui::PopStyleColor();
	ImGui::End();
}

void PlotPanel::DrawOpenReadingFrames(const std::vector<uint32_t>& residueSizes, const std::string& sequenceName)
{
	if (residueSizes.empty())
	{
		GUI::TextCentered("No open reading frames were found for " + sequenceName);
		return;
	}

	if (!residueSizes.empty())
	{
		ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, { 0.0f, 0.0f });
		ImPlot::PushStyleVar(ImPlotStyleVar_LabelPadding, { 1.0f, 1.0f });
		ImPlot::PushColormap(GUI::g_ExtendedPlasmaColorMapID);

		constexpr ImPlotFlags openReadingFramesPlotFlags
		{
			ImPlotFlags_NoLegend		| // Disable legend  
			ImPlotFlags_NoMouseText		| // Disable displaying mouse positon
			ImPlotFlags_NoMenus			| // Disable menu box when left clicked
			ImPlotFlags_NoFrame			| // Disable plot frame, saving space
			ImPlotFlags_NoBoxSelect		  // Disable box selecting for smooth click callbacks
		};

		const std::string plotTitle{ "Residue indices of sequence " + sequenceName };
		const uint32_t residueSizesSum{ std::accumulate(residueSizes.begin(), residueSizes.end(), 0U) };

		if (ImPlot::BeginPlot(plotTitle.c_str(), ImVec2{-1.0f, 120.0f}, openReadingFramesPlotFlags))
		{
			ImPlot::SetupAxes
			(
				nullptr, 
				"ORF",
				ImPlotAxisFlags_Opposite,
				ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels
			);

			ImPlot::SetupAxisZoomConstraints(ImAxis_X1, 0.0, residueSizesSum);
			ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, 0.0, residueSizesSum);

			ImPlot::PlotBarGroups
			(
				s_ORFLabels.data(),
				residueSizes.data(),
				residueSizes.size() < g_MaxOpenReadingFramesCount ? static_cast<uint32_t>(residueSizes.size()) : g_MaxOpenReadingFramesCount,
				1,
				1.0,
				0.0,
				ImPlotBarGroupsFlags_Stacked | ImPlotBarGroupsFlags_Horizontal
			);

#ifdef min
#undef min
#endif
			size_t offset{ 0U };
			if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && ImPlot::IsPlotHovered())
			{
				ImPlotRect currentRectangle{ 0.0, 0.0, -std::numeric_limits<double>::max(), std::numeric_limits<double>::max() };
				const size_t size{ residueSizes.size() > s_ORFLabels.size() ? s_ORFLabels.size() : residueSizes.size() };

				for (size_t i{ 0U }; i < size; ++i)
				{
					const uint32_t length{ residueSizes[i] };
					const double labelPosition{ (length * 0.5) + offset };
					ImPlot::PlotText(s_ORFLabels[i], labelPosition, 0.0f, ImVec2(0, 15.0), ImPlotTextFlags_Vertical);

					currentRectangle.X.Min = (double)offset;
					currentRectangle.X.Max = (double)length + (double)offset;

					offset += length;
					if (currentRectangle.Contains(ImPlot::GetPlotMousePos()))
					{
						Project::InvalidateSelectionContext
						(
							Project::SelectedSequenceType(),
							Project::SelectedSequence(), 
							Project::SelectedFrame(), 
							static_cast<ID>(i)
						);
					}
				}
			}
			else
			{
				const size_t size{ residueSizes.size() > s_ORFLabels.size() ? s_ORFLabels.size() : residueSizes.size() };
				for (size_t i{ 0U }; i < size; ++i)
				{
					const uint32_t length{ residueSizes[i] };
					const double labelPosition{ (length * 0.5) + offset };
					ImPlot::PlotText(s_ORFLabels[i], labelPosition, 0.0f, ImVec2(0, 15.0), ImPlotTextFlags_Vertical);

					offset += length;
				}
			}

			ImPlot::EndPlot();
		}

		ImPlot::PopColormap();
		ImPlot::PopStyleVar();
		ImPlot::PopStyleVar();
	}
}