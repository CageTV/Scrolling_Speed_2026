/*
 * Scrolling Speed 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#include "PCH.h"
#include "Menu.h"
#include "Config.h"
#include "Speed.h"
#include "Wheel.h"

#include "SKSEMenuFramework.h"

namespace Menu
{
	namespace
	{
		void Render()
		{
			auto& c = Config::Get();
			bool  changed = false;

			ImGuiMCP::SeparatorText("Scrolling Speed 2026");
			if (ImGuiMCP::Checkbox("Enabled##ss_on", &c.enabled)) {
				changed = true;
				if (!c.enabled) {
					Wheel::Release();
				}
				Speed::Refresh();  // disabled -> the modifier comes off again
			}
			if (ImGuiMCP::Checkbox("Wheel changes speed (modifier + wheel = zoom)##ss_inv", &c.wheelChangesSpeed)) {
				changed = true;
			}
			ImGuiMCP::Text("Modifier key (DirectInput scan code: 42 left shift, 29 left ctrl, 56 left alt, 0 none)");
			if (ImGuiMCP::SliderInt("##ss_key", &c.modifierKey, 0, 255)) {
				changed = true;
			}
			if (ImGuiMCP::SliderInt("Speed levels##ss_steps", &c.steps, 2, 20)) {
				c.startLevel = std::clamp(c.startLevel, 0, c.steps);
				changed = true;
				Speed::Refresh();
			}
			if (ImGuiMCP::SliderFloat("Slowest speed (% of normal)##ss_slow", &c.slowestPercent, 5.0f, 95.0f, "%.0f")) {
				changed = true;
				Speed::Refresh();
			}
			if (ImGuiMCP::SliderInt("Level on a new game (0 = fastest)##ss_start", &c.startLevel, 0, c.steps)) {
				changed = true;
			}
			if (ImGuiMCP::Checkbox("Mount gets the same speed##ss_mount", &c.affectMounts)) {
				changed = true;
				Speed::Refresh();
			}
			if (ImGuiMCP::Checkbox("Show a note on every change##ss_note", &c.notify)) {
				changed = true;
			}

			ImGuiMCP::Separator();
			ImGuiMCP::Text("Now: level %d / %d = %.0f%% of normal speed", Speed::Level(), Speed::Steps(), Speed::CurrentPercent());
			if (ImGuiMCP::Button("Slower##ss_down")) {
				Speed::ChangeLevel(-1);
			}
			ImGuiMCP::SameLine();
			if (ImGuiMCP::Button("Faster##ss_up")) {
				Speed::ChangeLevel(+1);
			}
			ImGuiMCP::SameLine();
			if (ImGuiMCP::Button("Reset and remove all modifiers (before uninstalling)##ss_reset")) {
				Speed::ResetAll();
			}

			if (changed) {
				Config::Save();
			}
		}
	}

	void Init()
	{
		if (!SKSEMenuFramework::IsInstalled()) {
			SKSE::log::info("menu: SKSE Menu Framework not installed, settings stay in ScrollingSpeed2026.json");
			return;
		}
		SKSEMenuFramework::SetSection("Scrolling Speed");
		SKSEMenuFramework::AddSectionItem("Settings", Render);
		SKSE::log::info("menu: settings page registered");
	}
}
