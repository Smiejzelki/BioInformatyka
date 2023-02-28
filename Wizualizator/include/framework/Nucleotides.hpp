#pragma once
#include "Dna.hpp"
#include "DnaX.hpp"
#include "Rna.hpp"
#include "RnaX.hpp"
#include "Amino.hpp"
#include "AminoX.hpp"

namespace Bio {
	using DnaSequence = std::vector<Dna>;
	using DnaXSequence = std::vector<DnaX>;

	using RnaSequence = std::vector<Rna>;
	using RnaXSequence = std::vector<RnaX>;

	using AminoSequence = std::vector<AminoAcid>;
	using AminoXSequence = std::vector<AminoAcidX>;

	template<typename Type>
	constexpr std::string ConvertToString(const std::vector<Type>& sequence)
	{
		std::string result;
		result.resize(sequence.size());
		
		for (size_t i{ 0U }; i < sequence.size(); ++i)
			result[i] = sequence[i].AsCharacter();

		return result;
	}

	template<typename Type>
	constexpr DnaXSequence ConvertToDNAX(const Type& type)
	{
		DnaXSequence result;
		std::for_each(std::begin(type), std::end(type), [&result](const char character)
		{
			result.emplace_back(DnaX{}.AssignCharacter(character));
		});

		return result;
	}

	template<typename Type>
	constexpr DnaSequence ConvertToDNA(const Type& type)
	{
		DnaXSequence resultX{ ConvertToDNAX(type) };
		DnaSequence result;
		result.reserve(resultX.size());
		
		for (size_t i{ 0U }; i < resultX.size(); ++i)
		{
			const char asCharacter{ resultX[i].AsCharacter() };
			if (asCharacter != DnaX::Invalid)
				result.emplace_back(Dna{}.AssignCharacter(asCharacter));
		}

		result.shrink_to_fit();
		return result;
	}

	template<typename Type>
	constexpr DnaSequence ConvertToDNAReversed(const Type& type)
	{
		DnaXSequence resultX{ ConvertToDNAX(type) };
		DnaSequence result;
		result.reserve(resultX.size());

		for (size_t i{ resultX.size() - 1U }; i != 0U; --i)
		{
			const char asCharacter{ resultX[i].AsCharacter() };
			[[likely]]
			if (asCharacter != DnaX::Invalid)
				result.emplace_back(Dna{}.AssignCharacter(asCharacter).Complement());
		}

		result.shrink_to_fit();
		return result;
	}

	template<typename Type>
	constexpr RnaXSequence ConvertToRNAX(const Type& type)
	{
		RnaXSequence result;
		std::for_each(std::begin(type), std::end(type), [&result](const char character)
		{
			result.emplace_back(RnaX{}.AssignCharacter(character));
		});

		return result;
	}

	template<typename Type>
	constexpr RnaSequence ConvertToRNA(const Type& type)
	{
		RnaXSequence resultX{ ConvertToRNAX(type) };
		RnaSequence result;
		result.reserve(resultX.size());

		for (size_t i{ 0U }; i < resultX.size(); ++i)
		{
			const char asCharacter{ resultX[i].AsCharacter() };
			[[likely]]
			if (asCharacter != RnaX::Invalid)
				result.emplace_back(Rna{}.AssignCharacter(asCharacter));
		}

		result.shrink_to_fit();
		return result;
	}

	template<typename Type>
	constexpr AminoXSequence ConvertToAminoXSequence(const Type& type)
	{
		AminoXSequence result;
		std::for_each(std::begin(type), std::end(type), [&result](const char character)
			{
				result.emplace_back(AminoAcidX{}.AssignCharacter(character));
			});

		return result;
	}

	template<typename Type>
	constexpr AminoSequence ConvertToAminoSequence(const Type& type)
	{
		AminoXSequence resultX{ ConvertToAminoXSequence(type) };
		AminoSequence result;
		result.reserve(resultX.size());

		for (size_t i{ 0U }; i < resultX.size(); ++i)
		{
			const char asCharacter{ resultX[i].AsCharacter() };
			[[likely]]
			if (asCharacter != AminoAcidX::Invalid)
				result.emplace_back(AminoAcid{}.AssignCharacter(asCharacter));
		}

		result.shrink_to_fit();
		return result;
	}

