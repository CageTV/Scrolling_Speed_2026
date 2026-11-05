/*
 * Scrolling Speed 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the GNU General Public License, version 3 or (at your option) any later version; see LICENSE.txt.
 */
#pragma once

#include <Windows.h>

namespace Keys
{
	// Is the modifier key (a DirectInput scan code, e.g. 42 left shift, 29 left ctrl, 56 left alt) held down? 0 = no modifier needed, always true.
	inline bool ModifierHeld(int a_scanCode)
	{
		if (a_scanCode <= 0) {
			return true;
		}
		const UINT vk = MapVirtualKeyW(static_cast<UINT>(a_scanCode), MAPVK_VSC_TO_VK_EX);
		return vk != 0 && (GetAsyncKeyState(static_cast<int>(vk)) & 0x8000) != 0;
	}
}
