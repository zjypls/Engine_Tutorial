//
// Created by 32725 on 2023/3/12.
//

#include <iostream>

#include "Z/Core/Log.h"
#include "Include/spdlog/include/spdlog/sinks/stdout_color_sinks.h"

namespace Z{

	Ref<spdlog::logger> Log::z_CoreLog;
	Ref<spdlog::logger> Log::z_ClientLog;
	void Log::Init() {
		spdlog::set_pattern("%^[%T] [%n] [%=7l] : %v%$");
		Log::z_CoreLog=spdlog::stdout_color_mt("ZCore");
		Log::z_CoreLog->set_level(spdlog::level::trace);

		Log::z_ClientLog=spdlog::stdout_color_mt("Client");
		Log::z_ClientLog->set_level(spdlog::level::trace);
	}
}