#pragma once

namespace Dive
{
	class InstanceID
	{
	public:
		InstanceID();
		InstanceID(UINT64 instanceID);
		InstanceID(const InstanceID&) = default;

		operator UINT64() const { return m_InstanceID; }

	private:
		UINT64 m_InstanceID;
	};
}

namespace std
{
	template<typename T> struct hash;

	template<>
	struct hash<Dive::InstanceID>
	{
		std::size_t operator()(const Dive::InstanceID& instanceID) const
		{
			return static_cast<size_t>((UINT64)instanceID);
		}
	};
}
