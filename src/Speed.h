/*
 * Scrolling Speed 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#pragma once

namespace Speed
{
	void Init();                    // SKSE co-save callbacks (call once at plugin load)
	void ChangeLevel(int a_dir);    // +1 faster, -1 slower
	int  Level();                   // 1..steps
	int  Steps();
	float CurrentPercent();         // percent of normal speed the current level gives
	void Refresh();                 // reapply to the player / current mount, release the previous mount (called about once a second and after loads)
	void ResetAll();                // take every modifier this plugin added off again (menu button, mod disabled, before uninstalling)
	void OnPostLoad();              // a save finished loading: reconcile what the save kept with what the co-save says
}
