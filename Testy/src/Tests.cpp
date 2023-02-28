#include "Dna.hpp"
#include "DnaX.hpp"
#include "Nucleotides.hpp"
#include "Amino.hpp"
#include "AminoX.hpp"
#include "Transform.hpp"
#include <Windows.h>
#include <stdint.h>

static INT s_ExitCode{ EXIT_SUCCESS };
static uint32_t s_PassedTestCount{ 0U };
static uint32_t s_FailedTestCount{ 0U };

#define FORCE_ASSERT(condition) if(!(condition)) { fprintf(stderr, "===FAILED=== { %s }\n", #condition); ++s_FailedTestCount; s_ExitCode = EXIT_FAILURE; }
#define PASS_TEST() fprintf(stdout, "Success...\n"); ++s_PassedTestCount;

template<typename Dna>
void TestAssignCharacter(Dna& dna, const char input, const char expectedResult)
{
	dna.AssignCharacter(input);
	FORCE_ASSERT(dna.AsCharacter() == expectedResult);
}

template<double Error>
bool Approximate(const double value, const double expected) noexcept
{
	return std::abs(value - expected) <= Error;
}

/* Dna */
static void TestDNA();
static void TestDNAX();
/* Rna */
static void TestRNA();
static void TestRNAX();
/* Amino */
static void TestAmino();
static void TestAminoX();

/* Sequences */
static void TestDNASequence();
static void TestDNAXSequence();

static void TestRNASequence();
static void TestRNAXSequence();

/* Translation */
static void TestDNATranslation();
static void TestDNAXTranslation();

static void TestRNATranslation();
static void TestRNAXTranslation();

static void TestAminoTranslation();
static void TestAminoXTranslation();

static void TestCodonTranslation();

static void TestPropertyCalculations();

#define LOG(x) fprintf(stdout, "%s\n", x)
INT APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd)
{
	(void)hInstance;
	(void)hPrevInstance;
	(void)lpCmdLine;
	(void)nShowCmd;
	//seqan3::dna4;
	LOG("Testing Dna...");
	TestDNA();
	LOG("Testing DnaX...");
	TestDNAX();
	LOG("Testing Rna...");
	TestRNA();
	LOG("Testing RnaX...");
	TestRNAX();

	LOG("Testing Anmino...");
	TestAmino();
	LOG("Testing AnminoX...");
	TestAminoX();

	LOG("Testing Dna sequence...");
	TestDNASequence();
	LOG("Testing DnaX sequence...");
	TestDNAXSequence();

	LOG("Testing Rna sequence...");
	TestRNASequence();
	LOG("Testing RnaX sequence...");
	TestRNAXSequence();

	LOG("Testing DNA translation...");
	TestDNATranslation();
	LOG("Testing DNAX translation...");
	TestDNAXTranslation();

	LOG("Testing RNA translation...");
	TestRNATranslation();
	LOG("Testing RNAX translation...");
	TestRNAXTranslation();

	LOG("Testing Amino translation...");
	TestAminoTranslation();
	LOG("Testing AminoX translation...");
	TestAminoXTranslation();

	LOG("Testing codon translation...");
	TestCodonTranslation();

	LOG("Testing property calculations...");
	TestPropertyCalculations();

	LOG("============================");
	LOG("           SUMMARY			 ");
	LOG("============================");

	if (s_ExitCode == EXIT_SUCCESS)
		fprintf(stdout, "All %u tests have been passed!\n", s_PassedTestCount);
	else
	{
		fprintf(stderr, "%u assertion(s) have not been passed!\n", s_FailedTestCount);
		fprintf(stdout, "Tests passed: %u\n", s_PassedTestCount);
	}

#ifndef _DEBUG
	fprintf(stdout, "!!!Consider running the test in debug mode for additional checking!!!\n");
#endif
	return s_ExitCode;
}

static void TestDNA()
{
	Bio::Dna dna;
	for (uint32_t i{ 0U }; i < 256U; ++i)
	{
		const char character{ static_cast<char>(i) };

		switch (character)
		{
			case 'A':
			case 'T':
			case 'G':
			case 'C':
			case 'a':
			case 't':
			case 'g':
			case 'c':
				TestAssignCharacter(dna, character, static_cast<char>(std::toupper(character)));
				break;
			case 'U':
			case 'u':
				/* Turn uracil to thymine */
				TestAssignCharacter(dna, character, 'T');
				break;
			default:
				break;
		}
	}

	dna = 'A'_Dna;
	FORCE_ASSERT(dna.Complement().AsCharacter() == 'T');
	dna = 'T'_Dna;
	FORCE_ASSERT(dna.Complement().AsCharacter() == 'A');
	dna = 'C'_Dna;
	FORCE_ASSERT(dna.Complement().AsCharacter() == 'G');
	dna = 'G'_Dna;
	FORCE_ASSERT(dna.Complement().AsCharacter() == 'C');

	PASS_TEST();
}

static void TestDNAX()
{
	/* Test all 256 characters, unsupported should result in being 'X' */
	Bio::DnaX dna;
	for (uint32_t i{ 0U }; i < 256U; ++i)
	{
		const char character{ static_cast<char>(i) };

		switch (character)
		{
			case 'A':
			case 'T':
			case 'G':
			case 'C':
			case 'a':
			case 't':
			case 'g':
			case 'c':
				TestAssignCharacter(dna, character, static_cast<char>(std::toupper(character)));
				break;
			case 'U':
			case 'u':
				/* Turn uracil into thymine */
				TestAssignCharacter(dna, character, 'T');
				break;
			default:
				TestAssignCharacter(dna, character, Bio::DnaX::Invalid);
				break;
		}
	}

	PASS_TEST();
}

static void TestRNA()
{
	Bio::Rna dna;
	for (uint32_t i{ 0U }; i < 256U; ++i)
	{
		const char character{ static_cast<char>(i) };

		switch (character)
		{
			case 'A':
			case 'U':
			case 'G':
			case 'C':
			case 'a':
			case 'u':
			case 'g':
			case 'c':
				TestAssignCharacter(dna, character, static_cast<char>(std::toupper(character)));
				break;
			case 'T':
			case 't':
				/* Turn thymine into uracil */
				TestAssignCharacter(dna, character, 'U');
				break;
			default:
				break;
		}
	}

	PASS_TEST();
}

