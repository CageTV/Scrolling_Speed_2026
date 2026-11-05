/*
 * Key picker widget for SKSE Menu Framework pages (Scrolling Speed 2026, Beast Harmony)
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 *
 * Keys are DirectInput scan codes (what Skyrim and these plugins' config files use). "Press a key" waits for the next keyboard key
 * (Esc cancels); a key the game's own controls already use is flagged by name. Keys another MOD uses cannot be known.
 */
#pragma once

#include "SKSEMenuFramework.h"

#include <atomic>
#include <string>

namespace KeyPicker
{
	namespace detail
	{
		inline std::atomic<bool> capturing{ false };
		inline std::atomic<int>  captured{ -1 };
		inline const int*        target = nullptr;

		inline bool __stdcall OnInput(RE::InputEvent* a_event)
		{
			if (!capturing.load()) {
				return false;
			}
			for (auto* e = a_event; e; e = e->next) {
				const auto* b = e->AsButtonEvent();
				if (!b || !b->IsDown() || b->GetDevice() != RE::INPUT_DEVICE::kKeyboard) {
					continue;
				}
				const int code = static_cast<int>(b->GetIDCode());
				captured.store(code == 0x01 ? -2 : code);  // Esc = cancel (keeps the old key)
				capturing.store(false);
				return true;  // the key press is used up by the picker
			}
			return false;
		}
	}

	inline void Install() { SKSEMenuFramework::AddInputEvent(detail::OnInput); }

	inline bool IsCapturing() { return detail::capturing.load(); }

	// readable key name from the scan code (extended keys carry 0x80)
	inline std::string KeyName(int a_dik)
	{
		if (a_dik <= 0) {
			return "Unset";
		}
		LONG lparam = static_cast<LONG>((a_dik & 0x7F) << 16);
		if (a_dik & 0x80) {
			lparam |= 1 << 24;
		}
		char buf[64]{};
		if (GetKeyNameTextA(lparam, buf, sizeof(buf)) > 0) {
			return buf;
		}
		return std::format("key {}", a_dik);
	}

	// name of the vanilla game control that uses this key (empty = none found)
	inline std::string Conflict(int a_dik)
	{
		if (a_dik <= 0) {
			return {};
		}
		auto* cm = RE::ControlMap::GetSingleton();
		if (!cm) {
			return {};
		}
		for (const auto ctx : { RE::ControlMap::InputContextID::kGameplay, RE::ControlMap::InputContextID::kFavorites }) {
			const auto name = cm->GetUserEventName(static_cast<std::uint32_t>(a_dik), RE::INPUT_DEVICE::kKeyboard, ctx);
			if (!name.empty()) {
				return std::string(name);
			}
		}
		return {};
	}

	// "Label: [key] [Unmap]"  -> true when the key changed
	inline bool Field(const char* a_id, const char* a_label, int* a_key)
	{
		bool changed = false;
		const bool mine = detail::target == a_key;
		if (mine && !detail::capturing.load()) {
			const int got = detail::captured.exchange(-1);
			if (got >= 0) {
				*a_key = got;
				changed = true;
			}
			if (got != -1) {
				detail::target = nullptr;
			}
		}
		const bool waiting = detail::target == a_key && detail::capturing.load();

		ImGuiMCP::Text("%s", a_label);
		ImGuiMCP::SameLine();
		const std::string btn = (waiting ? std::string("Press a key...") : KeyName(*a_key)) + "##" + a_id;
		if (ImGuiMCP::Button(btn.c_str())) {
			detail::captured.store(-1);
			detail::target = a_key;
			detail::capturing.store(!waiting);
			if (waiting) {
				detail::target = nullptr;
			}
		}
		ImGuiMCP::SameLine();
		const std::string un = std::string("Unmap##") + a_id;
		if (ImGuiMCP::SmallButton(un.c_str())) {
			*a_key = 0;
			changed = true;
		}
		const auto used = Conflict(*a_key);
		if (!used.empty()) {
			ImGuiMCP::SameLine();
			ImGuiMCP::Text("(the game also uses this key for '%s')", used.c_str());
		}
		return changed;
	}
}
