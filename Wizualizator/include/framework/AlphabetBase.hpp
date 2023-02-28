#pragma once
#include <limits>
#include <stdint.h>
#include <array>
#include <vector>
#include <assert.h>
#include <algorithm>
#include <string>
#include <unordered_map>
#ifdef max
#undef max
#endif

namespace Bio {
	template <typename int_t> requires std::is_integral_v<int_t>
	constexpr size_t size_in_values_v = static_cast<size_t>(std::numeric_limits<int_t>::max()) - std::numeric_limits<int_t>::lowest() + 1;

	constexpr std::array<char, size_in_values_v<char>> ToLowerTable
	{
		[]() constexpr
		{
			std::array<char, size_in_values_v<char>> ret{};

			for (size_t i = 0; i < size_in_values_v<char>; ++i)
				ret[i] = static_cast<char>(i);

			for (auto i = char{'A'}; i <= char{'Z'}; ++i)
				ret[i] = ret[i] - char{'A'} + char{'a'};

			return ret;
		}()
	};

	constexpr char ToLower(const char character) noexcept
	{
		return ToLowerTable[character];
	}

	template <uint64_t value>
	using DeduceViableContainer =
		std::conditional_t<value == 1ULL, bool,
		std::conditional_t<value <= std::numeric_limits<uint8_t>::max(), uint8_t,
		std::conditional_t<value <= std::numeric_limits<uint16_t>::max(), uint16_t,
		std::conditional_t<value <= std::numeric_limits<uint32_t>::max(), uint32_t, uint64_t>>>>;

	template<typename Derived, size_t size, typename CharacterT = char>
	class AlphabetBase
	{
	protected:
		using StateType = DeduceViableContainer<size>;
		using CharacterType = CharacterT;
	public:
		constexpr AlphabetBase() noexcept = default;
		constexpr AlphabetBase(AlphabetBase const&) noexcept = default;
		constexpr AlphabetBase(AlphabetBase&&) noexcept = default;
		constexpr AlphabetBase& operator=(AlphabetBase const&) noexcept = default;
		constexpr AlphabetBase& operator=(AlphabetBase&&) noexcept = default;
		~AlphabetBase() noexcept = default;

		constexpr CharacterType AsCharacter() const noexcept
		{
			return Derived::StateToCharacter(m_State);
		}

		constexpr StateType AsState() const noexcept
		{
			return m_State;
		}

		constexpr Derived& AssignCharacter(const CharacterType character) noexcept
		{
			m_State = Derived::CharacterToState(character);
			return static_cast<Derived&>(*this);
		}

		constexpr Derived& AssignState(const StateType state) noexcept
		{
			assert(static_cast<size_t>(state) < size);
			m_State = state;
			return static_cast<Derived&>(*this);
		}

		static constexpr size_t s_AlphabetSize{ size };
	private:
		StateType m_State;
	protected:
		static_assert(size > 0U, "Invalid alphabet size!");
	};

	template<typename Derived, auto size> requires std::is_integral_v<decltype(size)>
	class NucleotideAlphabetBase : public AlphabetBase<Derived, size, char>
	{
	private:
		using BaseType = AlphabetBase<Derived, size, char>;

		constexpr NucleotideAlphabetBase() noexcept = default;
		constexpr NucleotideAlphabetBase(NucleotideAlphabetBase const&) noexcept = default;
		constexpr NucleotideAlphabetBase(NucleotideAlphabetBase&&) noexcept = default;
		constexpr NucleotideAlphabetBase& operator=(NucleotideAlphabetBase const&) noexcept = default;
		constexpr NucleotideAlphabetBase& operator=(NucleotideAlphabetBase&&) noexcept = default;
		~NucleotideAlphabetBase() noexcept = default;

		friend Derived;
	protected:
		using typename BaseType::CharacterType;
		using typename BaseType::StateType;
	public:
		using BaseType::s_AlphabetSize;
		using BaseType::AsState;
	};
}