static void TestRNAX()
{
	/* Test all 256 characters, unsupported should result in being 'X' */
	Bio::RnaX dna;
	for (uint32_t i{ 0U }; i < 256U; ++i)
	{
		const char character{ static_cast<char>(i) };

		switch (character)
		{
			case 'A':
			case 'U':
			case 'G':
			case 'C':
			case 'a':
			case 'u':
			case 'g':
			case 'c':
				TestAssignCharacter(dna, character, static_cast<char>(std::toupper(character)));
				break;
			case 'T':
			case 't':
				/* Turn thymine into uracil */
				TestAssignCharacter(dna, character, 'U');
				break;
			default:
				TestAssignCharacter(dna, character, Bio::RnaX::Invalid);
				break;
		}
	}

	PASS_TEST();
}

static void TestAmino()
{
	Bio::AminoAcid dna;
	for (uint32_t i{ 0U }; i < 256U; ++i)
	{
		const char character{ static_cast<char>(i) };

		switch (character)
		{
			case 'V':
			case 'A':
			case 'D':
			case 'E':
			case 'G':
			case 'F':
			case 'L':
			case 'S':
			case 'Y':
			case 'C':
			case 'W':
			case 'P':
			case 'H':
			case 'Q':
			case 'R':
			case 'I':
			case 'M':
			case 'T':
			case 'N':
			case 'K':

			case 'v':
			case 'a':
			case 'd':
			case 'e':
			case 'g':
			case 'f':
			case 'l':
			case 's':
			case 'y':
			case 'c':
			case 'w':
			case 'p':
			case 'h':
			case 'q':
			case 'r':
			case 'i':
			case 'm':
			case 't':
			case 'n':
			case 'k':
				TestAssignCharacter(dna, character, static_cast<char>(std::toupper(character)));
				break;
			default:
				break;
		}
	}

	PASS_TEST();
}

static void TestAminoX()
{
	Bio::AminoAcidX dna;
	for (uint32_t i{ 0U }; i < 256U; ++i)
	{
		const char character{ static_cast<char>(i) };

		switch (character)
		{
			case 'V':
			case 'A':
			case 'D':
			case 'E':
			case 'G':
			case 'F':
			case 'L':
			case 'S':
			case 'Y':
			case 'C':
			case 'W':
			case 'P':
			case 'H':
			case 'Q':
			case 'R':
			case 'I':
			case 'M':
			case 'T':
			case 'N':
			case 'K':

			case 'v':
			case 'a':
			case 'd':
			case 'e':
			case 'g':
			case 'f':
			case 'l':
			case 's':
			case 'y':
			case 'c':
			case 'w':
			case 'p':
			case 'h':
			case 'q':
			case 'r':
			case 'i':
			case 'm':
			case 't':
			case 'n':
			case 'k':
			case '-':
				TestAssignCharacter(dna, character, static_cast<char>(std::toupper(character)));
				break;
			default:
				TestAssignCharacter(dna, character, Bio::AminoAcidX::Invalid);
				break;
		}
	}

	PASS_TEST();
}

static void TestDNASequence()
{
	{
		Bio::DnaSequence sequence{ "atgcTGCTA"_Dnas };
		FORCE_ASSERT(sequence.size() == 9U);
		FORCE_ASSERT(sequence[0U].AsCharacter() == 'A');
		FORCE_ASSERT(sequence[1U].AsCharacter() == 'T');
		FORCE_ASSERT(sequence[2U].AsCharacter() == 'G');

		FORCE_ASSERT(sequence[3U].AsCharacter() == 'C');
		FORCE_ASSERT(sequence[4U].AsCharacter() == 'T');
		FORCE_ASSERT(sequence[5U].AsCharacter() == 'G');

		FORCE_ASSERT(sequence[6U].AsCharacter() == 'C');
		FORCE_ASSERT(sequence[7U].AsCharacter() == 'T');
		FORCE_ASSERT(sequence[8U].AsCharacter() == 'A');
	}

	{
		Bio::DnaSequence sequence{ "UTAGTu"_Dnas };
		FORCE_ASSERT(sequence.size() == 6U);
		FORCE_ASSERT(sequence[0U].AsCharacter() == 'T');
		FORCE_ASSERT(sequence[1U].AsCharacter() == 'T');
		FORCE_ASSERT(sequence[2U].AsCharacter() == 'A');

		FORCE_ASSERT(sequence[3U].AsCharacter() == 'G');
		FORCE_ASSERT(sequence[4U].AsCharacter() == 'T');
		FORCE_ASSERT(sequence[5U].AsCharacter() == 'T');
	}

	PASS_TEST();
}

static void TestDNAXSequence()
{
	{
		Bio::DnaXSequence sequence{ "atg cTG CTA"_DnaXs };
		FORCE_ASSERT(sequence.size() == 11U);
		FORCE_ASSERT(sequence[0U].AsCharacter() == 'A');
		FORCE_ASSERT(sequence[1U].AsCharacter() == 'T');
		FORCE_ASSERT(sequence[2U].AsCharacter() == 'G');

		FORCE_ASSERT(sequence[4U].AsCharacter() == 'C');
		FORCE_ASSERT(sequence[5U].AsCharacter() == 'T');
		FORCE_ASSERT(sequence[6U].AsCharacter() == 'G');

		FORCE_ASSERT(sequence[8U].AsCharacter() == 'C');
		FORCE_ASSERT(sequence[9U].AsCharacter() == 'T');
		FORCE_ASSERT(sequence[10U].AsCharacter() == 'A');
	}

	{
		Bio::DnaXSequence sequence{ "UTA GTu"_DnaXs };
		FORCE_ASSERT(sequence.size() == 7U);
		FORCE_ASSERT(sequence[0U].AsCharacter() == 'T');
		FORCE_ASSERT(sequence[1U].AsCharacter() == 'T');
		FORCE_ASSERT(sequence[2U].AsCharacter() == 'A');

		FORCE_ASSERT(sequence[4U].AsCharacter() == 'G');
		FORCE_ASSERT(sequence[5U].AsCharacter() == 'T');
		FORCE_ASSERT(sequence[6U].AsCharacter() == 'T');
	}

	PASS_TEST();
}

