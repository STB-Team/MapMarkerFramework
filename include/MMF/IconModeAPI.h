#pragma once

#include <cstdint>

namespace MMF
{
	// Independent extension: the original read-only MapMarkerInterface ABI is unchanged.
	struct IconModeAPI
	{
		std::uint32_t version;
		// Thread-safe request; applied on the Scaleform UI queue. Does not change discovery flags.
		void (*SetUndiscoveredIcons)(bool a_reveal);
	};

	inline constexpr std::uint32_t kIconModeVersion = 1;
	using RequestIconModeAPI = const IconModeAPI* (*)(std::uint32_t);
	inline constexpr auto kRequestIconModeAPI = "MMF_RequestIconModeAPI";
}
