#pragma once
#include "AlphabetBase.hpp"

namespace Bio {
	struct CarbonElementCount 
	{ 
		size_t Count; 
		
		constexpr CarbonElementCount operator=(const size_t count) noexcept
		{
			Count = count;
			return *this;
		}

		constexpr operator size_t() const noexcept
		{
			return Count;
		}
	};
	
	struct HydrogenElementCount 
	{ 
		size_t Count; 

		constexpr HydrogenElementCount operator=(const size_t count) noexcept
		{
			Count = count;
			return *this;
		}

		constexpr operator size_t() const noexcept
		{
			return Count;
		}
	};


	struct NitrogenElementCount 
	{ 
		size_t Count; 

		constexpr NitrogenElementCount operator=(const size_t count) noexcept
		{
			Count = count;
			return *this;
		}

		constexpr operator size_t() const noexcept
		{
			return Count;
		}
	};
	
	struct OxygenElementCount 
	{
		size_t Count;

		constexpr OxygenElementCount operator=(const size_t count) noexcept
		{
			Count = count;
			return *this;
		}

		constexpr operator size_t() const noexcept
		{
			return Count;
		}
	};

	struct SulphurElementCount 
	{ 
		size_t Count; 

		constexpr SulphurElementCount operator=(const size_t count) noexcept
		{
			Count = count;
			return *this;
		}

		constexpr operator size_t() const noexcept
		{
			return Count;
		}
	};

	template<typename Derived, size_t size, typename CharacterT = char>
	struct FormulaBase
	{
	protected:
		using CharacterType = CharacterT;
		using StateType = DeduceViableContainer<size>;
	public:
		constexpr FormulaBase() noexcept = default;
		constexpr FormulaBase(FormulaBase const&) noexcept = default;
		constexpr FormulaBase(FormulaBase&&) noexcept = default;
		constexpr FormulaBase& operator=(FormulaBase const&) noexcept = default;
		constexpr FormulaBase& operator=(FormulaBase&&) noexcept = default;
		~FormulaBase() noexcept = default;

		static constexpr size_t s_AlphabetSize{ size };
	private:
		friend Derived;
		using ThisType = Derived;
	};

	struct PeptideFormula final : public FormulaBase<PeptideFormula, 21U>
	{
		using FormulaBase<PeptideFormula, 21U>::ThisType;

		CarbonElementCount		Carbon;
		HydrogenElementCount	Hydrogen;
		NitrogenElementCount	Nitrogen;
		OxygenElementCount		Oxygen;
		SulphurElementCount		Sulphur;

		constexpr PeptideFormula() noexcept
			:
			Carbon(CarbonElementCount(0U)),
			Hydrogen(HydrogenElementCount(0U)),
			Nitrogen(NitrogenElementCount(0U)),
			Oxygen(OxygenElementCount(0U)),
			Sulphur(SulphurElementCount(0U))
		{}

		constexpr explicit PeptideFormula(
			const CarbonElementCount carbon,
			const HydrogenElementCount hydrogen,
			const NitrogenElementCount nitrogen,
			const OxygenElementCount oxygen,
			const SulphurElementCount sulphur) noexcept
			:
			Carbon(carbon),
			Hydrogen(hydrogen),
			Nitrogen(nitrogen),
			Oxygen(oxygen),
			Sulphur(sulphur)
		{}

		constexpr PeptideFormula& operator+=(const PeptideFormula& other)
		{
			Carbon.Count += other.Carbon.Count;
			Hydrogen.Count += other.Hydrogen.Count;
			Nitrogen.Count += other.Nitrogen.Count;
			Oxygen.Count += other.Oxygen.Count;
			Sulphur.Count += other.Sulphur.Count;

			Hydrogen.Count -= 2U;
			Oxygen.Count -= 1U;

			return *this;
		}
	};

	namespace literals::elements
	{
		constexpr CarbonElementCount operator""_C(const size_t count) noexcept
		{
			return { count };
		}

		constexpr HydrogenElementCount operator""_H(const size_t count) noexcept
		{
			return { count };
		}

		constexpr NitrogenElementCount operator""_N(const size_t count) noexcept
		{
			return { count };
		}

		constexpr OxygenElementCount operator""_O(const size_t count) noexcept
		{
			return { count };
		}

		constexpr SulphurElementCount operator""_S(const size_t count) noexcept
		{
			return { count };
		}
	}

	constexpr std::string BakePeptideFormula(const PeptideFormula& formula) noexcept
	{
		std::string result{};

		if (formula.Carbon.Count)
		{
			result += 'C';
			if (formula.Carbon.Count > 1U)
				result += std::to_string(formula.Carbon.Count);
		}

		if (formula.Hydrogen.Count)
		{
			result += 'H';
			if (formula.Hydrogen.Count > 1U)
				result += std::to_string(formula.Hydrogen.Count);
		}

		if (formula.Nitrogen.Count)
		{
			result += 'N';
			if (formula.Nitrogen.Count > 1U)
				result += std::to_string(formula.Nitrogen.Count);
		}

		if (formula.Oxygen.Count)
		{
			result += 'O';
			if (formula.Oxygen.Count > 1U)
				result += std::to_string(formula.Oxygen.Count);
		}

		if (formula.Sulphur.Count)
		{
			result += 'S';
			if (formula.Sulphur.Count > 1U)
				result += std::to_string(formula.Sulphur.Count);
		}

		return result;
	}