	template<typename Nucleotide>
	AminoAcid TranslateTriplet(const Nucleotide first, const Nucleotide second, const Nucleotide third)
	{
		/* string view provides comparison operators while not owning the container */
		static const std::unordered_map<std::string_view, const char> f_AminoMap
		{
			std::make_pair("GUU", 'V'),
			std::make_pair("GUC", 'V'),
			std::make_pair("GUA", 'V'),
			std::make_pair("GUG", 'V'),

			std::make_pair("GCU", 'A'),
			std::make_pair("GCC", 'A'),
			std::make_pair("GCA", 'A'),
			std::make_pair("GCG", 'A'),

			std::make_pair("GAU", 'D'),
			std::make_pair("GAC", 'D'),

			std::make_pair("GAA", 'E'),
			std::make_pair("GAG", 'E'),

			std::make_pair("GGU", 'G'),
			std::make_pair("GGC", 'G'),
			std::make_pair("GGA", 'G'),
			std::make_pair("GGG", 'G'),

			/* ----------------------------------------- */

			std::make_pair("UUU", 'F'),
			std::make_pair("UUC", 'F'),

			std::make_pair("UUA", 'L'),
			std::make_pair("UUG", 'L'),

			std::make_pair("UCU", 'S'),
			std::make_pair("UCC", 'S'),
			std::make_pair("UCA", 'S'),
			std::make_pair("UCG", 'S'),

			std::make_pair("UAU", 'Y'),
			std::make_pair("UAC", 'Y'),

			std::make_pair("UAA", '-'),
			std::make_pair("UAG", '-'),

			std::make_pair("UGU", 'C'),
			std::make_pair("UGC", 'C'),

			std::make_pair("UGA", '-'),
			std::make_pair("UGG", 'W'),

			/* ----------------------------------------- */

			std::make_pair("CUU", 'L'),
			std::make_pair("CUC", 'L'),
			std::make_pair("CUA", 'L'),
			std::make_pair("CUG", 'L'),

			std::make_pair("CCU", 'P'),
			std::make_pair("CCC", 'P'),
			std::make_pair("CCA", 'P'),
			std::make_pair("CCG", 'P'),

			std::make_pair("CAU", 'H'),
			std::make_pair("CAC", 'H'),

			std::make_pair("CAA", 'Q'),
			std::make_pair("CAG", 'Q'),

			std::make_pair("CGU", 'R'),
			std::make_pair("CGC", 'R'),
			std::make_pair("CGA", 'R'),
			std::make_pair("CGG", 'R'),

			/* ----------------------------------------- */

			std::make_pair("AUU", 'I'),
			std::make_pair("AUC", 'I'),
			std::make_pair("AUA", 'I'),

			std::make_pair("AUG", 'M'),

			std::make_pair("ACU", 'T'),
			std::make_pair("ACC", 'T'),
			std::make_pair("ACA", 'T'),
			std::make_pair("ACG", 'T'),

			std::make_pair("AAU", 'N'),
			std::make_pair("AAC", 'N'),

			std::make_pair("AAA", 'K'),
			std::make_pair("AAG", 'K'),

			std::make_pair("AGU", 'S'),
			std::make_pair("AGC", 'S'),

			std::make_pair("AGA", 'R'),
			std::make_pair("AGG", 'R'),
			/* ===================================================== */
			/* Thymine can be used instead of uracil for translation */
			/* ===================================================== */
			std::make_pair("GTT", 'V'),
			std::make_pair("GTC", 'V'),
			std::make_pair("GTA", 'V'),
			std::make_pair("GTG", 'V'),

			std::make_pair("GCT", 'A'),
			std::make_pair("GCC", 'A'),
			std::make_pair("GCA", 'A'),
			std::make_pair("GCG", 'A'),

			std::make_pair("GAT", 'D'),
			std::make_pair("GAC", 'D'),

			std::make_pair("GAA", 'E'),
			std::make_pair("GAG", 'E'),

			std::make_pair("GGT", 'G'),
			std::make_pair("GGC", 'G'),
			std::make_pair("GGA", 'G'),
			std::make_pair("GGG", 'G'),

			/* ----------------------------------------- */

			std::make_pair("TTT", 'F'),
			std::make_pair("TTC", 'F'),

			std::make_pair("TTA", 'L'),
			std::make_pair("TTG", 'L'),

			std::make_pair("TCT", 'S'),
			std::make_pair("TCC", 'S'),
			std::make_pair("TCA", 'S'),
			std::make_pair("TCG", 'S'),

			std::make_pair("TAT", 'Y'),
			std::make_pair("TAC", 'Y'),

			std::make_pair("TAA", '-'),
			std::make_pair("TAG", '-'),

			std::make_pair("TGT", 'C'),
			std::make_pair("TGC", 'C'),

			std::make_pair("TGA", '-'),
			std::make_pair("TGG", 'W'),

			/* ----------------------------------------- */

			std::make_pair("CTT", 'L'),
			std::make_pair("CTC", 'L'),
			std::make_pair("CTA", 'L'),
			std::make_pair("CTG", 'L'),

			std::make_pair("CCT", 'P'),
			std::make_pair("CCC", 'P'),
			std::make_pair("CCA", 'P'),
			std::make_pair("CCG", 'P'),

			std::make_pair("CAT", 'H'),
			std::make_pair("CAC", 'H'),

			std::make_pair("CAA", 'Q'),
			std::make_pair("CAG", 'Q'),

			std::make_pair("CGT", 'R'),
			std::make_pair("CGC", 'R'),
			std::make_pair("CGA", 'R'),
			std::make_pair("CGG", 'R'),

			/* ----------------------------------------- */

			std::make_pair("ATT", 'I'),
			std::make_pair("ATC", 'I'),
			std::make_pair("ATA", 'I'),

			std::make_pair("ATG", 'M'),

			std::make_pair("ACT", 'T'),
			std::make_pair("ACC", 'T'),
			std::make_pair("ACA", 'T'),
			std::make_pair("ACG", 'T'),

			std::make_pair("AAT", 'N'),
			std::make_pair("AAC", 'N'),

			std::make_pair("AAA", 'K'),
			std::make_pair("AAG", 'K'),

			std::make_pair("AGT", 'S'),
			std::make_pair("AGC", 'S'),

			std::make_pair("AGA", 'R'),
			std::make_pair("AGG", 'R'),
		};

		const char key[4]
		{
			first.AsCharacter(),
			second.AsCharacter(),
			third.AsCharacter(),
			'\0'
		};

		assert(f_AminoMap.contains(key));
		return AminoAcid{}.AssignCharacter(f_AminoMap.at(key));
	}

