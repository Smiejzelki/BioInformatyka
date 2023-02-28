#pragma once
#include "AlphabetBase.hpp"

namespace Bio {
	enum class EAminoAcid : uint8_t
	{
		STOP = 0,	// STOP
		V,			// VAL
		A,			// ALA
		D,			// ASP
		E,			// GLU
		G,			// GLY
		F,			// PHE
		L,			// LEU
		S,			// SER
		Y,			// TYR
		C,			// CYS
		W,			// TRP
		P,			// PRO
		H,			// HIS
		Q,			// GLN
		R,			// ARG
		I,			// ILE
		M,			// MET (START)
		T,			// THR
		N,			// ASN
		K,			// LYS 		
	};

	class AminoAcid final : public AlphabetBase<AminoAcid, 21U>
	{
	private:
		using BaseType = AlphabetBase<AminoAcid, 21U>;

		friend BaseType;
	public:
		constexpr AminoAcid() noexcept = default;
		constexpr AminoAcid(AminoAcid const&) noexcept = default;
		constexpr AminoAcid(AminoAcid&&) noexcept = default;
		constexpr AminoAcid& operator=(AminoAcid const&) noexcept = default;
		constexpr AminoAcid& operator=(AminoAcid&&) noexcept = default;
		~AminoAcid() noexcept = default;
	private:
		static constexpr CharacterType StateToCharacterTable[s_AlphabetSize]
		{
			'-', // STOP

			'V', // VAL
			'A', // ALA
			'D', // ASP
			'E', // GLU
			'G', // GLY

			'F', // PHE
			'L', // LEU
			'S', // SER
			'Y', // TYR
			'C', // CYS
			'W', // TRP

			'P', // PRO
			'H', // HIS
			'Q', // GLN
			'R', // ARG

			'I', // ILE
			'M', // MET (START)
			'T', // THR
			'N', // ASN
			'K', // LYS 		
		};

		static constexpr std::array<StateType, 256U> CharacterToStateTable
		{
			[]() constexpr
			{
				std::array<StateType, 256U> returnValue{};

				for (StateType state{ 0U }; state < s_AlphabetSize; ++state)
				{
					returnValue[StateToCharacterTable[state]] = state;
					returnValue[ToLower(StateToCharacterTable[state])] = state;
				}

				return returnValue;
			}()
		};
	private:
		static constexpr CharacterType StateToCharacter(const StateType type) noexcept
		{
			return StateToCharacterTable[static_cast<size_t>(type)];
		}

		static constexpr StateType CharacterToState(const CharacterType character) noexcept
		{
#ifdef _DEBUG
			bool isPartOfAlphabet{ false };

			for (uint32_t i{ 0U }; i < s_AlphabetSize; ++i)
				if (StateToCharacterTable[i] == character || tolower(StateToCharacterTable[i]) == character)
					isPartOfAlphabet = true;

			assert(isPartOfAlphabet);
#endif
			using indexType = std::make_unsigned_t<CharacterType>;
			return CharacterToStateTable[static_cast<indexType>(character)];
		}
	public:
		constexpr operator EAminoAcid() const noexcept
		{
			return static_cast<EAminoAcid>(AsState());
		}
	};
}