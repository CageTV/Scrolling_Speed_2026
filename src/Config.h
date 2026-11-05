/*
 * Scrolling Speed 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#pragma once

#include <string>
#include <vector>

namespace Config
{
	struct Settings
	{
		bool  enabled = true;
		int   modifierKey = 42;           // DirectInput scan code (42 left shift, 29 left ctrl, 56 left alt); 0 = no modifier
		bool  wheelChangesSpeed = true;   // true: wheel = speed, modifier + wheel = zoom.  false: wheel = zoom, modifier + wheel = speed
		int   steps = 5;                  // number of speed levels (2..20)
		float slowestPercent = 18.0f;     // speed at level 1, in percent of normal (level `steps` = 100%)
		int   startLevel = 0;             // level on a new game; 0 = the fastest
		bool  affectMounts = true;        // the mount gets the same level while riding
		bool  notify = true;              // short on-screen note on every change
		std::vector<std::string> blockedMenus = { "Dialogue Menu", "LootMenu", "Console", "Journal Menu", "TweenMenu" };  // wheel is left alone while one of these is open
	};

	Settings& Get();
	void      Load();
	void      Save();
}
