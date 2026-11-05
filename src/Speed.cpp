/*
 * Scrolling Speed 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the MIT License; see LICENSE.txt.
 */
#include "PCH.h"
#include "Speed.h"
#include "Config.h"

// How the speed is applied (no ESP, no spell): a TEMPORARY modifier on the SpeedMult actor value, the same value the game's own slow / fortify
// effects move. Only the difference to what this plugin added before is applied, so other speed effects keep stacking. What was added is
// written to the SKSE co-save, and after a load it is compared with what the save really kept (modifier at save time vs now), so the speed
// is never added twice and a stale slowdown is taken off again.
namespace Speed
{
	namespace
	{
		constexpr std::uint32_t kRecord = 'SSP6';
		constexpr std::uint32_t kVersion = 1;
		constexpr auto          kAV = RE::ActorValue::kSpeedMult;
		constexpr auto          kMod = RE::ACTOR_VALUE_MODIFIER::kTemporary;

		struct Slot
		{
			RE::FormID formID = 0;
			float      delta = 0.0f;      // what this plugin has added to the actor's SpeedMult modifier
			float      modAtSave = 0.0f;  // the actor's temporary SpeedMult modifier when the co-save was written
			bool       pending = false;   // loaded from the co-save, not reconciled yet
		};

		int  level = 0;  // 0 = not set yet
		Slot player;
		Slot mount;

		int Top() { return Config::Get().steps; }

		float TargetDelta(int a_level)
		{
			const auto& c = Config::Get();
			const int   top = c.steps;
			if (!c.enabled || a_level >= top) {
				return 0.0f;
			}
			const float pct = c.slowestPercent + (100.0f - c.slowestPercent) * static_cast<float>(a_level - 1) / static_cast<float>(top - 1);
			return pct - 100.0f;
		}

		float ModNow(RE::Actor* a) { return a ? a->GetActorValueModifier(kMod, kAV) : 0.0f; }

		// the game recalculates the movement speed when the carry weight changes; the Papyrus original nudged InventoryWeight the same way
		void Nudge(RE::Actor* a)
		{
			if (auto* owner = a->AsActorValueOwner()) {
				owner->ModActorValue(kMod, RE::ActorValue::kInventoryWeight, 1.0f);
				owner->ModActorValue(kMod, RE::ActorValue::kInventoryWeight, -1.0f);
			}
		}

		void Apply(Slot& s, RE::Actor* a, float a_target)
		{
			if (!a) {
				return;
			}
			const float diff = a_target - s.delta;
			if (std::fabs(diff) < 0.001f) {
				return;
			}
			if (auto* owner = a->AsActorValueOwner()) {
				owner->ModActorValue(kMod, kAV, diff);
			}
			s.delta = a_target;
			Nudge(a);
		}

		void Reconcile(Slot& s, RE::Actor* a, const char* a_name)
		{
			s.pending = false;
			if (!a || std::fabs(s.delta) < 0.001f) {
				s.delta = 0.0f;
				return;
			}
			const float now = ModNow(a);
			const bool  kept = std::fabs(now - s.modAtSave) <= std::fabs(now - (s.modAtSave - s.delta));
			SKSE::log::info("reconcile {}: co-save delta {:.2f}, modifier at save {:.2f}, now {:.2f} -> {}", a_name, s.delta, s.modAtSave, now, kept ? "the save kept it" : "the save dropped it, will be applied again");
			if (!kept) {
				s.delta = 0.0f;
			}
		}

		RE::Actor* Find(RE::FormID a_id) { return a_id ? RE::TESForm::LookupByID<RE::Actor>(a_id) : nullptr; }

		void SaveCallback(SKSE::SerializationInterface* a_intfc)
		{
			if (!a_intfc->OpenRecord(kRecord, kVersion)) {
				return;
			}
			auto* pc = RE::PlayerCharacter::GetSingleton();
			player.modAtSave = ModNow(pc);
			auto* m = Find(mount.formID);
			mount.modAtSave = ModNow(m);
			a_intfc->WriteRecordData(level);
			a_intfc->WriteRecordData(player.delta);
			a_intfc->WriteRecordData(player.modAtSave);
			a_intfc->WriteRecordData(mount.formID);
			a_intfc->WriteRecordData(mount.delta);
			a_intfc->WriteRecordData(mount.modAtSave);
		}

