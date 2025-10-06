#pragma once
#include <xhash>
#include <format>

namespace Teddy {
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;
		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};
}


namespace std {
	template<>
	struct hash<Teddy::UUID>
	{
		std::size_t operator()(const Teddy::UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};

	template<>
	struct formatter<Teddy::UUID> : formatter<uint64_t> {
		auto format(Teddy::UUID uuid, format_context& ctx) const {
			return formatter<uint64_t>::format(static_cast<uint64_t>(uuid), ctx);
		}
	};
}