static void TestRNASequence()
{
	{
		Bio::RnaSequence sequence{ "atgcTGCTA"_Rnas };
		FORCE_ASSERT(sequence.size() == 9U);
		FORCE_ASSERT(sequence[0U].AsCharacter() == 'A');
		FORCE_ASSERT(sequence[1U].AsCharacter() == 'U');
		FORCE_ASSERT(sequence[2U].AsCharacter() == 'G');

		FORCE_ASSERT(sequence[3U].AsCharacter() == 'C');
		FORCE_ASSERT(sequence[4U].AsCharacter() == 'U');
		FORCE_ASSERT(sequence[5U].AsCharacter() == 'G');

		FORCE_ASSERT(sequence[6U].AsCharacter() == 'C');
		FORCE_ASSERT(sequence[7U].AsCharacter() == 'U');
		FORCE_ASSERT(sequence[8U].AsCharacter() == 'A');
	}

	{
		Bio::RnaSequence sequence{ "UTAGTu"_Rnas };
		FORCE_ASSERT(sequence.size() == 6U);
		FORCE_ASSERT(sequence[0U].AsCharacter() == 'U');
		FORCE_ASSERT(sequence[1U].AsCharacter() == 'U');
		FORCE_ASSERT(sequence[2U].AsCharacter() == 'A');

		FORCE_ASSERT(sequence[3U].AsCharacter() == 'G');
		FORCE_ASSERT(sequence[4U].AsCharacter() == 'U');
		FORCE_ASSERT(sequence[5U].AsCharacter() == 'U');
	}

	PASS_TEST();
}

static void TestRNAXSequence()
{
	{
		Bio::RnaXSequence sequence{ "atg cTG CTA"_RnaXs };
		FORCE_ASSERT(sequence.size() == 11U);
		FORCE_ASSERT(sequence[0U].AsCharacter() == 'A');
		FORCE_ASSERT(sequence[1U].AsCharacter() == 'U');
		FORCE_ASSERT(sequence[2U].AsCharacter() == 'G');

		FORCE_ASSERT(sequence[4U].AsCharacter() == 'C');
		FORCE_ASSERT(sequence[5U].AsCharacter() == 'U');
		FORCE_ASSERT(sequence[6U].AsCharacter() == 'G');

		FORCE_ASSERT(sequence[8U].AsCharacter() == 'C');
		FORCE_ASSERT(sequence[9U].AsCharacter() == 'U');
		FORCE_ASSERT(sequence[10U].AsCharacter() == 'A');
	}

	{
		Bio::RnaXSequence sequence{ "UTA GTu"_RnaXs };
		FORCE_ASSERT(sequence.size() == 7U);
		FORCE_ASSERT(sequence[0U].AsCharacter() == 'U');
		FORCE_ASSERT(sequence[1U].AsCharacter() == 'U');
		FORCE_ASSERT(sequence[2U].AsCharacter() == 'A');

		FORCE_ASSERT(sequence[4U].AsCharacter() == 'G');
		FORCE_ASSERT(sequence[5U].AsCharacter() == 'U');
		FORCE_ASSERT(sequence[6U].AsCharacter() == 'U');
	}

	PASS_TEST();
}

static void TestDNATranslation()
{
	auto fromString{ Bio::ConvertToDNA(std::string("atgATGaug")) };
	FORCE_ASSERT(fromString[0].AsCharacter() == 'A');
	FORCE_ASSERT(fromString[1].AsCharacter() == 'T');
	FORCE_ASSERT(fromString[2].AsCharacter() == 'G');
	FORCE_ASSERT(fromString[3].AsCharacter() == 'A');
	FORCE_ASSERT(fromString[4].AsCharacter() == 'T');
	FORCE_ASSERT(fromString[5].AsCharacter() == 'G');
	FORCE_ASSERT(fromString[6].AsCharacter() == 'A');
	FORCE_ASSERT(fromString[7].AsCharacter() == 'T');
	FORCE_ASSERT(fromString[8].AsCharacter() == 'G');

	auto fromStringView{ Bio::ConvertToDNA(std::string_view("atgATGaug{")) };
	FORCE_ASSERT(fromStringView[0].AsCharacter() == 'A');
	FORCE_ASSERT(fromStringView[1].AsCharacter() == 'T');
	FORCE_ASSERT(fromStringView[2].AsCharacter() == 'G');
	FORCE_ASSERT(fromStringView[3].AsCharacter() == 'A');
	FORCE_ASSERT(fromStringView[4].AsCharacter() == 'T');
	FORCE_ASSERT(fromStringView[5].AsCharacter() == 'G');
	FORCE_ASSERT(fromStringView[6].AsCharacter() == 'A');
	FORCE_ASSERT(fromStringView[7].AsCharacter() == 'T');
	FORCE_ASSERT(fromStringView[8].AsCharacter() == 'G');

	auto fromVector{ Bio::ConvertToDNA(std::vector<char>{ 'a', 't', 'g', 'A', 'T', 'G', 'a', 'u', 'g', '0' }) };
	FORCE_ASSERT(fromVector[0].AsCharacter() == 'A');
	FORCE_ASSERT(fromVector[1].AsCharacter() == 'T');
	FORCE_ASSERT(fromVector[2].AsCharacter() == 'G');
	FORCE_ASSERT(fromVector[3].AsCharacter() == 'A');
	FORCE_ASSERT(fromVector[4].AsCharacter() == 'T');
	FORCE_ASSERT(fromVector[5].AsCharacter() == 'G');
	FORCE_ASSERT(fromVector[6].AsCharacter() == 'A');
	FORCE_ASSERT(fromVector[7].AsCharacter() == 'T');
	FORCE_ASSERT(fromVector[8].AsCharacter() == 'G');

	PASS_TEST();
}

