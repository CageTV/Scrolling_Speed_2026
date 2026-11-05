/*
 * Scrolling Speed 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the GNU General Public License, version 3 or (at your option) any later version; see LICENSE.txt.
 */
#pragma once

namespace Wheel
{
	void Init();       // input sink (call at kDataLoaded)
	void Release();    // give the camera its own wheel zoom back (mod disabled, menu)
}
