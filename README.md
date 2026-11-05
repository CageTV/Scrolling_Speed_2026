# Scrolling Speed 2026

Mouse-wheel walking / running / riding speed for Skyrim SE/AE (1.7.x included). A pure SKSE plugin: **no ESP, no Papyrus, no MCM**.

* Wheel changes the speed level (default 5 levels, slowest = 18% of normal); the modifier key (default Left Shift) + wheel zooms the camera. Swap the roles in the menu.
* Speed is a temporary modifier on the SpeedMult actor value, so other speed effects keep stacking. What the plugin added is kept in the SKSE co-save and reconciled on load.
* Mounts follow the player's level while riding.
* Settings: SKSE Menu Framework page "Scrolling Speed" (optional) or `Data/SKSE/Plugins/ScrollingSpeed2026.json`.
* Camera plugins: the wheel is read from the raw input device, zoom is blocked with the game's own `fMouseWheelZoomIncrement:Camera` (set to 0 while the wheel is for speed, original value restored after). TFCam's free camera keeps its wheel.
* Before uninstalling press "Reset and remove all modifiers" in the menu.

## Credits and licence
**Based on [Scrolling Speed - adjust running speed with mouse wheel](https://www.nexusmods.com/skyrim/mods/65570) by SirArindel (Arindel).**
The original author's permissions are: *"You are free to use the resources in this mod as long as you share your mod with the same permissions."*
This mod is shared with the same permissions: you are free to use anything in it, as long as you share your own mod with the same permissions (the MIT licence in LICENSE.txt says the same and more; Arindel's credit has to stay).

Scrolling Speed 2026 is a new SKSE plugin written from scratch (no ESP, no Papyrus, no MCM) that keeps the original's idea: wheel speed levels for the player and the horse, modifier key to zoom. It contains no code or assets from Scrolling Speed NG, which is a separate, closed-licence mod.
Built with alandtse's CommonLibSSE-NG; bundles the SKSE Menu Framework header.
