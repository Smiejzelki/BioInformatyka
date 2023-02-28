#pragma once
#include "AlphabetBase.hpp"

namespace Bio {
	class Dna final : public NucleotideAlphabetBase<Dna, 4U>
	{
	private:
		using BaseType = NucleotideAlphabetBase<Dna, 4U>;

		friend BaseType;
		friend BaseType::BaseType;
	public:
		using BaseType::BaseType;

		constexpr Dna() noexcept = default;
		constexpr Dna(Dna const&) noexcept = default;
		constexpr Dna(Dna&&) noexcept = default;
		constexpr Dna& operator=(Dna const&) noexcept = default;
		constexpr Dna& operator=(Dna&&) noexcept = default;
		~Dna() noexcept = default;

		constexpr Dna Complement() const noexcept
		{
			return Dna{}.AssignState(AsState() ^ 0b11);
		}
	private:
		/* Adenine -> Cytosine -> Guanine -> Thymine */
		static constexpr CharacterType StateToCharacterTable[s_AlphabetSize]{ 'A', 'C', 'G', 'T' };

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

				returnValue['u'] = returnValue['U'] = returnValue['T'];
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
		constexpr Dna operator""_Dna(const char character) noexcept
		{
			return Dna{}.AssignCharacter(character);
		}
	}
}