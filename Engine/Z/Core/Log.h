//
// Created by 32725 on 2023/3/12.
//

#ifndef ENGINE_TUTORIAL_LOG_H
#define ENGINE_TUTORIAL_LOG_H
#include <memory>

#include "Include/glm/glm/gtx/string_cast.hpp"
#include "Include/spdlog/include/spdlog/spdlog.h"
#include "Include/spdlog/include/spdlog/fmt/ostr.h"
#include "Include/spdlog/include/spdlog/logger.h"

#include "Z/Core/Core.h"

namespace Z {
	class Z_API Log {
		static std::shared_ptr<spdlog::logger> z_CoreLog;
		static std::shared_ptr<spdlog::logger> z_ClientLog;
	public:
		static void Init();
		inline static auto& GetCoreLog(){return Log::z_CoreLog;}
		inline static auto& GetClientLog(){return Log::z_ClientLog;}

	};
}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}

#define Z_CORE_TRACE(...) ::Z::Log::GetCoreLog()->trace(__VA_ARGS__)
#define Z_CORE_INFO(...) ::Z::Log::GetCoreLog()->info(__VA_ARGS__)
#define Z_CORE_WARN(...) ::Z::Log::GetCoreLog()->warn(__VA_ARGS__)
#define Z_CORE_ERROR(...) ::Z::Log::GetCoreLog()->error(__VA_ARGS__)
#define Z_CORE_FATAL(...) ::Z::Log::GetCoreLog()->fatal(__VA__ARGS__)

#define Z_TRACE(...)    ::Z::Log::GetClientLog()->trace(__VA_ARGS__)
#define Z_INFO(...)     ::Z::Log::GetClientLog()->info(__VA_ARGS__)
#define Z_WARN(...)     ::Z::Log::GetClientLog()->warn(__VA_ARGS__)
#define Z_ERROR(...)    ::Z::Log::GetClientLog()->error(__VA_ARGS__)
#define Z_FATAL(...)    ::Z::Log::GetClientLog()->fatal(__VA__ARGS__)

#endif //ENGINE_TUTORIAL_LOG_H
