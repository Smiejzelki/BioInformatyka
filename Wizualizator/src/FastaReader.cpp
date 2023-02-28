#include "FastaReader.hpp"

FastaReader::FastaReader(const std::filesystem::path& path)
	:
	m_File(path)
{
	BIO_UNLIKELY
	if (!m_File.is_open())
		THROW_EXCEPTION("Failed to open file");
}

FastaReader::~FastaReader() noexcept
{
	BIO_LIKELY
	if(m_File.is_open())
		m_File.close();
}

std::vector<std::pair<std::string, std::string>> FastaReader::ReadNucleotideSequences() const
{
	if (!m_File.is_open())
		THROW_EXCEPTION("File is not valid");

	m_File.seekg(0U, std::ios::beg);
	std::vector<std::pair<std::string, std::string>> result;
	std::string currentLine, sequenceName, sequenceContent;
	while (std::getline(m_File, currentLine))
	{
		if (currentLine.empty() || currentLine.front() == s_Identifier)
		{
			if (!sequenceName.empty())
			{
				result.push_back({ sequenceName, sequenceContent });
				sequenceName.clear();
			}

			if (!currentLine.empty())
				sequenceName = currentLine.substr(1U);

			sequenceContent.clear();
		}
		else if (!sequenceName.empty())
		{
			BIO_UNLIKELY
			if (currentLine.find(' ') != std::string::npos)
				THROW_EXCEPTION("Invalid format - no spaces allowed");
			else
				sequenceContent += currentLine;
		}
	}

	if (!sequenceName.empty() && !sequenceContent.empty())
		result.push_back({ sequenceName, sequenceContent });

	return result;
}