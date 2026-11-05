/*
 * Scrolling Speed 2026
 * Copyright (c) 2026 CageTV
 *
 * Released under the GNU General Public License, version 3 or (at your option) any later version; see LICENSE.txt.
 */
#include "PCH.h"
#include "Config.h"
#include "Speed.h"
#include "Wheel.h"
#include "Menu.h"

namespace
{
	void SetupLog()
	{
		const auto dir = SKSE::log::log_directory();
		if (!dir) {
			return;
		}
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>((*dir / "ScrollingSpeed2026.log").string(), true);
		auto log = std::make_shared<spdlog::logger>("ScrollingSpeed2026", std::move(sink));
		log->set_level(spdlog::level::info);
		log->flush_on(spdlog::level::info);
		log->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
		spdlog::set_default_logger(std::move(log));
	}

	// the mount can change at any time; once a second is plenty and costs nothing
	void StartPoll()
	{
		std::thread([] {
			for (;;) {
				std::this_thread::sleep_for(1s);
				SKSE::GetTaskInterface()->AddTask([] {
					try {
						Speed::Refresh();
					} catch (const std::exception& e) {
						SKSE::log::error("poll: {}", e.what());
					}
				});
			}
		}).detach();
	}

	void OnMessage(SKSE::MessagingInterface::Message* a_msg)
	{
		try {
			switch (a_msg->type) {
			case SKSE::MessagingInterface::kDataLoaded:
				Config::Load();
				Wheel::Init();
				Menu::Init();
				StartPoll();
				break;
			case SKSE::MessagingInterface::kPostLoadGame:
			case SKSE::MessagingInterface::kNewGame:
				Speed::OnPostLoad();
				break;
			default:
				break;
			}
		} catch (const std::exception& e) {
			SKSE::log::error("failed: {}", e.what());
		}
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);
	SetupLog();
	SKSE::log::info("ScrollingSpeed2026 1.0.1, game {}", REL::Module::get().version().string());
	Speed::Init();
	SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
	return true;
}
