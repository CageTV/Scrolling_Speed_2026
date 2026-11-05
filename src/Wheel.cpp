/*
 * Scrolling Speed 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the GNU General Public License, version 3 or (at your option) any later version; see LICENSE.txt.
 */
#include "PCH.h"
#include "Wheel.h"
#include "Config.h"
#include "Speed.h"
#include "Keys.h"

#include <RE/I/INISettingCollection.h>

// The wheel is read from the raw input device events (so it does not matter which camera plugin - TFCam, SmoothCam, ImprovedCamera - sits on the
// camera states). Zoom is switched off the way the game's own INI does it: fMouseWheelZoomIncrement:Camera = 0 while the wheel belongs to the speed,
// the saved original value is put back as soon as the wheel is for zoom again. No memory patch, no camera state is touched.
namespace Wheel
{
	namespace
	{
		float       savedIncrement = -1.0f;  // the game's value before this plugin zeroed it
		bool        blocking = false;

		RE::Setting* ZoomSetting()
		{
			auto* ini = RE::INISettingCollection::GetSingleton();
			return ini ? ini->GetSetting("fMouseWheelZoomIncrement:Camera") : nullptr;
		}

		void SetBlock(bool a_block)
		{
			if (a_block == blocking) {
				return;
			}
			auto* s = ZoomSetting();
			if (!s) {
				return;
			}
			if (a_block) {
				savedIncrement = s->data.f;
				s->data.f = 0.0f;
			} else if (savedIncrement >= 0.0f) {
				s->data.f = savedIncrement;
			}
			blocking = a_block;
			SKSE::log::info("wheel zoom {} (fMouseWheelZoomIncrement {})", a_block ? "blocked" : "restored", s->data.f);
		}

		// should the wheel mean "speed" right now?
		bool SpeedMode()
		{
			const auto& c = Config::Get();
			if (!c.enabled) {
				return false;
			}
			if (c.zoomKey > 0 && Keys::ModifierHeld(c.zoomKey)) {
				return false;  // the optional zoom key: the wheel zooms, whatever the other modifier says
			}
			const bool held = c.modifierKey > 0 && Keys::ModifierHeld(c.modifierKey);
			return c.wheelChangesSpeed ? !held : held;
		}

		// a camera or UI state that owns the wheel: leave it alone
		bool WheelIsSomeoneElses()
		{
			auto* ui = RE::UI::GetSingleton();
			if (ui) {
				if (ui->GameIsPaused()) {
					return true;
				}
				for (const auto& m : Config::Get().blockedMenus) {
					if (ui->IsMenuOpen(m)) {
						return true;
					}
				}
				// any menu that shows the cursor or pauses the game (dialogue, containers, books, mod menus ...) owns the wheel
				for (auto& [name, entry] : ui->menuMap) {
					if (!entry.menu || !ui->IsMenuOpen(name.c_str())) {
						continue;
					}
					const std::string_view n = name.c_str();
					if (n == "Cursor Menu" || n == "HUD Menu") {
						continue;
					}
					if (entry.menu->UsesCursor() || entry.menu->PausesGame()) {
						return true;
					}
				}
			}
			// a conversation is running (a dialogue replacer or camera mod may hide the standard menu)
			if (auto* topics = RE::MenuTopicManager::GetSingleton(); topics && topics->speaker.get()) {
				return true;
			}
			auto* cam = RE::PlayerCamera::GetSingleton();
			if (cam && cam->IsInFreeCameraMode()) {  // TFCam's free camera uses the wheel itself
				return true;
			}
			return false;
		}

		struct Sink : RE::BSTEventSink<RE::InputEvent*>
		{
			RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_events, RE::BSTEventSource<RE::InputEvent*>*) override
			{
				if (!a_events) {
					return RE::BSEventNotifyControl::kContinue;
				}
				try {
					const bool mine = SpeedMode() && !WheelIsSomeoneElses();
					SetBlock(mine);  // follows the modifier key at once, before the wheel event itself is processed
					if (!mine) {
						return RE::BSEventNotifyControl::kContinue;
					}
					for (auto* e = *a_events; e; e = e->next) {
						const auto* b = e->AsButtonEvent();
						if (b && b->GetDevice() == RE::INPUT_DEVICE::kKeyboard && b->IsDown() && !WheelIsSomeoneElses()) {
							const auto& c = Config::Get();
							const int   code = static_cast<int>(b->GetIDCode());
							if (c.enabled && c.keyFaster > 0 && code == c.keyFaster) {
								Speed::ChangeLevel(+1);
							} else if (c.enabled && c.keySlower > 0 && code == c.keySlower) {
								Speed::ChangeLevel(-1);
							}
							continue;
						}
						if (!b || b->GetDevice() != RE::INPUT_DEVICE::kMouse || b->Value() <= 0.0f) {
							continue;
						}
						if (b->GetIDCode() == RE::BSWin32MouseDevice::Keys::kWheelUp) {
							Speed::ChangeLevel(+1);
						} else if (b->GetIDCode() == RE::BSWin32MouseDevice::Keys::kWheelDown) {
							Speed::ChangeLevel(-1);
						}
					}
				} catch (const std::exception& ex) {
					SKSE::log::error("wheel sink: {}", ex.what());
				}
				return RE::BSEventNotifyControl::kContinue;
			}
		};
		Sink sink;
	}

	void Release() { SetBlock(false); }

	void Init()
	{
		if (auto* input = RE::BSInputDeviceManager::GetSingleton()) {
			input->AddEventSink(&sink);
			SKSE::log::info("wheel: input sink installed");
		}
	}
}
