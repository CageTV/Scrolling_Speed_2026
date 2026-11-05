# Scrolling Speed 2026

Mouse-wheel walking / running / riding speed for Skyrim SE/AE (1.7.x included). A pure SKSE plugin: **no ESP, no Papyrus, no MCM**.

* Wheel changes the speed level (default 5 levels, slowest = 18% of normal); the modifier key (default Left Shift) + wheel zooms the camera. Swap the roles in the menu.
* Speed is a temporary modifier on the SpeedMult actor value, so other speed effects keep stacking. What the plugin added is kept in the SKSE co-save and reconciled on load.
* Mounts follow the player's level while riding.
* Settings: SKSE Menu Framework page "Scrolling Speed" (optional) or `Data/SKSE/Plugins/ScrollingSpeed2026.json`.
* Camera plugins: the wheel is read from the raw input device, zoom is blocked with the game's own `fMouseWheelZoomIncrement:Camera` (set to 0 while the wheel is for speed, original value restored after). TFCam's free camera keeps its wheel.
* Before uninstalling press "Reset and remove all modifiers" in the menu.

## Credits and licence
MIT (LICENSE.txt). Idea and behaviour descend from **Scrolling Speed** by Arindel (Nexus 65570, open permissions) and the ideas of Scrolling Speed NG; this plugin is a clean-room rewrite and contains no code or assets from the closed-licence NG release.
Built with alandtse's CommonLibSSE-NG; bundles the SKSE Menu Framework header.
