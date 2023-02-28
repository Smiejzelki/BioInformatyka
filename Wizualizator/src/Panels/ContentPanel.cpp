#include "Panels/ContentPanel.hpp"
#include "GUI.hpp"
#include "Platform.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "FastaReader.hpp"

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

#define AMINO_TO_CODE_BUTTON(amino, code) if (ImGui::Button(amino)) sequenceBuffer += code

void DisplaySequenceImportPopup(const char* popupID, bool& outDisplay, const std::optional<const std::filesystem::path> updateImportPath = std::nullopt)
{
	static std::filesystem::path f_LocalImportPath;
	if (updateImportPath.has_value())
		f_LocalImportPath = updateImportPath.value();

	ImGuiViewport* mainViewport{ ImGui::GetMainViewport() };
	float popupWidth{ mainViewport->Size.x * 0.5f };
	float popupHeight{ mainViewport->Size.y * 0.35f };
	popupWidth = std::min(popupWidth, 400.0f);
	popupHeight = std::min(popupHeight, 190.0f);

	ImGui::SetNextWindowPos({ mainViewport->Size.x * 0.5f - popupWidth * 0.5f, 190.0f });
	ImGui::SetNextWindowSize({ popupWidth, popupHeight });

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
	if
		(
			ImGui::BeginPopupModal
			(
				popupID,
				nullptr,
				ImGuiWindowFlags_AlwaysAutoResize	|
				ImGuiWindowFlags_NoMove				|
				ImGuiWindowFlags_NoNavInputs		|
				0U
			)
		)
	{
		GUI::TextCentered("Import options");
		ImGui::NewLine();

		GUI::Text("File: " + f_LocalImportPath.string());
		const char* comboOptions[3U]{ "DNA", "RNA", "Peptide" };
		static size_t comboOptionIndex{ 0U };

		std::string_view projectionMode = comboOptions[comboOptionIndex];
		if (ImGui::BeginCombo("Import as##ImportCombo", projectionMode.data()))
		{
			for (size_t i{ 0U }; i < 3U; ++i)
			{
				const std::string_view modeIterator = comboOptions[i];
				const bool isSelected = projectionMode == modeIterator;

				if (!isSelected)
				{
					if (ImGui::Selectable(modeIterator.data(), isSelected))
						comboOptionIndex = i;
				}
				else
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		};

		ImGui::BeginDisabled(comboOptionIndex != 0U);
		static bool shouldReverse{ false };
		ImGui::Checkbox("Reverse (5'3' " ICON_FA_ARROW_RIGHT " 3'5')", &shouldReverse);
		ImGui::EndDisabled();

		if (ImGui::Button("Import"))
		{
			const std::string_view importAs{ comboOptions[comboOptionIndex] };
			try
			{
				FastaReader fastaReader(f_LocalImportPath);
				auto& project{ Project::Get() };

				if (importAs == "DNA")
				{
					for (const auto [sequenceName, sequence] : fastaReader.ReadNucleotideSequences())
					{
						auto& ref{ project->RegisterSequence<DnaMetadata>(sequenceName) };
						auto value{ DnaMetadata::Create(sequenceName, sequence, shouldReverse) };
						ref = std::move(value);
					}
				}
				else if (importAs == "RNA")
				{
					for (const auto [sequenceName, sequence] : fastaReader.ReadNucleotideSequences())
					{
						auto& ref{ project->RegisterSequence<RnaMetadata>(sequenceName) };
						auto value{ RnaMetadata::Create(sequenceName, sequence) };
						ref = std::move(value);
					}
				}
				else if (importAs == "Peptide")
				{
					for (const auto [sequenceName, sequence] : fastaReader.ReadNucleotideSequences())
					{
						auto& ref{ project->RegisterSequence<AminoMetadata>(sequenceName) };
						auto value{ AminoMetadata::Create(sequenceName, sequence) };
						ref = std::move(value);
					}
				}
			}
			catch (...)
			{
				HandleExceptions();
			}


			outDisplay = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			outDisplay = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::PopStyleVar();
}

template<typename SequenceType>
void DisplaySequenceTypeCreationPopup(const char* popupID, bool& display)
{
	ImGuiViewport* mainViewport{ ImGui::GetMainViewport() };
	const float popupWidth{ mainViewport->Size.x * 0.5f };
	const float popupHeight{ mainViewport->Size.y * 0.35f };

	ImGui::SetNextWindowPos({ mainViewport->Pos.x + popupWidth * 0.5f, mainViewport->Pos.y + popupHeight * 0.5f });
	ImGui::SetNextWindowSize({ popupWidth, popupHeight });

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
	if 
	(
		ImGui::BeginPopupModal
		(
			popupID, 
			nullptr, 
			ImGuiWindowFlags_AlwaysAutoResize	|	 
			ImGuiWindowFlags_NoMove				| 
			ImGuiWindowFlags_NoNavInputs		|
			0U
		)
	)
	{
		GUI::TextCentered("Name");

		auto& project{ Project::Get() };
		static std::string sequenceName{};
		if (sequenceName.empty())
		{
			if constexpr (std::is_same_v<SequenceType, Bio::DnaSequence>)
				sequenceName = "Unnamed DNA Sequence";
			else if constexpr (std::is_same_v<SequenceType, Bio::RnaSequence>)
				sequenceName = "Unnamed RNA Sequence";
			else if constexpr (std::is_same_v<SequenceType, Bio::AminoSequence>)
				sequenceName = "Unnamed Amino Sequence";
			else
				BIO_ASSERT(false);
		}

		const ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		char buffer[512]{ '\0' };
		memcpy(buffer, sequenceName.c_str(), sequenceName.length() > 512 ? 512 : sequenceName.length());
		ImGui::SetCursorPosX(popupWidth * 0.25f);
		ImGui::PushItemWidth(popupWidth * 0.5f);
		if (ImGui::InputText("##SequenceName", buffer, 512 - 1))
			sequenceName = std::string(buffer);

		ImGui::NewLine();
		GUI::TextCentered("Sequence");

		constexpr std::size_t g_Size{ 8916 * 8 };
		struct BufferWrapper
		{
			char* Buffer;

			BufferWrapper()
				:
				Buffer(new char[g_Size]{ '\0' })
			{}

			~BufferWrapper()
			{
				delete[] Buffer;
			}

			operator char* ()
			{
				return Buffer;
			}
		} static bufferWrapper{};

		static std::string sequenceBuffer{};
		if constexpr (std::is_same_v<SequenceType, Bio::AminoSequence>)
		{
			ImGui::Columns(10, nullptr, false);
			AMINO_TO_CODE_BUTTON("Ala (A)", 'A'); 
			ImGui::NextColumn();
			AMINO_TO_CODE_BUTTON("Arg (R)", 'R');
			ImGui::NextColumn();
			AMINO_TO_CODE_BUTTON("Asn (N)", 'N');
			ImGui::NextColumn();
			AMINO_TO_CODE_BUTTON("Asp (D)", 'D');
			ImGui::NextColumn();
			AMINO_TO_CODE_BUTTON("Cys (C)", 'C');
			ImGui::NextColumn();
			AMINO_TO_CODE_BUTTON("Gln (Q)", 'Q');
			ImGui::NextColumn();
			AMINO_TO_CODE_BUTTON("Glu (E)", 'E');
			ImGui::NextColumn();
			AMINO_TO_CODE_BUTTON("Gly (G)", 'G');
			ImGui::NextColumn();
			AMINO_TO_CODE_BUTTON("His (H)", 'H');
			ImGui::NextColumn();
			AMINO_TO_CODE_BUTTON("Ile (I)", 'I');
			ImGui::NextColumn();
			AMINO_TO_CODE_BUTTON("Leu (L)", 'L');
			ImGui::NextColumn();
			AMINO_TO_CODE_BUTTON("Lys (K)", 'K');
			ImGui::NextColumn();
			AMINO_TO_CODE_BUTTON("Met (M)", 'M');
			ImGui::NextColumn();
			AMINO_TO_CODE_BUTTON("Phe (F)", 'F');
			ImGui::NextColumn();
			AMINO_TO_CODE_BUTTON("Pro (P)", 'P');
			ImGui::NextColumn();
			AMINO_TO_CODE_BUTTON("Ser (S)", 'T');
			ImGui::NextColumn();
			AMINO_TO_CODE_BUTTON("Thr (T)", 'T');
			ImGui::NextColumn();
			AMINO_TO_CODE_BUTTON("Trp (W)", 'W');
			ImGui::NextColumn();
			AMINO_TO_CODE_BUTTON("Tyr (Y)", 'Y');
			ImGui::NextColumn();
			AMINO_TO_CODE_BUTTON("Val (V)", 'V');
			ImGui::NextColumn();
			ImGui::Columns(1);
		}

		memcpy(bufferWrapper, sequenceBuffer.c_str(), sequenceBuffer.length() > g_Size ? g_Size : sequenceBuffer.length());
		if (ImGui::InputTextMultiline("##SequenceBuffer", bufferWrapper, g_Size - 1, { popupWidth, popupHeight * 0.5f - 15.0f }))
			sequenceBuffer = std::string(bufferWrapper);

		[[maybe_unused]] static bool shouldReverse{ false };
		if constexpr (std::is_same_v<SequenceType, Bio::DnaSequence>)
			ImGui::Checkbox("Reverse (5'3' " ICON_FA_ARROW_RIGHT " 3'5')", &shouldReverse);

		if (ImGui::Button("Create"))
		{
			{
				if constexpr (std::is_same_v<SequenceType, Bio::DnaSequence>)
				{
					auto& ref{ project->RegisterSequence<DnaMetadata>(sequenceName) };
					auto value{ DnaMetadata::Create(sequenceName, sequenceBuffer, shouldReverse) };
					ref = std::move(value);
				}
				else if constexpr (std::is_same_v<SequenceType, Bio::RnaSequence>)
				{
					auto& ref{ project->RegisterSequence<RnaMetadata>(sequenceName) };
					auto value{ RnaMetadata::Create(sequenceName, sequenceBuffer) };
					ref = std::move(value);	
				}
				else if constexpr (std::is_same_v<SequenceType, Bio::AminoSequence>)
				{
					auto& ref{ project->RegisterSequence<AminoMetadata>(sequenceName) };
					auto value{ AminoMetadata::Create(sequenceName, sequenceBuffer) };
					ref = std::move(value);
				}
			}

			display = false;
			sequenceName.clear();
			sequenceBuffer.clear();
			memset(bufferWrapper, '\0', g_Size);
			memset(buffer, '\0', sizeof(buffer) / sizeof(buffer));
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			display = false;
			sequenceName.clear();
			sequenceBuffer.clear();
			memset(bufferWrapper, '\0', g_Size);
			memset(buffer, '\0', sizeof(buffer) / sizeof(buffer));
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::PopStyleVar();
}

ContentPanel::ContentPanel() noexcept
	:
	PanelBase("Content Panel")
{}

void ContentPanel::OnGUIRender()
{
	static bool f_DisplayRNACreation[g_FrameCount]{ false, false, false };
	static bool f_DisplayDNACreation[g_FrameCount]{ false, false, false };
	static bool f_DisplayAminoCreation[g_FrameCount]{ false, false, false };

	static bool f_DisplaySequenceImportPopup[g_FrameCount]{ false, false, false };
	static std::filesystem::path f_ImportPath;

	constexpr const char* g_FrameIndexWindowNames[g_FrameCount]{ "Frame 1", "Frame 2", "Frame 3", };
	
	const auto& colors{ ImGui::GetStyle().Colors };
	ImGui::PushStyleColor(ImGuiCol_WindowBg, colors[ImGuiCol_FrameBg]);
	for (uint32_t i{ 0U }; i < g_FrameCount; ++i)
	{
		/* Draw in reverse order to keep original layout */
		const uint32_t frameIndex{ g_FrameCount - i - 1U };
		ImGui::Begin(g_FrameIndexWindowNames[frameIndex]);

		const ImGuiWindow* const currentFrameWindow{ ImGui::FindWindowByName(g_FrameIndexWindowNames[frameIndex]) };
		if (!currentFrameWindow->SkipItems)
		{
			const ImVec2 cachedPosition{ ImGui::GetCursorPos() };
			ImGui::Dummy({ ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y + ImGui::GetScrollY() });
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("File"))
				{
					try
					{
						const std::filesystem::path filepath{ reinterpret_cast<const char*>(payload->Data) };

						if (std::filesystem::exists(filepath))
						{
							const size_t fileSize{ std::filesystem::file_size(filepath) };
							if ((fileSize / 1000U / 1000U) > 100U)
							{
								if (Platform::PushConfirmationWindow("Import file", "The file has a size of greater than 100MB and is considered a large file. Importing the file will probably slow down the use of the program significantly. Proceed anyway?"))
								{
									f_DisplaySequenceImportPopup[frameIndex] = true;
									f_ImportPath = filepath;
								}
							}
							else
							{
								f_DisplaySequenceImportPopup[frameIndex] = true;
								f_ImportPath = filepath;
							}
						}
					}
					catch (...)
					{
						HandleExceptions();
					}
				}

				ImGui::EndDragDropTarget();
			}

			ImGui::SetCursorPos(cachedPosition);

			GUI::SearchBar(m_SearchFilter, true, "Search " ICON_FA_SEARCH);
			std::transform(m_SearchFilter.begin(), m_SearchFilter.end(), m_SearchFilter.begin(), [](const char character)
			{
				return static_cast<char>(std::toupper(static_cast<int>(character)));
			});

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.0f, 2.0f });
			ImGui::Columns(2, nullptr, false);
			ImGui::SetColumnWidth(0, 190.0f);
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Sequence");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Type");
			
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 14.0f, 2.0f });
			ImGui::Columns(2, nullptr, true);
			ImGui::SetColumnWidth(0, 190.0f);
			ImGui::Unindent();

			for (const auto& [sequenceUUID, sequenceMetadata] : Project::IterateSequences())
			{
				std::visit(overloaded
				{
					[this, sequenceUUID, frameIndex](const DnaMetadata& dnaMetadata)
					{
						const auto& candidates{ dnaMetadata.Frames[frameIndex].ProteinCandidates };
						DrawSequence(sequenceUUID, dnaMetadata.SequenceName, ICON_FA_DNA, "DNA", ICON_FA_LINK, candidates, frameIndex);
					},
					[this, sequenceUUID, frameIndex](const RnaMetadata& rnaMetadata)
					{
						const auto& candidates{ rnaMetadata.Frames[frameIndex].ProteinCandidates };
						DrawSequence(sequenceUUID, rnaMetadata.SequenceName, ICON_FA_VIRUSES, "RNA", ICON_FA_LINK, candidates, frameIndex);
					},
					[this, sequenceUUID, frameIndex](const AminoMetadata& aminoMetadata)
					{
						DrawPeptideSequence(sequenceUUID, aminoMetadata.SequenceName, "\xef\x8c\x9c", "Peptide", ICON_FA_LINK, aminoMetadata.AminoSequence);
					},
					[](auto arg) { (void)arg; BIO_ASSERT(false); }
				},
				sequenceMetadata);
			}

			ImGui::PushItemWidth(-1);
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal | ImGuiSeparatorFlags_SpanAllColumns);
			ImGui::PopStyleVar(2);
		}
		else
		{
			ImGui::PushItemWidth(-1);
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal | ImGuiSeparatorFlags_SpanAllColumns);
		}

		if (ImGui::IsWindowHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			Project::InvalidateSelectionContext(Project::ESequenceSelectionType::None);
		
		if (ImGui::BeginPopupContextWindow("##CreateSequencePopup", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("New DNA Sequence"))
				f_DisplayDNACreation[frameIndex] = true;
			
			if (ImGui::MenuItem("New RNA Sequence"))
				f_DisplayRNACreation[frameIndex] = true;
		
			if (ImGui::MenuItem("New Amino Sequence"))
				f_DisplayAminoCreation[frameIndex] = true;
		
			ImGui::EndPopup();
		}

		BIO_UNLIKELY
		if (f_DisplayDNACreation[frameIndex])
		{
			constexpr const char* popupID{ "Create DNA Sequence" };
			ImGui::OpenPopup(popupID);
			DisplaySequenceTypeCreationPopup<Bio::DnaSequence>(popupID, f_DisplayDNACreation[frameIndex]);
		}
		
		BIO_UNLIKELY
		if (f_DisplayRNACreation[frameIndex])
		{
			constexpr const char* popupID{ "Create RNA Sequence" };
			ImGui::OpenPopup(popupID);
			DisplaySequenceTypeCreationPopup<Bio::RnaSequence>(popupID, f_DisplayRNACreation[frameIndex]);
		}
		
		BIO_UNLIKELY
		if (f_DisplayAminoCreation[frameIndex])
		{
			constexpr const char* popupID{ "Create Amino Sequence" };
			ImGui::OpenPopup(popupID);
			DisplaySequenceTypeCreationPopup<Bio::AminoSequence>(popupID, f_DisplayAminoCreation[frameIndex]);
		}

		BIO_UNLIKELY
		if (f_DisplaySequenceImportPopup[frameIndex])
		{
			constexpr const char* popupID{ "Import File" };
			ImGui::OpenPopup(popupID);

			BIO_UNLIKELY
			if(!f_ImportPath.empty())
				DisplaySequenceImportPopup(popupID, f_DisplaySequenceImportPopup[frameIndex], f_ImportPath);
			else
				DisplaySequenceImportPopup(popupID, f_DisplaySequenceImportPopup[frameIndex]);

			f_ImportPath.clear();
		}

		ImGui::End();
		for (const ID uuid : m_ToRemove)
		{
			Project::UnregisterSequence(uuid);
			Project::InvalidateSelectionContext(Project::ESequenceSelectionType::None);
			Project::ResetCache();
		}

		m_ToRemove.clear();
	}

	if (ImGui::IsKeyPressed(ImGuiKey_Escape))
	{
		if(Project::SelectedPeptide())
			Project::InvalidateSelectionContext(Project::SelectedSequenceType(), Project::SelectedSequence(), Project::SelectedFrame());
		else
			Project::InvalidateSelectionContext(Project::ESequenceSelectionType::None);
	}

	ImGui::PopStyleColor();
}

