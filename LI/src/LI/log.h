#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace LI {
	class LI_API log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

#define LI_CORE_ERROR(...) ::LI::log::GetCoreLogger()->error(__VA_ARGS__)
#define LI_CORE_WARN(...) ::LI::log::GetCoreLogger()->warn(__VA_ARGS__)
#define LI_CORE_INFO(...) ::LI::log::GetCoreLogger()->info(__VA_ARGS__)
#define LI_CORE_TRACE(...) ::LI::log::GetCoreLogger()->trace(__VA_ARGS__)

#define LI_ERROR(...) ::LI::log::GetClientLogger()->error(__VA_ARGS__)
#define LI_WARN(...) ::LI::log::GetClientLogger()->warn(__VA_ARGS__)
#define LI_INFO(...) ::LI::log::GetClientLogger()->info(__VA_ARGS__)
#define LI_TRACE(...) ::LI::log::GetClientLogger()->trace(__VA_ARGS__)
