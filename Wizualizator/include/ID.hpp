#pragma once

enum class EFrame
{
	Frame1,
	Frame2,
	Frame3,
};

typedef size_t ID;
static constexpr ID g_InvalidID = std::numeric_limits<ID>::max();

struct IDWrapper
{
	ID m_UUID;

	constexpr IDWrapper() noexcept
		:
		m_UUID(g_InvalidID)
	{}

	constexpr IDWrapper(const ID uuid) noexcept
		:
		m_UUID(uuid)
	{}

	IDWrapper& operator=(const ID uuid) noexcept
	{
		m_UUID = uuid;
		return *this;
	}

	operator EFrame() const noexcept
	{
		return static_cast<EFrame>(static_cast<size_t>(m_UUID));
	}

	operator bool() const noexcept
	{
		return m_UUID != g_InvalidID;
	}

	operator ID() const noexcept
	{
		return m_UUID;
	}

	bool operator==(const ID uuid) const noexcept
	{
		return m_UUID == uuid;
	}

	bool operator!=(const ID uuid) const noexcept
	{
		return m_UUID != uuid;
	}
};
