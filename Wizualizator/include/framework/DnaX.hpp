#pragma once
#include "AlphabetBase.hpp"

namespace Bio {
	class DnaX final : public NucleotideAlphabetBase<DnaX, 5U>
	{
    private:
        using BaseType = NucleotideAlphabetBase<DnaX, 5U>;
        
        friend BaseType;
        friend BaseType::BaseType;
	public:
        using BaseType::BaseType;
		
        constexpr DnaX() noexcept                           = default;        
		constexpr DnaX(DnaX const&) noexcept                = default;        
		constexpr DnaX(DnaX&&) noexcept                     = default;                
		constexpr DnaX& operator=(DnaX const&) noexcept     = default;
		constexpr DnaX& operator=(DnaX&&) noexcept          = default;     
		~DnaX() noexcept                                    = default;

        static constexpr char Invalid{ 'X' };
    private:
        /* Adenine -> Cytosine -> Guanine -> Invalid (unknown) -> Thymine */
        static constexpr CharacterType StateToCharacterTable[s_AlphabetSize]{ 'A', 'C', 'G', Invalid, 'T' };
        
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
			using indexType = std::make_unsigned_t<CharacterType>;
			return CharacterToStateTable[static_cast<indexType>(character)];
        }
	};

	inline namespace literals
	{
		constexpr DnaX operator""_DnaX(const char character) noexcept
		{
		    return DnaX{}.AssignCharacter(character);
		}
	}
}