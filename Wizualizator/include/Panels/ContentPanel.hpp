#pragma once
#include "PanelBase.hpp"

class ContentPanel final : public PanelBase
{
public:
	explicit ContentPanel() noexcept;
	virtual ~ContentPanel() noexcept = default;

	virtual void OnGUIRender() override final;
private:
	void DrawSequence(
		const ID sequenceUUID,
		const std::string_view sequenceName,
		const std::string_view sequenceLabel,
		const std::string_view tooltip,
		const std::string_view sequenceChildrenLabel,
		const std::vector<Bio::AminoSequence>& candidates,
		const ID frameIndex);
	
	void DrawPeptideSequence(
		const ID sequenceUUID,
		const std::string_view sequenceName,
		const std::string_view sequenceLabel,
		const std::string_view tooltip,
		const std::string_view sequenceChildrenLabel,
		const Bio::AminoSequence& peptide);
private:
	std::string m_SearchFilter;
	std::vector<ID> m_ToRemove;
};