#include "Wizualizator.hpp"
#include "Project.hpp"
#include "Panels/PanelBase.hpp"
#include "Panels/ProjectPanel.hpp"
#include "Panels/ContentPanel.hpp"
#include "Panels/PropertiesPanel.hpp"
#include "Panels/PlotPanel.hpp"
#include "Panels/StructurePanel.hpp"
#include "FastaReader.hpp"

#include "imgui.h"
#include "imgui_internal.h"
/* Core includes */
#include "GUI.hpp"
#include "Platform.hpp"
#include "Window.hpp"

Wizualizator::Wizualizator(CommandLineArguments&& arguments)
	:
	Application(std::move(arguments), VanillaWindowName, 1280U, 960U, true),
	m_Panels{ new ProjectPanel(), new ContentPanel(), new PropertiesPanel(), new PlotPanel(), new StructurePanel() },
	m_CurrentProjectDirectory{}
{
	Project::Create();
	Project::SetPathCallback
	(
		[this]() -> std::filesystem::path 
		{
			return m_CurrentProjectDirectory;
		}
	);
}

Wizualizator::~Wizualizator() noexcept
{
	for (PanelBase* const panel : m_Panels)
		delete panel;

	m_Panels.clear();
	Project::Reset();
}

void Wizualizator::OnGUIRender()
{
	static bool f_OpenAboutPopup{ false };

	BIO_UNLIKELY
	if (ImGui::BeginMenuBar())
	{
		BIO_UNLIKELY
		if (ImGui::BeginMenu("File##file"))
		{
			if (ImGui::MenuItem("New"))
			{
				if (!m_CurrentProjectDirectory.empty() || Project::IsUnsaved())
				{
					if (Platform::PushConfirmationWindow("New Project", "You are about to create a new project. Any unsaved changes will be lost. Proceed?"))
					{
						Project::Reset();
						SetWindowAppendix({});
					}

					m_CurrentProjectDirectory.clear();
				}
				else
				{
					Project::Reset();
					SetWindowAppendix({});
				}
			}

			if (ImGui::MenuItem("Open"))
			{
				const std::optional<std::filesystem::path> openedFile{ Platform::OpenFile(VisualizerProjectFilter) };

				BIO_LIKELY
				if (openedFile.has_value())
					OpenProject(openedFile.value());
			}

			if (ImGui::MenuItem("Save"))
			{
				if (!m_CurrentProjectDirectory.empty() && std::filesystem::exists(m_CurrentProjectDirectory))
					SaveAs(m_CurrentProjectDirectory);
				else
				{
					const std::optional<std::filesystem::path> openedFile{ Platform::SaveFile(VisualizerProjectFilter) };

					BIO_LIKELY
					if (openedFile.has_value())
						SaveAs(openedFile.value());
				}
			}

			if (ImGui::MenuItem("Save as"))
			{
				const std::optional<std::filesystem::path> openedFile{ Platform::SaveFile(VisualizerProjectFilter) };

				BIO_LIKELY
				if (openedFile.has_value())
					SaveAs(openedFile.value());
			}

			GUI::HorizontalSeparator();
			BIO_UNLIKELY
			if (ImGui::MenuItem("Quit"))
				m_IsRunning = false;

			ImGui::EndMenu();
		}

		BIO_UNLIKELY
		if(ImGui::BeginMenu("Project##proj"))
		{ 
			if (ImGui::MenuItem("Import DNA"))
			{
				const std::optional<std::filesystem::path> openedFile{ Platform::OpenFile(FastaFormatFilter) };

				BIO_LIKELY
				if (openedFile.has_value())
				{
					try
					{
						const size_t fileSize{ std::filesystem::file_size(openedFile.value()) };
						if ((fileSize / 1000U / 1000U) > 100U)
							if (!Platform::PushConfirmationWindow("Import file", "The file has a size of greater than 100MB and is considered a large file. Importing the file will probably slow down the use of the program significantly. Proceed anyway?"))
								goto breakOfImporting;
						
						const FastaReader reader(openedFile.value());
						const auto parsedDNA{ reader.ReadNucleotideSequences() };
						
						BIO_LIKELY
						if (Project::Get())
						{
							for (const auto& [sequenceName, sequenceContent] : parsedDNA)
							{
								auto& ref{ Project::Get()->RegisterSequence<DnaMetadata>(sequenceName) };
								auto metadata{ DnaMetadata::Create(sequenceName, sequenceContent) };
								ref = std::move(metadata);
							}
						}
						else
							LOG("Cant open a file in an invalid project");
					}
					catch (...)
					{
						HandleExceptions();
					}
				}
			}

			if (ImGui::MenuItem("Import RNA"))
			{
				const std::optional<std::filesystem::path> openedFile{ Platform::OpenFile(FastaFormatFilter) };

				BIO_LIKELY
				if (openedFile.has_value())
				{
					try
					{
						const size_t fileSize{ std::filesystem::file_size(openedFile.value()) };
						if ((fileSize / 1000U / 1000U) > 100U)
							if (!Platform::PushConfirmationWindow("Import file", "The file has a size of greater than 100MB and is considered a large file. Importing the file will probably slow down the use of the program significantly. Proceed anyway?"))
								goto breakOfImporting;

						const FastaReader reader(openedFile.value());
						const auto parsedRNA{ reader.ReadNucleotideSequences() };

						BIO_LIKELY
						if (Project::Get())
						{
							for (const auto& [sequenceName, sequenceContent] : parsedRNA)
							{
								auto& ref{ Project::Get()->RegisterSequence<RnaMetadata>(sequenceName) };
								auto metadata{ RnaMetadata::Create(sequenceName, sequenceContent) };
								ref = std::move(metadata);
							}
						}
						else
							LOG("Cant open a file in an invalid project");
					}
					catch (...)
					{
						HandleExceptions();
					}
				}
			}

			breakOfImporting:
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			for (const PanelBase* const panel : m_Panels)
			{
				const std::string_view panelName{ panel->GetPanelName() };
				bool isEnabled{ panel->IsEnabled() };

				if (ImGui::Checkbox(panelName.data(), &isEnabled))
					panel->SetEnabled(isEnabled);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Open manual"))
				Platform::Execute(L"open", L"Dokumentacja\\instrukcja.html");
			
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("About"))
		{
			f_OpenAboutPopup = true;
			ImGui::EndMenu();
		}
		
		ImGui::EndMenuBar();
	}

	for (PanelBase* const panel : m_Panels)
		BIO_LIKELY
		if(panel->IsEnabled())
			panel->OnGUIRender();

	BIO_UNLIKELY
	if (f_OpenAboutPopup)
	{
		constexpr const char* g_AboutPopupID{ "About##popup" };
		ImGui::OpenPopup(g_AboutPopupID);

		ImGuiViewport* mainViewport = ImGui::GetMainViewport();
		const float popupWidth = mainViewport->Size.x * 0.5f;
		const float popupHeight = mainViewport->Size.y * 0.25f;

		ImGui::SetNextWindowPos({ mainViewport->Pos.x + popupWidth * 0.5f, mainViewport->Pos.y + popupHeight * 0.5f });
		ImGui::SetNextWindowSize({ popupWidth, popupHeight });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);

		if
		(
			ImGui::BeginPopupModal
			(
				g_AboutPopupID,
				nullptr,
				ImGuiWindowFlags_AlwaysAutoResize	|
				ImGuiWindowFlags_NoMove				|
				ImGuiWindowFlags_NoNavInputs		|
				0U
			)
		)
		{
			GUI::TextCentered("Autorzy:");
			ImGui::NewLine();

			GUI::TextCentered("Oliver Slawinski");
			ImGui::NewLine();

			GUI::TextCentered("Martin Slawinski");
			ImGui::NewLine();

			GUI::TextCentered("Leon Luczyn");
			ImGui::NewLine();

			GUI::TextCentered("Oskar Podkowa");
			ImGui::NewLine();

			GUI::TextCentered("Karol Kasperek");
			ImGui::NewLine();

			if (GUI::ButtonCentered("Close"))
				f_OpenAboutPopup = false;

			ImGui::EndPopup();
		}

		ImGui::PopStyleVar();
	}
}

