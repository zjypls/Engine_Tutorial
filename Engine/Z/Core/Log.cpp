//
// Created by 32725 on 2023/3/12.
//

#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Z{

	std::shared_ptr<spdlog::logger> Log::z_CoreLog;
	std::shared_ptr<spdlog::logger> Log::z_ClientLog;
	void Log::Init() {
		spdlog::set_pattern("%^[%T] %n:%v%$");
		Log::z_CoreLog=spdlog::stdout_color_mt("Z");
		Log::z_CoreLog->set_level(spdlog::level::trace);

		Log::z_ClientLog=spdlog::stdout_color_mt("Client");
		Log::z_ClientLog->set_level(spdlog::level::trace);
	}
}