static void TestDNAXTranslation()
{
	auto fromString{ Bio::ConvertToDNAX(std::string("atgATGaug#")) };
	FORCE_ASSERT(fromString[0].AsCharacter() == 'A');
	FORCE_ASSERT(fromString[1].AsCharacter() == 'T');
	FORCE_ASSERT(fromString[2].AsCharacter() == 'G');
	FORCE_ASSERT(fromString[3].AsCharacter() == 'A');
	FORCE_ASSERT(fromString[4].AsCharacter() == 'T');
	FORCE_ASSERT(fromString[5].AsCharacter() == 'G');
	FORCE_ASSERT(fromString[6].AsCharacter() == 'A');
	FORCE_ASSERT(fromString[7].AsCharacter() == 'T');
	FORCE_ASSERT(fromString[8].AsCharacter() == 'G');
	FORCE_ASSERT(fromString[9].AsCharacter() == Bio::DnaX::Invalid);

	auto fromStringView{ Bio::ConvertToDNAX(std::string_view("atgATGaug{")) };
	FORCE_ASSERT(fromStringView[0].AsCharacter() == 'A');
	FORCE_ASSERT(fromStringView[1].AsCharacter() == 'T');
	FORCE_ASSERT(fromStringView[2].AsCharacter() == 'G');
	FORCE_ASSERT(fromStringView[3].AsCharacter() == 'A');
	FORCE_ASSERT(fromStringView[4].AsCharacter() == 'T');
	FORCE_ASSERT(fromStringView[5].AsCharacter() == 'G');
	FORCE_ASSERT(fromStringView[6].AsCharacter() == 'A');
	FORCE_ASSERT(fromStringView[7].AsCharacter() == 'T');
	FORCE_ASSERT(fromStringView[8].AsCharacter() == 'G');
	FORCE_ASSERT(fromStringView[9].AsCharacter() == Bio::DnaX::Invalid);

	auto fromVector{ Bio::ConvertToDNAX(std::vector<char>{ 'a', 't', 'g', 'A', 'T', 'G', 'a', 'u', 'g', '0' }) };
	FORCE_ASSERT(fromVector[0].AsCharacter() == 'A');
	FORCE_ASSERT(fromVector[1].AsCharacter() == 'T');
	FORCE_ASSERT(fromVector[2].AsCharacter() == 'G');
	FORCE_ASSERT(fromVector[3].AsCharacter() == 'A');
	FORCE_ASSERT(fromVector[4].AsCharacter() == 'T');
	FORCE_ASSERT(fromVector[5].AsCharacter() == 'G');
	FORCE_ASSERT(fromVector[6].AsCharacter() == 'A');
	FORCE_ASSERT(fromVector[7].AsCharacter() == 'T');
	FORCE_ASSERT(fromVector[8].AsCharacter() == 'G');
	FORCE_ASSERT(fromVector[9].AsCharacter() == Bio::DnaX::Invalid);

	PASS_TEST();
}

static void TestRNATranslation()
{
	auto fromString{ Bio::ConvertToRNA(std::string("atgATGaug")) };
	FORCE_ASSERT(fromString[0].AsCharacter() == 'A');
	FORCE_ASSERT(fromString[1].AsCharacter() == 'U');
	FORCE_ASSERT(fromString[2].AsCharacter() == 'G');
	FORCE_ASSERT(fromString[3].AsCharacter() == 'A');
	FORCE_ASSERT(fromString[4].AsCharacter() == 'U');
	FORCE_ASSERT(fromString[5].AsCharacter() == 'G');
	FORCE_ASSERT(fromString[6].AsCharacter() == 'A');
	FORCE_ASSERT(fromString[7].AsCharacter() == 'U');
	FORCE_ASSERT(fromString[8].AsCharacter() == 'G');

	auto fromStringView{ Bio::ConvertToRNA(std::string_view("atgATGaug{")) };
	FORCE_ASSERT(fromStringView[0].AsCharacter() == 'A');
	FORCE_ASSERT(fromStringView[1].AsCharacter() == 'U');
	FORCE_ASSERT(fromStringView[2].AsCharacter() == 'G');
	FORCE_ASSERT(fromStringView[3].AsCharacter() == 'A');
	FORCE_ASSERT(fromStringView[4].AsCharacter() == 'U');
	FORCE_ASSERT(fromStringView[5].AsCharacter() == 'G');
	FORCE_ASSERT(fromStringView[6].AsCharacter() == 'A');
	FORCE_ASSERT(fromStringView[7].AsCharacter() == 'U');
	FORCE_ASSERT(fromStringView[8].AsCharacter() == 'G');

	auto fromVector{ Bio::ConvertToRNA(std::vector<char>{ 'a', 't', 'g', 'A', 'T', 'G', 'a', 'u', 'g', '0' }) };
	FORCE_ASSERT(fromVector[0].AsCharacter() == 'A');
	FORCE_ASSERT(fromVector[1].AsCharacter() == 'U');
	FORCE_ASSERT(fromVector[2].AsCharacter() == 'G');
	FORCE_ASSERT(fromVector[3].AsCharacter() == 'A');
	FORCE_ASSERT(fromVector[4].AsCharacter() == 'U');
	FORCE_ASSERT(fromVector[5].AsCharacter() == 'G');
	FORCE_ASSERT(fromVector[6].AsCharacter() == 'A');
	FORCE_ASSERT(fromVector[7].AsCharacter() == 'U');
	FORCE_ASSERT(fromVector[8].AsCharacter() == 'G');

	PASS_TEST();
}

static void TestRNAXTranslation()
{
	auto fromString{ Bio::ConvertToRNAX(std::string("atgATGaug#")) };
	FORCE_ASSERT(fromString[0].AsCharacter() == 'A');
	FORCE_ASSERT(fromString[1].AsCharacter() == 'U');
	FORCE_ASSERT(fromString[2].AsCharacter() == 'G');
	FORCE_ASSERT(fromString[3].AsCharacter() == 'A');
	FORCE_ASSERT(fromString[4].AsCharacter() == 'U');
	FORCE_ASSERT(fromString[5].AsCharacter() == 'G');
	FORCE_ASSERT(fromString[6].AsCharacter() == 'A');
	FORCE_ASSERT(fromString[7].AsCharacter() == 'U');
	FORCE_ASSERT(fromString[8].AsCharacter() == 'G');
	FORCE_ASSERT(fromString[9].AsCharacter() == Bio::RnaX::Invalid);

	auto fromStringView{ Bio::ConvertToRNAX(std::string_view("atgATGaug{")) };
	FORCE_ASSERT(fromStringView[0].AsCharacter() == 'A');
	FORCE_ASSERT(fromStringView[1].AsCharacter() == 'U');
	FORCE_ASSERT(fromStringView[2].AsCharacter() == 'G');
	FORCE_ASSERT(fromStringView[3].AsCharacter() == 'A');
	FORCE_ASSERT(fromStringView[4].AsCharacter() == 'U');
	FORCE_ASSERT(fromStringView[5].AsCharacter() == 'G');
	FORCE_ASSERT(fromStringView[6].AsCharacter() == 'A');
	FORCE_ASSERT(fromStringView[7].AsCharacter() == 'U');
	FORCE_ASSERT(fromStringView[8].AsCharacter() == 'G');
	FORCE_ASSERT(fromStringView[9].AsCharacter() == Bio::RnaX::Invalid);

	auto fromVector{ Bio::ConvertToRNAX(std::vector<char>{ 'a', 't', 'g', 'A', 'T', 'G', 'a', 'u', 'g', '0' }) };
	FORCE_ASSERT(fromVector[0].AsCharacter() == 'A');
	FORCE_ASSERT(fromVector[1].AsCharacter() == 'U');
	FORCE_ASSERT(fromVector[2].AsCharacter() == 'G');
	FORCE_ASSERT(fromVector[3].AsCharacter() == 'A');
	FORCE_ASSERT(fromVector[4].AsCharacter() == 'U');
	FORCE_ASSERT(fromVector[5].AsCharacter() == 'G');
	FORCE_ASSERT(fromVector[6].AsCharacter() == 'A');
	FORCE_ASSERT(fromVector[7].AsCharacter() == 'U');
	FORCE_ASSERT(fromVector[8].AsCharacter() == 'G');
	FORCE_ASSERT(fromVector[9].AsCharacter() == Bio::RnaX::Invalid);

	PASS_TEST();
}

