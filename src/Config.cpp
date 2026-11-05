/*
 * Scrolling Speed 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#include "PCH.h"
#include "Config.h"

#include <nlohmann/json.hpp>

namespace Config
{
	namespace
	{
		using json = nlohmann::json;
		Settings   cfg;
		const auto kFile = std::filesystem::path("Data/SKSE/Plugins/ScrollingSpeed2026.json");
	}

	Settings& Get() { return cfg; }

	void Load()
	{
		std::ifstream f(kFile);
		if (!f) {
			Save();
			return;
		}
		try {
			const auto j = json::parse(f, nullptr, true, true);
			cfg.enabled = j.value("enabled", cfg.enabled);
			cfg.modifierKey = std::clamp(j.value("modifierKey", cfg.modifierKey), 0, 255);
			cfg.wheelChangesSpeed = j.value("wheelChangesSpeed", cfg.wheelChangesSpeed);
			cfg.steps = std::clamp(j.value("steps", cfg.steps), 2, 20);
			cfg.slowestPercent = std::clamp(j.value("slowestPercent", cfg.slowestPercent), 5.0f, 95.0f);
			cfg.startLevel = std::clamp(j.value("startLevel", cfg.startLevel), 0, cfg.steps);
			cfg.affectMounts = j.value("affectMounts", cfg.affectMounts);
			cfg.notify = j.value("notify", cfg.notify);
			if (j.contains("blockedMenus") && j["blockedMenus"].is_array()) {
				cfg.blockedMenus = j["blockedMenus"].get<std::vector<std::string>>();
			}
		} catch (const std::exception& e) {
			SKSE::log::warn("ScrollingSpeed2026.json unreadable ({}), using defaults", e.what());
		}
	}

	void Save()
	{
		std::error_code ec;
		std::filesystem::create_directories(kFile.parent_path(), ec);
		json j = { { "_help", { { "modifierKey", "DirectInput scan code: 42 left shift, 29 left ctrl, 56 left alt, 0 = none" }, { "wheelChangesSpeed", "true: wheel = speed, modifier+wheel = zoom; false: wheel = zoom, modifier+wheel = speed" },
					{ "slowestPercent", "speed of level 1 in percent of normal; the top level is 100" }, { "startLevel", "0 = fastest" }, { "blockedMenus", "the wheel is left alone while one of these menus is open" } } },
			{ "enabled", cfg.enabled }, { "modifierKey", cfg.modifierKey }, { "wheelChangesSpeed", cfg.wheelChangesSpeed }, { "steps", cfg.steps }, { "slowestPercent", cfg.slowestPercent }, { "startLevel", cfg.startLevel },
			{ "affectMounts", cfg.affectMounts }, { "notify", cfg.notify }, { "blockedMenus", cfg.blockedMenus } };
		std::ofstream(kFile) << j.dump(2);
	}
}
