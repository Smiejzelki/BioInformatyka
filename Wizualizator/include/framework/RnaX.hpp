#pragma once
#include "AlphabetBase.hpp"

namespace Bio {
	class RnaX final : public NucleotideAlphabetBase<RnaX, 5U>
	{
	private:
		using BaseType = NucleotideAlphabetBase<RnaX, 5U>;

		friend BaseType;
		friend BaseType::BaseType;
	public:
		using BaseType::BaseType;

		constexpr RnaX() noexcept							= default;
		constexpr RnaX(RnaX const&) noexcept				= default;
		constexpr RnaX(RnaX&&) noexcept						= default;
		constexpr RnaX& operator=(RnaX const&) noexcept		= default;
		constexpr RnaX& operator=(RnaX&&) noexcept			= default;
		~RnaX() noexcept									= default;

		static constexpr char Invalid{ 'X' };
	private:
		/* Adenine -> Cytosine -> Guanine -> Invalid (unknown) -> Uracil */
		static constexpr CharacterType StateToCharacterTable[s_AlphabetSize]{ 'A', 'C', 'G', Invalid, 'U' };

		static constexpr std::array<StateType, 256U> CharacterToStateTable
		{
			[]() constexpr
			{
				std::array<StateType, 256U> returnValue{};
				returnValue.fill(3U); /* Fill buckets with invalid state */

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
			using indexType = std::make_unsigned_t<CharacterType>;
			return CharacterToStateTable[static_cast<indexType>(character)];
		}
	};

	inline namespace literals
	{
		constexpr RnaX operator""_RnaX(const char character) noexcept
		{
			return RnaX{}.AssignCharacter(character);
		}
	}
}