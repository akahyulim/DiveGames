#pragma once

namespace Dive
{
	class UUID
	{
	public:
		UUID();
		UUID(UINT64 uuid);
		UUID(const UUID&) = default;

		operator UINT64() const { return m_UUID; }

	private:
		UINT64 m_UUID;
	};
}

namespace std
{
	template<typename T> struct hash;

	template<>
	struct hash<Dive::UUID>
	{
		std::size_t operator()(const Dive::UUID& uuid) const
		{
			return static_cast<size_t>((UINT64)uuid);
		}
	};
}