		void LoadCallback(SKSE::SerializationInterface* a_intfc)
		{
			std::uint32_t type, version, length;
			while (a_intfc->GetNextRecordInfo(type, version, length)) {
				if (type != kRecord || version != kVersion) {
					continue;
				}
				a_intfc->ReadRecordData(level);
				a_intfc->ReadRecordData(player.delta);
				a_intfc->ReadRecordData(player.modAtSave);
				RE::FormID id = 0;
				a_intfc->ReadRecordData(id);
				a_intfc->ReadRecordData(mount.delta);
				a_intfc->ReadRecordData(mount.modAtSave);
				if (!a_intfc->ResolveFormID(id, mount.formID)) {
					mount = Slot{};
				}
				player.pending = true;
				mount.pending = mount.formID != 0;
			}
		}

		void RevertCallback(SKSE::SerializationInterface*)
		{
			level = 0;
			player = Slot{};
			mount = Slot{};
		}

		void EnsureLevel()
		{
			const auto& c = Config::Get();
			if (level <= 0) {
				level = c.startLevel > 0 ? std::clamp(c.startLevel, 1, c.steps) : c.steps;
			}
			level = std::clamp(level, 1, c.steps);
		}
	}

	void Init()
	{
		const auto* ser = SKSE::GetSerializationInterface();
		ser->SetUniqueID(kRecord);
		ser->SetSaveCallback(SaveCallback);
		ser->SetLoadCallback(LoadCallback);
		ser->SetRevertCallback(RevertCallback);
	}

	int Steps() { return Top(); }

	int Level()
	{
		EnsureLevel();
		return level;
	}

	float CurrentPercent() { return 100.0f + TargetDelta(Level()); }

	void OnPostLoad()
	{
		auto* pc = RE::PlayerCharacter::GetSingleton();
		if (player.pending) {
			Reconcile(player, pc, "player");
		}
		if (mount.pending) {
			if (auto* m = Find(mount.formID)) {
				Reconcile(mount, m, "mount");
			}
		}
		Refresh();
	}

	void Refresh()
	{
		auto* pc = RE::PlayerCharacter::GetSingleton();
		if (!pc || !pc->Is3DLoaded()) {
			return;
		}
		const auto& c = Config::Get();
		EnsureLevel();
		const float target = TargetDelta(level);

		if (player.pending) {
			Reconcile(player, pc, "player");
		}
		Apply(player, pc, target);

		RE::NiPointer<RE::Actor> ridden;
		if (c.affectMounts && pc->GetMount(ridden) && ridden) {
			if (mount.formID != ridden->GetFormID()) {
				if (auto* old = Find(mount.formID)) {
					Apply(mount, old, 0.0f);
				}
				mount = Slot{};
				mount.formID = ridden->GetFormID();
			}
			if (mount.pending) {
				Reconcile(mount, ridden.get(), "mount");
			}
			Apply(mount, ridden.get(), target);
		} else if (mount.formID != 0) {
			if (auto* old = Find(mount.formID)) {
				Apply(mount, old, 0.0f);
				mount = Slot{};
			} else if (!mount.pending) {
				mount = Slot{};
			}
		}
	}

	void ChangeLevel(int a_dir)
	{
		EnsureLevel();
		const int before = level;
		level = std::clamp(level + a_dir, 1, Top());
		if (level != before) {
			Refresh();
		}
		if (Config::Get().notify) {
			const auto text = std::format("Speed {} / {} ({:.0f}%)", level, Top(), CurrentPercent());
			RE::SendHUDMessage::ShowHUDMessage(text.c_str(), nullptr, false);
		}
	}

	void ResetAll()
	{
		auto* pc = RE::PlayerCharacter::GetSingleton();
		Apply(player, pc, 0.0f);
		if (auto* m = Find(mount.formID)) {
			Apply(mount, m, 0.0f);
		}
		mount = Slot{};
	}
}
