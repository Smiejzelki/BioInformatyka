#include "Panels/ProjectPanel.hpp"
#include "Canvas.hpp"
#include "Camera.hpp"
#include "Event.hpp"
#include "GUI.hpp"
#include "Platform.hpp"

#include "implot.h"
#include "imgui_internal.h"
#include <Psapi.h>

ProjectPanel::ProjectPanel() noexcept
	:
	PanelBase("Project Panel")
{}

void ProjectPanel::OnGUIRender()
{
	const ImGuiWindowFlags windowFlags
	{
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		(Project::IsUnsaved() * ImGuiWindowFlags_UnsavedDocument) |
		ImGuiWindowFlags_None
	};

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
	ImGui::Begin("Project        ", nullptr, windowFlags);

	ImGui::BeginChild("Project File Explorer", { ImGui::GetWindowWidth() * 0.33f, ImGui::GetWindowHeight() }, true);
	GUI::TextCentered("File tree");

	const std::filesystem::path currentWorkingPath{ Project::GetWorkingPath() };
	if (!currentWorkingPath.empty() && std::filesystem::exists(currentWorkingPath))
	{
		DrawDirectoryFromRoot(currentWorkingPath, "", [this](const std::filesystem::path& assetPath)
		{
			std::string extension{ assetPath.extension().string() };
			std::transform(extension.begin(), extension.end(), extension.begin(), [](const char character) 
			{ 
				return static_cast<char>(std::tolower(character)); 
			});

			if (extension == ".fasta" || extension == ".txt")
			{
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					const std::string assetPathString{ assetPath.string() };
					ImGui::SetDragDropPayload("File", assetPathString.c_str(), (assetPathString.size() + 1U) * sizeof(const char));
					ImGui::EndDragDropSource();
				}
			}
		});
	}
	else
	{
		ImGui::NewLine();
		GUI::TextCentered("No project selected");
	}

	ImGui::EndChild();
	ImGui::SameLine();
	GUI::VerticalSeparator();

	ImGui::SameLine();
	ImGui::BeginChild("Project IO##IO", { ImGui::GetWindowWidth() * 0.67f, ImGui::GetWindowHeight() }, true);
	GUI::TextCentered("Sequence View");
		
	const bool isAminoSequence{ Project::SelectedSequenceType() == Project::ESequenceSelectionType::AminoSequence || (Project::SelectedSequenceType() == Project::ESequenceSelectionType::NucleotideSequence && Project::SelectedPeptide() )};
	ImGui::BeginDisabled(!isAminoSequence);

	static bool f_UseThreeLetterCode{ false };
	ImGui::Checkbox("Use 3-letter code", &f_UseThreeLetterCode);
	ImGui::EndDisabled();
	
	ImGui::SameLine();
	const bool copyToClipBoard{ ImGui::Button("Copy to clipboard") };
	
	{
		ImGui::SetNextWindowBgAlpha(0.2f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 5.0f, 5.0f });
		ImGui::BeginChild("TextWrapperChild", { ImGui::GetContentRegionAvail().x - 22.0f, ImGui::GetContentRegionAvail().y - 5.0f }, true);

		const auto manageSequence
		{
			[](const std::string_view sequence, const bool copyToClipboard)
			{
				ImGui::TextWrapped(sequence.data()); 
				
				BIO_UNLIKELY
				if (copyToClipboard)
					Platform::CopyTextToClipboard(sequence);
			}
		};

		if(!Project::OnSequenceSelected
			(
				[this, &manageSequence, copyToClipBoard](const Project::NucleotideSequenceCache& nucleotideSequenceCache) 
				{ 
					manageSequence(nucleotideSequenceCache.NucleotideSequence, copyToClipBoard); 
				},

				[this, &manageSequence, copyToClipBoard](const Project::NucleotideSequencePeptideCache& nucleotideSequencePeptideCache)	
				{ 
					manageSequence(f_UseThreeLetterCode ? nucleotideSequencePeptideCache.AminoSequenceThreeLetterCode : nucleotideSequencePeptideCache.ProteinCandidate, copyToClipBoard);
				},

				[this, &manageSequence, copyToClipBoard](const Project::AminoSequenceCache& aminoSequenceCache)
				{ 
					manageSequence(f_UseThreeLetterCode ? aminoSequenceCache.AminoSequenceThreeLetterCode : aminoSequenceCache.AminoSequence, copyToClipBoard);
				},

				[this, &manageSequence, copyToClipBoard](const Project::AminoSequencePeptideCache& aminoSequencePeptideCache)
				{ 
					manageSequence(f_UseThreeLetterCode ? aminoSequencePeptideCache.AminoSequenceThreeLetterCode : aminoSequencePeptideCache.ProteinCandidate, copyToClipBoard);
				}
			)
		)
		{
			ImGui::TextWrapped("");
		}

		ImGui::EndChild();
		ImGui::PopStyleVar();
	}

	ImGui::EndChild();
	ImGui::End();
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor();
}

void ProjectPanel::DrawDirectoryFromRoot(const std::filesystem::path& rootPath, const std::string_view filter, const AssetCallbackFunction& function)
{
	constexpr ImGuiTreeNodeFlags directoryFlags
	{
		ImGuiTreeNodeFlags_SpanAvailWidth |
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_DefaultOpen |
		0U
	};

	std::string stringRootPath = std::string(ICON_FA_FOLDER) + " ";
	stringRootPath += rootPath.stem().string() + "##ContentBrowserRoot";

	if (ImGui::TreeNodeEx(stringRootPath.c_str(), directoryFlags))
	{
		DrawDirectoriesRecursively(rootPath, filter, function);
		ImGui::TreePop();
	}
}

void ProjectPanel::DrawDirectoriesRecursively(const std::filesystem::path& directoryPath, const std::string_view filter, const AssetCallbackFunction& function)
{
	for (const auto& directoryEntry : std::filesystem::directory_iterator(directoryPath))
	{
		const std::filesystem::path& filepathEntry = directoryEntry.path();
		const bool isDirectory = directoryEntry.is_directory();

		std::string filenameString = isDirectory ? std::string{ ICON_FA_FOLDER } : std::string{ ICON_FA_FILE };
		filenameString += " " + filepathEntry.filename().string();

		constexpr bool passesFilter = true; // TODO: Add filtering?
		if (isDirectory)
		{
			if (passesFilter)
			{
				if (filter.empty())
				{
					constexpr ImGuiTreeNodeFlags directoryFlags{ ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow };
					if (ImGui::TreeNodeEx(filenameString.c_str(), directoryFlags))
					{
						DrawDirectoriesRecursively(filepathEntry, "", function);
						ImGui::TreePop();
					}
				}
				else
				{
					constexpr ImGuiTreeNodeFlags directoryFlags{ ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen };
					if (ImGui::TreeNodeEx(filenameString.c_str(), directoryFlags))
					{
						DrawDirectoriesRecursively(filepathEntry, "", function);
						ImGui::TreePop();
					}
				}
			}
			else
			{
				DrawDirectoriesRecursively(filepathEntry, filter, function);
			}
		}
		else
		{
			if (passesFilter)
			{
				ImGuiTreeNodeFlags resourceFlags =
					ImGuiTreeNodeFlags_SpanAvailWidth |
					ImGuiTreeNodeFlags_OpenOnArrow |
					ImGuiTreeNodeFlags_Leaf;

				if (!filter.empty())
					resourceFlags |= ImGuiTreeNodeFlags_Selected;

				if (ImGui::TreeNodeEx(filenameString.c_str(), resourceFlags))
					ImGui::TreePop();

				function(filepathEntry);
			}
		}
	}
}