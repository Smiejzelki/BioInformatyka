#pragma once
#include "AlphabetBase.hpp"

namespace Bio {
	class Rna final : public NucleotideAlphabetBase<Rna, 4U>
	{
	private:
		using BaseType = NucleotideAlphabetBase<Rna, 4U>;

		friend BaseType;
		friend BaseType::BaseType;
	public:
		using BaseType::BaseType;

		constexpr Rna() noexcept = default;
		constexpr Rna(Rna const&) noexcept = default;
		constexpr Rna(Rna&&) noexcept = default;
		constexpr Rna& operator=(Rna const&) noexcept = default;
		constexpr Rna& operator=(Rna&&) noexcept = default;
		~Rna() noexcept = default;
	private:
		/* Adenine -> Cytosine -> Guanine -> Uracil */
		static constexpr CharacterType StateToCharacterTable[s_AlphabetSize]{ 'A', 'C', 'G', 'U' };

		static constexpr std::array<StateType, 256U> CharacterToStateTable
		{
			[]() constexpr
			{
				std::array<StateType, 256U> returnValue{};
				returnValue.fill(3U);

				for (StateType state{ 0U }; state < s_AlphabetSize; ++state)
				{
					returnValue[StateToCharacterTable[state]] = state;
					returnValue[ToLower(StateToCharacterTable[state])] = state;
				}

				returnValue['t'] = returnValue['T'] = returnValue['U'];
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
			assert
			(
				character == 'A' || character == 'C' || character == 'G' || character == 'T' || character == 'U' ||
				character == 'a' || character == 'c' || character == 'g' || character == 't' || character == 'u'
			);

			using indexType = std::make_unsigned_t<CharacterType>;
			return CharacterToStateTable[static_cast<indexType>(character)];
		}
	};

	inline namespace literals
	{
		constexpr Rna operator""_Rna(const char character) noexcept
		{
			return Rna{}.AssignCharacter(character);
		}
	}
}