static void TestAminoTranslation()
{
	auto fromString{ Bio::ConvertToAminoSequence(std::string("aArRnNdDcCqQeEgGhHiIlLkKmMfFpPsStTwWyYvV")) };
	FORCE_ASSERT(fromString[0].AsCharacter() == 'A');
	FORCE_ASSERT(fromString[1].AsCharacter() == 'A');

	FORCE_ASSERT(fromString[2].AsCharacter() == 'R');
	FORCE_ASSERT(fromString[3].AsCharacter() == 'R');

	FORCE_ASSERT(fromString[4].AsCharacter() == 'N');
	FORCE_ASSERT(fromString[5].AsCharacter() == 'N');

	FORCE_ASSERT(fromString[6].AsCharacter() == 'D');
	FORCE_ASSERT(fromString[7].AsCharacter() == 'D');

	FORCE_ASSERT(fromString[8].AsCharacter() == 'C');
	FORCE_ASSERT(fromString[9].AsCharacter() == 'C');

	FORCE_ASSERT(fromString[10].AsCharacter() == 'Q');
	FORCE_ASSERT(fromString[11].AsCharacter() == 'Q');

	FORCE_ASSERT(fromString[12].AsCharacter() == 'E');
	FORCE_ASSERT(fromString[13].AsCharacter() == 'E');

	FORCE_ASSERT(fromString[14].AsCharacter() == 'G');
	FORCE_ASSERT(fromString[15].AsCharacter() == 'G');

	FORCE_ASSERT(fromString[16].AsCharacter() == 'H');
	FORCE_ASSERT(fromString[17].AsCharacter() == 'H');

	FORCE_ASSERT(fromString[18].AsCharacter() == 'I');
	FORCE_ASSERT(fromString[19].AsCharacter() == 'I');

	FORCE_ASSERT(fromString[20].AsCharacter() == 'L');
	FORCE_ASSERT(fromString[21].AsCharacter() == 'L');

	FORCE_ASSERT(fromString[22].AsCharacter() == 'K');
	FORCE_ASSERT(fromString[23].AsCharacter() == 'K');

	FORCE_ASSERT(fromString[24].AsCharacter() == 'M');
	FORCE_ASSERT(fromString[25].AsCharacter() == 'M');

	FORCE_ASSERT(fromString[26].AsCharacter() == 'F');
	FORCE_ASSERT(fromString[27].AsCharacter() == 'F');

	FORCE_ASSERT(fromString[28].AsCharacter() == 'P');
	FORCE_ASSERT(fromString[29].AsCharacter() == 'P');

	FORCE_ASSERT(fromString[30].AsCharacter() == 'S');
	FORCE_ASSERT(fromString[31].AsCharacter() == 'S');

	FORCE_ASSERT(fromString[32].AsCharacter() == 'T');
	FORCE_ASSERT(fromString[33].AsCharacter() == 'T');

	FORCE_ASSERT(fromString[34].AsCharacter() == 'W');
	FORCE_ASSERT(fromString[35].AsCharacter() == 'W');

	FORCE_ASSERT(fromString[36].AsCharacter() == 'Y');
	FORCE_ASSERT(fromString[37].AsCharacter() == 'Y');

	FORCE_ASSERT(fromString[38].AsCharacter() == 'V');
	FORCE_ASSERT(fromString[39].AsCharacter() == 'V');

	auto fromStringView{ Bio::ConvertToAminoSequence(std::string_view("aArRnNdDcCqQeEgGhHiIlLkKmMfFpPsStTwWyYvV")) };
	FORCE_ASSERT(fromStringView[0].AsCharacter() == 'A');
	FORCE_ASSERT(fromStringView[1].AsCharacter() == 'A');
				 
	FORCE_ASSERT(fromStringView[2].AsCharacter() == 'R');
	FORCE_ASSERT(fromStringView[3].AsCharacter() == 'R');
				 
	FORCE_ASSERT(fromStringView[4].AsCharacter() == 'N');
	FORCE_ASSERT(fromStringView[5].AsCharacter() == 'N');
				 
	FORCE_ASSERT(fromStringView[6].AsCharacter() == 'D');
	FORCE_ASSERT(fromStringView[7].AsCharacter() == 'D');
				 
	FORCE_ASSERT(fromStringView[8].AsCharacter() == 'C');
	FORCE_ASSERT(fromStringView[9].AsCharacter() == 'C');
				 
	FORCE_ASSERT(fromStringView[10].AsCharacter() == 'Q');
	FORCE_ASSERT(fromStringView[11].AsCharacter() == 'Q');
				 
	FORCE_ASSERT(fromStringView[12].AsCharacter() == 'E');
	FORCE_ASSERT(fromStringView[13].AsCharacter() == 'E');
				 
	FORCE_ASSERT(fromStringView[14].AsCharacter() == 'G');
	FORCE_ASSERT(fromStringView[15].AsCharacter() == 'G');
				 
	FORCE_ASSERT(fromStringView[16].AsCharacter() == 'H');
	FORCE_ASSERT(fromStringView[17].AsCharacter() == 'H');
				 
	FORCE_ASSERT(fromStringView[18].AsCharacter() == 'I');
	FORCE_ASSERT(fromStringView[19].AsCharacter() == 'I');
				 
	FORCE_ASSERT(fromStringView[20].AsCharacter() == 'L');
	FORCE_ASSERT(fromStringView[21].AsCharacter() == 'L');
				 
	FORCE_ASSERT(fromStringView[22].AsCharacter() == 'K');
	FORCE_ASSERT(fromStringView[23].AsCharacter() == 'K');
				 
	FORCE_ASSERT(fromStringView[24].AsCharacter() == 'M');
	FORCE_ASSERT(fromStringView[25].AsCharacter() == 'M');
				 
	FORCE_ASSERT(fromStringView[26].AsCharacter() == 'F');
	FORCE_ASSERT(fromStringView[27].AsCharacter() == 'F');
				 
	FORCE_ASSERT(fromStringView[28].AsCharacter() == 'P');
	FORCE_ASSERT(fromStringView[29].AsCharacter() == 'P');
				 
	FORCE_ASSERT(fromStringView[30].AsCharacter() == 'S');
	FORCE_ASSERT(fromStringView[31].AsCharacter() == 'S');
				 
	FORCE_ASSERT(fromStringView[32].AsCharacter() == 'T');
	FORCE_ASSERT(fromStringView[33].AsCharacter() == 'T');
				 
	FORCE_ASSERT(fromStringView[34].AsCharacter() == 'W');
	FORCE_ASSERT(fromStringView[35].AsCharacter() == 'W');
				 
	FORCE_ASSERT(fromStringView[36].AsCharacter() == 'Y');
	FORCE_ASSERT(fromStringView[37].AsCharacter() == 'Y');
				 
	FORCE_ASSERT(fromStringView[38].AsCharacter() == 'V');
	FORCE_ASSERT(fromStringView[39].AsCharacter() == 'V');

	const auto deconstructString = [](const char* data) -> std::vector<char>
	{
		std::vector<char> result;

		while (*data != '\0')
		{
			result.emplace_back(*data);
			++data;
		}

		return result;
	};

	auto fromVector{ Bio::ConvertToAminoSequence(deconstructString("aArRnNdDcCqQeEgGhHiIlLkKmMfFpPsStTwWyYvV")) };
	FORCE_ASSERT(fromVector[0].AsCharacter() == 'A');
	FORCE_ASSERT(fromVector[1].AsCharacter() == 'A');
				 
	FORCE_ASSERT(fromVector[2].AsCharacter() == 'R');
	FORCE_ASSERT(fromVector[3].AsCharacter() == 'R');
				 
	FORCE_ASSERT(fromVector[4].AsCharacter() == 'N');
	FORCE_ASSERT(fromVector[5].AsCharacter() == 'N');
				 
	FORCE_ASSERT(fromVector[6].AsCharacter() == 'D');
	FORCE_ASSERT(fromVector[7].AsCharacter() == 'D');
				 
	FORCE_ASSERT(fromVector[8].AsCharacter() == 'C');
	FORCE_ASSERT(fromVector[9].AsCharacter() == 'C');
				 
	FORCE_ASSERT(fromVector[10].AsCharacter() == 'Q');
	FORCE_ASSERT(fromVector[11].AsCharacter() == 'Q');
				 
	FORCE_ASSERT(fromVector[12].AsCharacter() == 'E');
	FORCE_ASSERT(fromVector[13].AsCharacter() == 'E');
				 
	FORCE_ASSERT(fromVector[14].AsCharacter() == 'G');
	FORCE_ASSERT(fromVector[15].AsCharacter() == 'G');
				 
	FORCE_ASSERT(fromVector[16].AsCharacter() == 'H');
	FORCE_ASSERT(fromVector[17].AsCharacter() == 'H');
				 
	FORCE_ASSERT(fromVector[18].AsCharacter() == 'I');
	FORCE_ASSERT(fromVector[19].AsCharacter() == 'I');
				 
	FORCE_ASSERT(fromVector[20].AsCharacter() == 'L');
	FORCE_ASSERT(fromVector[21].AsCharacter() == 'L');
				 
	FORCE_ASSERT(fromVector[22].AsCharacter() == 'K');
	FORCE_ASSERT(fromVector[23].AsCharacter() == 'K');
				 
	FORCE_ASSERT(fromVector[24].AsCharacter() == 'M');
	FORCE_ASSERT(fromVector[25].AsCharacter() == 'M');
				 
	FORCE_ASSERT(fromVector[26].AsCharacter() == 'F');
	FORCE_ASSERT(fromVector[27].AsCharacter() == 'F');
				 
	FORCE_ASSERT(fromVector[28].AsCharacter() == 'P');
	FORCE_ASSERT(fromVector[29].AsCharacter() == 'P');
				 
	FORCE_ASSERT(fromVector[30].AsCharacter() == 'S');
	FORCE_ASSERT(fromVector[31].AsCharacter() == 'S');
				 
	FORCE_ASSERT(fromVector[32].AsCharacter() == 'T');
	FORCE_ASSERT(fromVector[33].AsCharacter() == 'T');
				 
	FORCE_ASSERT(fromVector[34].AsCharacter() == 'W');
	FORCE_ASSERT(fromVector[35].AsCharacter() == 'W');
				 
	FORCE_ASSERT(fromVector[36].AsCharacter() == 'Y');
	FORCE_ASSERT(fromVector[37].AsCharacter() == 'Y');
				
	FORCE_ASSERT(fromVector[38].AsCharacter() == 'V');
	FORCE_ASSERT(fromVector[39].AsCharacter() == 'V');

	PASS_TEST();
}