void ContentPanel::DrawSequence(
	const ID sequenceUUID, 
	const std::string_view sequenceName, 
	const std::string_view sequenceLabel, 
	const std::string_view tooltip,
	const std::string_view sequenceChildrenLabel, 
	const std::vector<Bio::AminoSequence>& candidates,
	const ID frameIndex)
{
	const bool isSequenceFrameSelected
	{
		Project::SelectedSequence() == sequenceUUID &&
		Project::SelectedFrame()	== frameIndex
	};

	const ImGuiTreeNodeFlags sequenceTreeNode
	{
		ImGuiTreeNodeFlags_SpanAvailWidth							|
		ImGuiTreeNodeFlags_OpenOnArrow								|
		ImGuiTreeNodeFlags_OpenOnDoubleClick						|
		(candidates.empty()) * ImGuiTreeNodeFlags_Bullet			|
		(!m_SearchFilter.empty()) * ImGuiTreeNodeFlags_DefaultOpen
	};

	ImGui::PushStyleColor(ImGuiCol_Text, { 0.65f, 0.65f, 0.65f, 1.0f });

	BIO_UNLIKELY
	if(isSequenceFrameSelected)
		ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f });
	
	ImGui::AlignTextToFramePadding();
	const bool openedSequenceTreeNode{ ImGui::TreeNodeEx(reinterpret_cast<void*>(sequenceUUID), sequenceTreeNode, sequenceName.data()) };

	BIO_UNLIKELY
	if (ImGui::IsItemHovered())
	{
		BIO_UNLIKELY
		if (ImGui::CalcTextSize(sequenceName.data()).x > 170.0f)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f });
			ImGui::BeginTooltip();
			ImGui::TextUnformatted(sequenceName.data());
			ImGui::EndTooltip();
			ImGui::PopStyleColor();
		}
	}

	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		if (Project::SelectedSequence() != sequenceUUID || Project::SelectedFrame() != frameIndex)
			Project::InvalidateSelectionContext(Project::ESequenceSelectionType::NucleotideSequence, sequenceUUID, frameIndex);

	if (sequenceUUID == Project::SelectedSequence())
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Remove sequence"))
				m_ToRemove.emplace_back(sequenceUUID);

			ImGui::EndPopup();
		}
	}

	ImGui::NextColumn();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal | ImGuiSeparatorFlags_SpanAllColumns);

	ImGui::Text(sequenceLabel.data());
	BIO_UNLIKELY
	if (isSequenceFrameSelected)
		ImGui::PopStyleColor();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 3.0f, 3.0f });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
	
	BIO_UNLIKELY
	if (ImGui::IsItemHovered())
	{
		ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f });
		ImGui::BeginTooltip();
		ImGui::TextUnformatted(tooltip.data());
		ImGui::EndTooltip();
		ImGui::PopStyleColor();
	}

	ImGui::PopStyleVar(2);
	if (candidates.empty())
	{
		if (openedSequenceTreeNode)
			ImGui::TreePop();

		ImGui::PopStyleColor();
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		return;
	}

	if (openedSequenceTreeNode)
	{
		std::vector<std::string> candidatesString;
		for (const auto& proteinCandidate : candidates)
		{
			auto candidate{ std::string{} };
			candidate.reserve(proteinCandidate.size() > 12U ? 20U : proteinCandidate.size() + 8U);

			candidate += '(' + std::to_string(proteinCandidate.size()) + ") ";
			for (size_t i{ 0U }; i < (proteinCandidate.size() > 12U ? 12U : proteinCandidate.size()); ++i)
				candidate += proteinCandidate[i].AsCharacter();

			if (proteinCandidate.size() > 12U)
				candidate += "...";

			candidatesString.emplace_back(std::move(candidate));
		}

		for (ID peptideIndex{ 0U }; peptideIndex < candidatesString.size(); ++peptideIndex)
		{
			const auto& candidate{ candidatesString[peptideIndex] };
			if (!m_SearchFilter.empty() && (candidate.find(m_SearchFilter) == std::string::npos))
				continue;

			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			
			const bool isCurrentCandidateSelected
			{
				isSequenceFrameSelected	&&
				Project::SelectedPeptide() == peptideIndex
			};

			BIO_UNLIKELY
			if (isCurrentCandidateSelected)
				ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f });
			
			const ImGuiTreeNodeFlags flags
			{ 
				ImGuiTreeNodeFlags_Leaf		|
				(m_SearchFilter.empty()) * ImGuiTreeNodeFlags_DefaultOpen
			};

			ImGui::AlignTextToFramePadding();
			if(ImGui::TreeNodeEx(reinterpret_cast<const void*>(candidate.data()), flags, candidate.data()))
				ImGui::TreePop();
			
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				Project::InvalidateSelectionContext(Project::ESequenceSelectionType::NucleotideSequence, sequenceUUID, frameIndex, peptideIndex);

			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			ImGui::Text(sequenceChildrenLabel.data());

			BIO_UNLIKELY
			if (isCurrentCandidateSelected)
				ImGui::PopStyleColor();
		}
	}

	ImGui::PopStyleColor();
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);
	
	if (openedSequenceTreeNode)
		ImGui::TreePop();
}