void Wizualizator::OnEvent(const Event& event)
{
	for (PanelBase* const panel : m_Panels)
		BIO_LIKELY
		if (panel->IsEnabled())
			panel->OnEvent(event);
}

void Wizualizator::OpenProject(const std::filesystem::path& path)
{
	BIO_ASSERT(std::filesystem::exists(path));

	try
	{
		Project::Reset();
		ProjectSerializer serializer(Project::Get());
		serializer.OnDeserialize(path);
		
		m_CurrentProjectDirectory = path.parent_path();
		Project::SetSaveStatus(true);
		SetWindowAppendix(path.filename().string());
	}
	catch (...)
	{
		HandleExceptions();

		const std::string stringifiedPath{ path.string() };
		fprintf(stdout, "Failed to open file with path %s\n", stringifiedPath.c_str());
	}
}

void Wizualizator::SaveAs(const std::filesystem::path& path)
{
	try
	{
		ProjectSerializer serializer(Project::Get());
		serializer.OnSerialize(path);

		m_CurrentProjectDirectory = path.parent_path();
		Project::SetSaveStatus(true);
		SetWindowAppendix(path.filename().string());
	}
	catch (...)
	{
		HandleExceptions();

		const std::string pathString{ path.string() };
		fprintf(stdout, "Failed to open file with path %s\n", pathString.c_str());
	}
}

void Wizualizator::SetWindowAppendix(const std::string& appendix)
{
	if (appendix.empty())
		m_Window->SetName(VanillaWindowName);
	else
		m_Window->SetName(std::string(VanillaWindowName) + " - " + appendix);
}

REGISTER_APPLICATION(Wizualizator)