static void TestAminoXTranslation()
{
	auto fromString{ Bio::ConvertToAminoXSequence(std::string("aArRnNdDcCqQeEgGhHiIlLkKmMfFpPsStTwWyYvV$")) };
	FORCE_ASSERT(fromString[0].AsCharacter() == 'A');
	FORCE_ASSERT(fromString[1].AsCharacter() == 'A');

	FORCE_ASSERT(fromString[2].AsCharacter() == 'R');
	FORCE_ASSERT(fromString[3].AsCharacter() == 'R');

	FORCE_ASSERT(fromString[4].AsCharacter() == 'N');
	FORCE_ASSERT(fromString[5].AsCharacter() == 'N');

	FORCE_ASSERT(fromString[6].AsCharacter() == 'D');
	FORCE_ASSERT(fromString[7].AsCharacter() == 'D');

	FORCE_ASSERT(fromString[8].AsCharacter() == 'C');
	FORCE_ASSERT(fromString[9].AsCharacter() == 'C');

	FORCE_ASSERT(fromString[10].AsCharacter() == 'Q');
	FORCE_ASSERT(fromString[11].AsCharacter() == 'Q');

	FORCE_ASSERT(fromString[12].AsCharacter() == 'E');
	FORCE_ASSERT(fromString[13].AsCharacter() == 'E');

	FORCE_ASSERT(fromString[14].AsCharacter() == 'G');
	FORCE_ASSERT(fromString[15].AsCharacter() == 'G');

	FORCE_ASSERT(fromString[16].AsCharacter() == 'H');
	FORCE_ASSERT(fromString[17].AsCharacter() == 'H');

	FORCE_ASSERT(fromString[18].AsCharacter() == 'I');
	FORCE_ASSERT(fromString[19].AsCharacter() == 'I');

	FORCE_ASSERT(fromString[20].AsCharacter() == 'L');
	FORCE_ASSERT(fromString[21].AsCharacter() == 'L');

	FORCE_ASSERT(fromString[22].AsCharacter() == 'K');
	FORCE_ASSERT(fromString[23].AsCharacter() == 'K');

	FORCE_ASSERT(fromString[24].AsCharacter() == 'M');
	FORCE_ASSERT(fromString[25].AsCharacter() == 'M');

	FORCE_ASSERT(fromString[26].AsCharacter() == 'F');
	FORCE_ASSERT(fromString[27].AsCharacter() == 'F');

	FORCE_ASSERT(fromString[28].AsCharacter() == 'P');
	FORCE_ASSERT(fromString[29].AsCharacter() == 'P');

	FORCE_ASSERT(fromString[30].AsCharacter() == 'S');
	FORCE_ASSERT(fromString[31].AsCharacter() == 'S');

	FORCE_ASSERT(fromString[32].AsCharacter() == 'T');
	FORCE_ASSERT(fromString[33].AsCharacter() == 'T');

	FORCE_ASSERT(fromString[34].AsCharacter() == 'W');
	FORCE_ASSERT(fromString[35].AsCharacter() == 'W');

	FORCE_ASSERT(fromString[36].AsCharacter() == 'Y');
	FORCE_ASSERT(fromString[37].AsCharacter() == 'Y');

	FORCE_ASSERT(fromString[38].AsCharacter() == 'V');
	FORCE_ASSERT(fromString[39].AsCharacter() == 'V');
	
	FORCE_ASSERT(fromString[40].AsCharacter() == Bio::AminoAcidX::Invalid);


	auto fromStringView{ Bio::ConvertToAminoXSequence(std::string_view("aArRnNdDcCqQeEgGhHiIlLkKmMfFpPsStTwWyYvV!")) };
	FORCE_ASSERT(fromStringView[0].AsCharacter() == 'A');
	FORCE_ASSERT(fromStringView[1].AsCharacter() == 'A');

	FORCE_ASSERT(fromStringView[2].AsCharacter() == 'R');
	FORCE_ASSERT(fromStringView[3].AsCharacter() == 'R');

	FORCE_ASSERT(fromStringView[4].AsCharacter() == 'N');
	FORCE_ASSERT(fromStringView[5].AsCharacter() == 'N');

	FORCE_ASSERT(fromStringView[6].AsCharacter() == 'D');
	FORCE_ASSERT(fromStringView[7].AsCharacter() == 'D');

	FORCE_ASSERT(fromStringView[8].AsCharacter() == 'C');
	FORCE_ASSERT(fromStringView[9].AsCharacter() == 'C');

	FORCE_ASSERT(fromStringView[10].AsCharacter() == 'Q');
	FORCE_ASSERT(fromStringView[11].AsCharacter() == 'Q');

	FORCE_ASSERT(fromStringView[12].AsCharacter() == 'E');
	FORCE_ASSERT(fromStringView[13].AsCharacter() == 'E');

	FORCE_ASSERT(fromStringView[14].AsCharacter() == 'G');
	FORCE_ASSERT(fromStringView[15].AsCharacter() == 'G');

	FORCE_ASSERT(fromStringView[16].AsCharacter() == 'H');
	FORCE_ASSERT(fromStringView[17].AsCharacter() == 'H');

	FORCE_ASSERT(fromStringView[18].AsCharacter() == 'I');
	FORCE_ASSERT(fromStringView[19].AsCharacter() == 'I');

	FORCE_ASSERT(fromStringView[20].AsCharacter() == 'L');
	FORCE_ASSERT(fromStringView[21].AsCharacter() == 'L');

	FORCE_ASSERT(fromStringView[22].AsCharacter() == 'K');
	FORCE_ASSERT(fromStringView[23].AsCharacter() == 'K');

	FORCE_ASSERT(fromStringView[24].AsCharacter() == 'M');
	FORCE_ASSERT(fromStringView[25].AsCharacter() == 'M');

	FORCE_ASSERT(fromStringView[26].AsCharacter() == 'F');
	FORCE_ASSERT(fromStringView[27].AsCharacter() == 'F');

	FORCE_ASSERT(fromStringView[28].AsCharacter() == 'P');
	FORCE_ASSERT(fromStringView[29].AsCharacter() == 'P');

	FORCE_ASSERT(fromStringView[30].AsCharacter() == 'S');
	FORCE_ASSERT(fromStringView[31].AsCharacter() == 'S');

	FORCE_ASSERT(fromStringView[32].AsCharacter() == 'T');
	FORCE_ASSERT(fromStringView[33].AsCharacter() == 'T');

	FORCE_ASSERT(fromStringView[34].AsCharacter() == 'W');
	FORCE_ASSERT(fromStringView[35].AsCharacter() == 'W');

	FORCE_ASSERT(fromStringView[36].AsCharacter() == 'Y');
	FORCE_ASSERT(fromStringView[37].AsCharacter() == 'Y');

	FORCE_ASSERT(fromStringView[38].AsCharacter() == 'V');
	FORCE_ASSERT(fromStringView[39].AsCharacter() == 'V');

	FORCE_ASSERT(fromStringView[40].AsCharacter() == Bio::AminoAcidX::Invalid);

	const auto deconstructString = [](const char* data) -> std::vector<char>
	{
		std::vector<char> result;

		while (*data != '\0')
		{
			result.emplace_back(*data);
			++data;
		}

		return result;
	};

	auto fromVector{ Bio::ConvertToAminoXSequence(deconstructString("aArRnNdDcCqQeEgGhHiIlLkKmMfFpPsStTwWyYvVj")) };
	FORCE_ASSERT(fromVector[0].AsCharacter() == 'A');
	FORCE_ASSERT(fromVector[1].AsCharacter() == 'A');
				 
	FORCE_ASSERT(fromVector[2].AsCharacter() == 'R');
	FORCE_ASSERT(fromVector[3].AsCharacter() == 'R');
				 
	FORCE_ASSERT(fromVector[4].AsCharacter() == 'N');
	FORCE_ASSERT(fromVector[5].AsCharacter() == 'N');
				 
	FORCE_ASSERT(fromVector[6].AsCharacter() == 'D');
	FORCE_ASSERT(fromVector[7].AsCharacter() == 'D');
				 
	FORCE_ASSERT(fromVector[8].AsCharacter() == 'C');
	FORCE_ASSERT(fromVector[9].AsCharacter() == 'C');
				 
	FORCE_ASSERT(fromVector[10].AsCharacter() == 'Q');
	FORCE_ASSERT(fromVector[11].AsCharacter() == 'Q');
				 
	FORCE_ASSERT(fromVector[12].AsCharacter() == 'E');
	FORCE_ASSERT(fromVector[13].AsCharacter() == 'E');
				 
	FORCE_ASSERT(fromVector[14].AsCharacter() == 'G');
	FORCE_ASSERT(fromVector[15].AsCharacter() == 'G');
				 
	FORCE_ASSERT(fromVector[16].AsCharacter() == 'H');
	FORCE_ASSERT(fromVector[17].AsCharacter() == 'H');
				 
	FORCE_ASSERT(fromVector[18].AsCharacter() == 'I');
	FORCE_ASSERT(fromVector[19].AsCharacter() == 'I');
				 
	FORCE_ASSERT(fromVector[20].AsCharacter() == 'L');
	FORCE_ASSERT(fromVector[21].AsCharacter() == 'L');
				 
	FORCE_ASSERT(fromVector[22].AsCharacter() == 'K');
	FORCE_ASSERT(fromVector[23].AsCharacter() == 'K');
				 
	FORCE_ASSERT(fromVector[24].AsCharacter() == 'M');
	FORCE_ASSERT(fromVector[25].AsCharacter() == 'M');
				 
	FORCE_ASSERT(fromVector[26].AsCharacter() == 'F');
	FORCE_ASSERT(fromVector[27].AsCharacter() == 'F');
				 
	FORCE_ASSERT(fromVector[28].AsCharacter() == 'P');
	FORCE_ASSERT(fromVector[29].AsCharacter() == 'P');

	FORCE_ASSERT(fromVector[30].AsCharacter() == 'S');
	FORCE_ASSERT(fromVector[31].AsCharacter() == 'S');

	FORCE_ASSERT(fromVector[32].AsCharacter() == 'T');
	FORCE_ASSERT(fromVector[33].AsCharacter() == 'T');

	FORCE_ASSERT(fromVector[34].AsCharacter() == 'W');
	FORCE_ASSERT(fromVector[35].AsCharacter() == 'W');

	FORCE_ASSERT(fromVector[36].AsCharacter() == 'Y');
	FORCE_ASSERT(fromVector[37].AsCharacter() == 'Y');

	FORCE_ASSERT(fromVector[38].AsCharacter() == 'V');
	FORCE_ASSERT(fromVector[39].AsCharacter() == 'V');
	FORCE_ASSERT(fromVector[40].AsCharacter() == Bio::AminoAcidX::Invalid);

	PASS_TEST();
}

