#pragma once

#include "ImportData.h"

namespace IconMode
{
	struct Frame
	{
		std::uint32_t discovered;
		std::uint32_t undiscovered;
		RE::GFxTimelineDef::ExecuteTagList question;
		RE::GFxTimelineDef::ExecuteTagList icon;
	};

	// The owning movie keeps the sprite, its tag allocator and imports alive.
	// At most one HUD and one map resource set are retained, never one per map opening.
	void Register(MenuType a_menu, RE::GFxMovieDef* a_owner, RE::GFxSpriteDef* a_sprite,
		std::vector<Frame> a_frames);
	void Request(bool a_reveal);
	bool IsPrepared(MenuType a_menu, const RE::GFxSpriteDef* a_sprite);
}
