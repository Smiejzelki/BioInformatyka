#include "EntryPoint.hpp"

class Project;
class PanelBase;

class Wizualizator final : public Application
{
private:
	NON_COPYABLE(Wizualizator)
public:
	explicit Wizualizator(CommandLineArguments&& arguments);
	virtual ~Wizualizator() noexcept;
	
	virtual void OnGUIRender() final override;
	virtual void OnEvent(const Event& event) final override;
private:
	void OpenProject(const std::filesystem::path& path);
	void SaveAs(const std::filesystem::path& path);
	void SetWindowAppendix(const std::string& appendix);
private:
	std::vector<PanelBase*> m_Panels;
	std::filesystem::path m_CurrentProjectDirectory;

	static constexpr std::string_view VanillaWindowName{ "Visualizer" };
	static constexpr std::string_view VisualizerProjectFilter{ "Visualizer Project (*.vis)\0*.vis\0" };
	static constexpr std::string_view FastaFormatFilter
	{ 
		"FASTA file" 
		"(*.fasta)\0*.fasta\0" 
		"(*.fna)\0*.fna\0" 
		"(*.faa)\0*.faa\0" 
		"(*.frn)\0*.frn\0" 
		"(*.fa)\0*.fa\0" 
		"(*.txt)\0*.txt\0"
	};
};