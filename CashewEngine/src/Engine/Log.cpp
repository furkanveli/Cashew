#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Cashew
{
	std::shared_ptr<spdlog::logger> Log::s_EngineLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
	std::once_flag Log::initFlag; // Define the flag

	void Log::Init()
	{
		std::call_once(initFlag, []() {
			spdlog::set_pattern("%^[%T] %n: %v%$");
			s_EngineLogger = spdlog::stderr_color_mt("Cashew Engine");
			s_EngineLogger->set_level(spdlog::level::trace);

			s_ClientLogger = spdlog::stdout_color_mt("Client");
			s_ClientLogger->set_level(spdlog::level::trace);
			});
	}

}