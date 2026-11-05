# Scrolling Speed 2026

Mouse-wheel walking / running / riding speed for Skyrim SE/AE (1.7.x included). A pure SKSE plugin: **no ESP, no Papyrus, no MCM**.

* Wheel changes the speed level (default 5 levels, slowest = 18% of normal); the modifier key (default Left Shift) + wheel zooms the camera. Swap the roles in the menu.
* Speed is a temporary modifier on the SpeedMult actor value, so other speed effects keep stacking. What the plugin added is kept in the SKSE co-save and reconciled on load.
* Mounts follow the player's level while riding.
* Settings: SKSE Menu Framework page "Scrolling Speed" (optional) or `Data/SKSE/Plugins/ScrollingSpeed2026.json`.
* Camera plugins: the wheel is read from the raw input device, zoom is blocked with the game's own `fMouseWheelZoomIncrement:Camera` (set to 0 while the wheel is for speed, original value restored after). TFCam's free camera keeps its wheel.
* Optional extras: a zoom key (while held the wheel always zooms), faster / slower keys, a menu to pick every key, the on-screen speed note can be switched off.
* The wheel is left alone in menus (dialogue, containers, ...) and during conversations.
* Not covered: the wheel can still switch between first and third person when the camera is fully zoomed in or out.
* Before uninstalling press "Reset and remove all modifiers" in the menu.

## Install
Needs SKSE64, Address Library for SKSE Plugins, and (for the settings page) SKSE Menu Framework. Built for Skyrim SE / AE including 1.7.x (tested on 1.7.104; not VR). Put `SKSE/Plugins/ScrollingSpeed2026.dll` in your Data folder (or install the zip with your mod manager). Settings are created on first start in `Data/SKSE/Plugins/ScrollingSpeed2026.json`. Disable the old Scrolling Speed (Arindel, Papyrus) and Scrolling Speed NG when you use this one.

## Credits and licence
**Based on [Scrolling Speed - adjust running speed with mouse wheel](https://www.nexusmods.com/skyrim/mods/65570) by SirArindel (Arindel).**
The original author's permissions are: *"You are free to use the resources in this mod as long as you share your mod with the same permissions."*
This mod is shared with the same permissions: you are free to use anything in it, as long as you share your own mod with the same permissions (the GNU General Public License v3 in LICENSE.txt gives everyone the same freedoms; Arindel's credit has to stay).

Why a new plugin instead of updating Scrolling Speed NG: NG no longer loads on Skyrim 1.7.x, its author is no longer active and no source is available, so it could not be updated or forked. The original has open permissions, so this plugin is built on its idea.

Scrolling Speed 2026 is a new SKSE plugin written from scratch (no ESP, no Papyrus, no MCM) that keeps the original's idea: wheel speed levels for the player and the horse, modifier key to zoom. It contains no code or assets from Scrolling Speed NG, which is a separate, closed-licence mod.
Built with alandtse's CommonLibSSE-NG (GPL-3.0 with a modding exception) and bundles the SKSE Menu Framework header (GPL-3.0), so the whole plugin is licensed under the **GNU General Public License, version 3 or later** (LICENSE.txt, NOTICE.txt). The source of every release is this repository at the release tag.
