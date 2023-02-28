#pragma once
#include "Core.hpp"

class FastaReader
{
private:
public:
	explicit FastaReader(const std::filesystem::path& path);
	~FastaReader() noexcept;

	/* Sequence name -> sequence content */
	[[nodiscard]] std::vector<std::pair<std::string, std::string>> ReadNucleotideSequences() const;
private:
	mutable std::ifstream m_File;

	static constexpr char s_Identifier{ '>' };
};