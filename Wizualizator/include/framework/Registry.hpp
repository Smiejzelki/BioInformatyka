#pragma once

typedef size_t ID;

namespace Bio {	
	template<typename SequenceTypes>
	class SequenceRegistry
	{
	private:
		using Types = SequenceTypes;
	public:
		SequenceRegistry() noexcept = default;
		~SequenceRegistry() noexcept = default;

		template<typename Sequence, typename ... Args>
		SequenceTypes& Register(const std::string_view name) noexcept
		{
			const ID uuid{ GenerateUUID() };
			m_SequenceRegitry[uuid] = Sequence{};	
			m_Names[uuid] = name;

			return m_SequenceRegitry[uuid];
		}

		void Unregister(const ID uuid) noexcept
		{
			BIO_ASSERT(m_SequenceRegitry.contains(uuid));
			m_SequenceRegitry.erase(uuid);
		}

		SequenceTypes& Find(const ID uuid)
		{
			BIO_ASSERT(m_SequenceRegitry.contains(uuid));
			return m_SequenceRegitry[uuid];
		}

		std::string& FindName(const ID uuid)
		{
			BIO_ASSERT(m_Names.contains(uuid));
			return m_Names[uuid];
		}

		std::unordered_map<ID, SequenceTypes>::iterator begin() noexcept
		{
			return m_SequenceRegitry.begin();
		}

		std::unordered_map<ID, SequenceTypes>::iterator end() noexcept
		{
			return m_SequenceRegitry.end();
		}
	private:
		ID GenerateUUID() noexcept
		{
			static constinit ID f_Counter{ 0U };
			return ++f_Counter;
		}
	private:
		std::unordered_map<ID, SequenceTypes> m_SequenceRegitry;
		std::unordered_map<ID, std::string> m_Names;
	};
}