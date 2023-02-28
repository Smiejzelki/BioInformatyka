#pragma once
#include "AlphabetBase.hpp"

namespace Bio {
	class AminoAcidX final : public AlphabetBase<AminoAcidX, 22U>
	{
	private:
		using BaseType = AlphabetBase<AminoAcidX, 22U>;
		friend BaseType;
	public:
		constexpr AminoAcidX() noexcept = default;
		constexpr AminoAcidX(AminoAcidX const&) noexcept = default;
		constexpr AminoAcidX(AminoAcidX&&) noexcept = default;
		constexpr AminoAcidX& operator=(AminoAcidX const&) noexcept = default;
		constexpr AminoAcidX& operator=(AminoAcidX&&) noexcept = default;
		~AminoAcidX() noexcept = default;

		static constexpr char Invalid{ 'X' };
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

			Invalid,
		};

		static constexpr std::array<StateType, 256U> CharacterToStateTable
		{
			[]() constexpr
			{
				std::array<StateType, 256U> returnValue{};
				returnValue.fill(21U); /* Fill buckets with invalid state */

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
			using indexType = std::make_unsigned_t<CharacterType>;
			return CharacterToStateTable[static_cast<indexType>(character)];
		}
	};
}