	template<typename NucleotideSequence>
	constexpr AminoSequence TranslateNucleotideSequence(const NucleotideSequence& nucleotideSequence)
	{
		[[likely]]
		if (!nucleotideSequence.empty())
		{
			AminoSequence aminoSequence(nucleotideSequence.size() / 3U);

			uint32_t nucleotideSequenceOffset{ 0U };
			for (auto& amino : aminoSequence)
			{
				amino =
				TranslateTriplet
				(
					nucleotideSequence[nucleotideSequenceOffset + 0U],
					nucleotideSequence[nucleotideSequenceOffset + 1U],
					nucleotideSequence[nucleotideSequenceOffset + 2U]
				);

				nucleotideSequenceOffset += 3U;
			}

			return aminoSequence;
		}

		return AminoSequence{};
	}

	inline const char* AminoToThreeLetterCode(const AminoAcid amino) noexcept
	{
		switch (amino)
		{
			case EAminoAcid::STOP:  return "TER";
			case EAminoAcid::V:		return "VAL";
			case EAminoAcid::A:		return "ALA";
			case EAminoAcid::D:		return "ASP";
			case EAminoAcid::E:		return "GLU";
			case EAminoAcid::G:		return "GLY";
			case EAminoAcid::F:		return "PHE";
			case EAminoAcid::L:		return "LEU";
			case EAminoAcid::S:		return "SER";
			case EAminoAcid::Y:		return "TYR";
			case EAminoAcid::C:		return "CYS";
			case EAminoAcid::W:		return "TRP";
			case EAminoAcid::P:		return "PRO";
			case EAminoAcid::H:		return "HIS";
			case EAminoAcid::Q:		return "GLN";
			case EAminoAcid::R:		return "ARG";
			case EAminoAcid::I:		return "ILE";
			case EAminoAcid::M:		return "MET"; 
			case EAminoAcid::T:		return "THR";
			case EAminoAcid::N:		return "ASN";
			case EAminoAcid::K:		return "LYS";

			default:
				assert(false);
				break;
		}

		assert(false);
		return "[UNKNOWN]";
	}