	struct AminoChainMolecularWeight 
	{ 
		double Value{ 0.0 };

		constexpr AminoChainMolecularWeight& operator=(const double value) noexcept
		{
			Value = value;
			return *this;
		}

		constexpr AminoChainMolecularWeight& operator+=(const AminoChainMolecularWeight  other) noexcept
		{
			Value += other.Value;
			/* Subtract 2 hydrogens + oxygen molecular weight */
			Value -= 18.02;
			return *this;
		}

		constexpr operator double() const noexcept
		{
			return Value;
		}
	};

	namespace literals::properties
	{
		constexpr AminoChainMolecularWeight  operator""_gMol(const long double value) noexcept
		{
			return { static_cast<double>(value) };
		}
	}

	constexpr AminoChainMolecularWeight GetAminoAcidMolecularWeight(const char character) noexcept
	{
		// Based on https://pubchem.ncbi.nlm.nih.gov/
		using namespace literals::properties;

		switch (character)
		{
			case '-': return { 0.0_gMol }; 
			case 'V': return { 117.14634_gMol };
			case 'A': return { 89.09318_gMol };
			case 'D': return { 133.10268_gMol };
			case 'E': return { 147.12926_gMol };
			case 'G': return { 75.0666_gMol };
			case 'F': return { 165.18914_gMol };
			case 'L': return { 131.17292_gMol };
			case 'S': return { 105.09258_gMol };
			case 'Y': return { 181.18854_gMol };
			case 'C': return { 121.15818_gMol };
			case 'W': return { 204.22518_gMol };
			case 'P': return { 115.13046_gMol };
			case 'H': return { 155.15456_gMol };
			case 'Q': return { 146.1445_gMol };
			case 'R': return { 174.20096_gMol };
			case 'I': return { 131.17292_gMol };
			case 'M': return { 149.21134_gMol };
			case 'T': return { 119.11916_gMol };
			case 'N': return { 132.11792_gMol };
			case 'K': return { 146.18756_gMol };

			default: break;
		}

		assert(false);
		return 0.0_gMol;
	}

	constexpr PeptideFormula GetAminoAcidFormula(const char character) noexcept
	{
		// Based on https://pubchem.ncbi.nlm.nih.gov/
		using namespace literals::elements;

		switch (character)
		{
			case '-': return PeptideFormula{ 0_C, 0_H, 0_N, 0_O, 0_S };
			case 'V': return PeptideFormula{ 5_C, 11_H, 1_N, 2_O, 0_S };
			case 'A': return PeptideFormula{ 3_C, 7_H, 1_N, 2_O, 0_S };
			case 'D': return PeptideFormula{ 4_C, 7_H, 1_N, 4_O, 0_S };
			case 'E': return PeptideFormula{ 5_C, 9_H, 1_N, 4_O, 0_S };
			case 'G': return PeptideFormula{ 2_C, 5_H, 1_N, 2_O, 0_S };
			case 'F': return PeptideFormula{ 9_C, 11_H, 1_N, 2_O, 0_S };
			case 'L': return PeptideFormula{ 6_C, 13_H, 1_N, 2_O, 0_S };
			case 'S': return PeptideFormula{ 3_C, 7_H, 1_N, 3_O, 0_S };
			case 'Y': return PeptideFormula{ 9_C, 11_H, 1_N, 3_O, 0_S };
			case 'C': return PeptideFormula{ 3_C, 7_H, 1_N, 2_O, 1_S };
			case 'W': return PeptideFormula{ 11_C, 12_H, 2_N, 2_O, 0_S };
			case 'P': return PeptideFormula{ 5_C, 9_H, 1_N, 2_O, 0_S };
			case 'H': return PeptideFormula{ 6_C, 9_H, 3_N, 2_O, 0_S };
			case 'Q': return PeptideFormula{ 5_C, 10_H, 2_N, 3_O, 0_S };
			case 'R': return PeptideFormula{ 6_C, 14_H, 4_N, 2_O, 0_S };
			case 'I': return PeptideFormula{ 6_C, 13_H, 1_N, 2_O, 0_S };
			case 'M': return PeptideFormula{ 5_C, 11_H, 1_N, 2_O, 1_S };
			case 'T': return PeptideFormula{ 4_C, 9_H, 1_N, 3_O, 0_S };
			case 'N': return PeptideFormula{ 4_C, 8_H, 2_N, 3_O, 0_S };
			case 'K': return PeptideFormula{ 6_C, 14_H, 2_N, 2_O, 0_S };

			default: break;
		}

		assert(false);
		return PeptideFormula{ 0_C, 0_H, 0_N, 0_O, 0_S };
	}
}