static void TestCodonTranslation()
{
	/* TODO: add tests */
	Bio::AminoAcid amino
	{ 
		TranslateTriplet
		(
			Bio::Rna{}.AssignCharacter('U'),
			Bio::Rna{}.AssignCharacter('U'),
			Bio::Rna{}.AssignCharacter('U')
		) 
	};

	FORCE_ASSERT(amino.AsCharacter() == 'F');
	PASS_TEST();
}

void TestPropertyCalculations()
{
	Bio::AminoSequence amino
	{
		"ARNDCQEGHILKMFPSTWYV"_Aminos
	};
	
	{
		const bool approximation
		{ 
			Approximate<1.0>
			(
				Bio::CalculateMolecularWeight(amino),
				2395.0
			) 
		};

		FORCE_ASSERT(approximation);
	}

	{
		const bool approximation
		{
			Approximate<0.25>
			(
				Bio::CalculateIsoelectricPoint(amino),
				7.17
			)
		};

		FORCE_ASSERT(approximation);
	}

	{
		const bool approximation1
		{
			Approximate<0.0>
			(
				Bio::CalculateNetCharge(amino, 7.0),
				0.0
			)
		};

		FORCE_ASSERT(approximation1);

		const bool approximation2
		{
			Approximate<0.0>
			(
				Bio::CalculateNetCharge(amino, 10.0),
				-2.0
			)
		};

		FORCE_ASSERT(approximation2);

		const bool approximation3
		{
			Approximate<0.0>
			(
				Bio::CalculateNetCharge(amino, 13.55),
				-5.0
			)
		};

		FORCE_ASSERT(approximation3);

		const bool approximation4
		{
			Approximate<0.0>
			(
				Bio::CalculateNetCharge(amino, 2.0),
				4.0
			)
		};

		FORCE_ASSERT(approximation4);
	}

	{
		auto copy = amino;
		copy.emplace_back(Bio::AminoAcid{}.AssignCharacter('C'));

		{
			const bool approximation
			{
				Approximate<0.0001>
				(
					(double)Bio::CalculateExtinctionCoefficient(copy),
					7115.0
				)
			};

			FORCE_ASSERT(approximation);
		}

		{
			const bool approximation
			{
				Approximate<0.0001>
				(
					(double)Bio::CalculateExtinctionCoefficientCysteinesReduced(copy),
					6990.0
				)
			};

			FORCE_ASSERT(approximation);
		}
	}

	PASS_TEST();
}