	inline std::string ConvertAminoSequenceToThreeLetterCode(const AminoSequence& sequence) noexcept
	{
		std::string result;
		/* Ami-Ami-Ami-Ami */
		result.reserve(sequence.size() * 4U - 1U);
		
		for (size_t i{ 0U }; i < sequence.size() - 1U; ++i)
			result += AminoToThreeLetterCode(sequence[i]) + std::string("-");

		result += AminoToThreeLetterCode(sequence[sequence.size() - 1U]);
		return result;
	}

	template<typename Sequence>
	void PrintSequence(const Sequence& sequence)
	{
		/* Buffering significantly increases console throughput */
		std::vector<char> buffer(sequence.size() + 1U);

		for (uint32_t i{ 0U }; i < sequence.size(); ++i)
			buffer[i] = sequence[i].AsCharacter();

		buffer[buffer.size() - 1U] = '\0';
		fprintf(stdout, buffer.data());
	}
}

inline namespace literals
{
	constexpr Bio::Dna operator""_Dna(const char data)
	{
		return Bio::Dna{}.AssignCharacter(data);
	}

	constexpr Bio::DnaX operator""_DnaX(const char data)
	{
		return Bio::DnaX{}.AssignCharacter(data);
	}

	constexpr Bio::Rna operator""_Rna(const char data)
	{
		return Bio::Rna{}.AssignCharacter(data);
	}

	constexpr Bio::RnaX operator""_RnaX(const char data)
	{
		return Bio::RnaX{}.AssignCharacter(data);
	}

	constexpr Bio::DnaSequence operator""_Dnas(const char* data, const size_t size) noexcept
	{
		Bio::DnaSequence sequence(size);

		for (size_t i{ 0U }; i < size; ++i)
			sequence[i].AssignCharacter(data[i]);

		return sequence;
	}

	constexpr Bio::DnaXSequence operator""_DnaXs(const char* data, const size_t size) noexcept
	{
		Bio::DnaXSequence sequence(size);

		for (size_t i{ 0U }; i < size; ++i)
			sequence[i].AssignCharacter(data[i]);

		return sequence;
	}

	constexpr Bio::RnaSequence operator""_Rnas(const char* data, const size_t size) noexcept
	{
		Bio::RnaSequence sequence(size);

		for (size_t i{ 0U }; i < size; ++i)
			sequence[i].AssignCharacter(data[i]);

		return sequence;
	}

	constexpr Bio::RnaXSequence operator""_RnaXs(const char* data, const size_t size) noexcept
	{
		Bio::RnaXSequence sequence(size);

		for (size_t i{ 0U }; i < size; ++i)
			sequence[i].AssignCharacter(data[i]);

		return sequence;
	}

	constexpr Bio::AminoAcid operator""_Amino(const char character) noexcept
	{
		return Bio::AminoAcid{}.AssignCharacter(character);
	}

	constexpr Bio::AminoSequence operator""_Aminos(const char* data, const size_t size) noexcept
	{
		Bio::AminoSequence sequence(size);

		for (size_t i{ 0U }; i < size; ++i)
			sequence[i].AssignCharacter(data[i]);

		return sequence;
	}

	constexpr Bio::AminoAcidX operator""_AminoX(const char character) noexcept
	{
		return Bio::AminoAcidX{}.AssignCharacter(character);
	}

	constexpr Bio::AminoXSequence operator""_AminoXs(const char* data, const size_t size) noexcept
	{
		Bio::AminoXSequence sequence(size);

		for (size_t i{ 0U }; i < size; ++i)
			sequence[i].AssignCharacter(data[i]);

		return sequence;
	}
}