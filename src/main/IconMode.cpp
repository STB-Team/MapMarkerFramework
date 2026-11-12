#include "IconMode.h"
#include "MMF/IconModeAPI.h"

#include <array>
#include <atomic>
#include <mutex>

namespace IconMode
{
	namespace
	{
		struct Surface
		{
			RE::GPtr<RE::GFxMovieDef> owner;
			RE::GFxSpriteDef* sprite = nullptr;
			std::vector<Frame> frames;
		};

		std::array<Surface, 2> surfaces;
		std::mutex surfaceLock;
		std::atomic<bool> requested{ false };

		void Apply(Surface& a_surface, bool a_reveal)
		{
			for (const auto& frame : a_surface.frames) {
				a_surface.sprite->frames[frame.undiscovered] = a_reveal ? frame.icon : frame.question;
			}
		}

		// Goto the discovered frame first: gotoAndStop(currentFrame) alone need not
		// execute a replacement tag. No game marker discovery state is modified.
		std::uint32_t RefreshHUD(const Surface& a_surface)
		{
			if (a_surface.frames.empty()) {
				return 0;
			}
			const auto ui = RE::UI::GetSingleton();
			const auto hud = ui ? ui->GetMenu<RE::HUDMenu>() : nullptr;
			if (!hud || !hud->uiMovie) {
				return 0;
			}

			RE::GFxValue markers;
			// CNO aliases HUDMovieBaseInstance.CompassMarkerList as MarkerList.
			// The array belongs to the registered HUD root, not the Compass clip.
			if ((!hud->root.IsObject() && !hud->root.IsDisplayObject()) ||
				!hud->root.GetMember("CompassMarkerList", &markers) || !markers.IsArray()) {
				logger::warn("MarkerIcons: HUD compass marker list not found");
				return 0;
			}

			std::uint32_t refreshed = 0;
			for (std::uint32_t i = 0; i < markers.GetArraySize(); ++i) {
				RE::GFxValue entry, clip, current;
				if (!markers.GetElement(i, &entry) || !entry.IsObject() ||
					!entry.GetMember("movie", &clip) || !clip.IsDisplayObject() ||
					!clip.GetMember("_currentframe", &current) || !current.IsNumber()) {
					continue;
				}
				const auto frameNumber = current.GetNumber();
				const auto it = std::lower_bound(a_surface.frames.begin(), a_surface.frames.end(), frameNumber,
					[](const Frame& a_frame, double a_number) { return a_frame.undiscovered + 1.0 < a_number; });
				if (it == a_surface.frames.end() || it->undiscovered + 1.0 != frameNumber) {
					continue;
				}

				const RE::GFxValue discovered{ it->discovered + 1.0 };
				if (clip.Invoke("gotoAndStop", nullptr, &discovered, 1) &&
					clip.Invoke("gotoAndStop", nullptr, &current, 1)) {
					++refreshed;
				}
			}
			return refreshed;
		}
	}

	bool IsPrepared(MenuType a_menu, const RE::GFxSpriteDef* a_sprite)
	{
		std::scoped_lock lock(surfaceLock);
		return surfaces[static_cast<std::size_t>(a_menu)].sprite == a_sprite;
	}

	void Register(MenuType a_menu, RE::GFxMovieDef* a_owner, RE::GFxSpriteDef* a_sprite,
		std::vector<Frame> a_frames)
	{
		std::scoped_lock lock(surfaceLock);
		auto& surface = surfaces[static_cast<std::size_t>(a_menu)];
		// Keep the old owner alive until all its raw references have been replaced.
		auto previousOwner = std::move(surface.owner);
		surface.owner = RE::GPtr(a_owner);
		surface.sprite = a_sprite;
		surface.frames = std::move(a_frames);
		Apply(surface, requested.load());
		logger::debug("MarkerIcons: prepared {} frames for {}", surface.frames.size(),
			a_menu == MenuType::HUD ? "HUD" : "Map");
	}

	void Request(bool a_reveal)
	{
		requested.store(a_reveal);
		SKSE::GetTaskInterface()->AddUITask([] {
			std::scoped_lock lock(surfaceLock);
			const bool reveal = requested.load();
			for (auto& surface : surfaces) {
				Apply(surface, reveal);
			}
			const auto refreshed = RefreshHUD(surfaces[static_cast<std::size_t>(MenuType::HUD)]);
			logger::debug("MarkerIcons: mode={}, HUD refreshed={}", reveal ? "icons" : "questions", refreshed);
		});
	}
}

extern "C" DLLEXPORT const MMF::IconModeAPI* MMF_RequestIconModeAPI(std::uint32_t a_version)
{
	static const MMF::IconModeAPI api{ MMF::kIconModeVersion, IconMode::Request };
	return a_version == api.version ? &api : nullptr;
}