void ContentPanel::DrawPeptideSequence(
	const ID sequenceUUID, 
	const std::string_view sequenceName, 
	const std::string_view sequenceLabel,
	const std::string_view tooltip,
	const std::string_view sequenceChildrenLabel,
	const Bio::AminoSequence& peptide)
{
	if (!m_SearchFilter.empty() && (sequenceName.find(m_SearchFilter) == std::string::npos))
		return;

	const bool isSequenceFrameSelected
	{
		Project::SelectedSequence() == sequenceUUID
	};

	const ImGuiTreeNodeFlags sequenceTreeNode
	{
		ImGuiTreeNodeFlags_SpanAvailWidth |
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		(!m_SearchFilter.empty()) * ImGuiTreeNodeFlags_DefaultOpen
	};

	ImGui::PushStyleColor(ImGuiCol_Text, { 0.65f, 0.65f, 0.65f, 1.0f });

	BIO_UNLIKELY
	if (isSequenceFrameSelected)
		ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f });

	ImGui::AlignTextToFramePadding();
	const bool openedSequenceTreeNode{ ImGui::TreeNodeEx(reinterpret_cast<void*>(sequenceUUID), sequenceTreeNode, sequenceName.data()) };

	BIO_UNLIKELY
	if (ImGui::IsItemHovered())
	{
		BIO_UNLIKELY
		if (ImGui::CalcTextSize(sequenceName.data()).x > 170.0f)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f });
			ImGui::BeginTooltip();
			ImGui::TextUnformatted(sequenceName.data());
			ImGui::EndTooltip();
			ImGui::PopStyleColor();
		}
	}

	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		if (Project::SelectedSequence() != sequenceUUID)
			Project::InvalidateSelectionContext(Project::ESequenceSelectionType::AminoSequence, sequenceUUID);

	if (sequenceUUID == Project::SelectedSequence())
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Remove sequence"))
				m_ToRemove.emplace_back(sequenceUUID);

			ImGui::EndPopup();
		}
	}

	ImGui::NextColumn();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal | ImGuiSeparatorFlags_SpanAllColumns);

	ImGui::Text(sequenceLabel.data());
	BIO_UNLIKELY
	if (isSequenceFrameSelected)
		ImGui::PopStyleColor();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 3.0f, 3.0f });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);

	BIO_UNLIKELY
	if (ImGui::IsItemHovered())
	{
		ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f });
		ImGui::BeginTooltip();
		ImGui::TextUnformatted(tooltip.data());
		ImGui::EndTooltip();
		ImGui::PopStyleColor();
	}

	ImGui::PopStyleVar(2);
	if (openedSequenceTreeNode)
	{
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		std::string peptideString{ Bio::ConvertToString(peptide) };
		if (peptideString.empty())
			peptideString = "Empty";

		if (peptideString.size() > 12U)
			peptideString = peptideString.substr(0U, 12U) + "...";

		if (isSequenceFrameSelected)
			ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f });

		const ImGuiTreeNodeFlags flags
		{ 
			ImGuiTreeNodeFlags_Leaf			|
			ImGuiTreeNodeFlags_DefaultOpen
		};

		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNodeEx(reinterpret_cast<const void*>(&sequenceUUID), flags, peptideString.c_str()))
		{
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
					Project::InvalidateSelectionContext(Project::ESequenceSelectionType::AminoSequence, sequenceUUID);

			ImGui::TreePop();
		}

		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		ImGui::Text(sequenceChildrenLabel.data());

		if (isSequenceFrameSelected)
			ImGui::PopStyleColor();
	}

	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	if (openedSequenceTreeNode)
		ImGui::TreePop();

	ImGui::PopStyleColor();
}