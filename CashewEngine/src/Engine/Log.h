#pragma once

#include <memory>
#include "Macros.h"
#include "spdlog/spdlog.h"


namespace Cashew
{

	class CASHEW_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetEngineLogger() { return s_EngineLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_EngineLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
		static std::once_flag initFlag; // Ensure initialization only happens once
	};

}// namespace end


#ifdef CASHEW_DEBUG

	// Engine Log Macros
	#define ENGINE_TRACE(...)      ::Cashew::Log::GetEngineLogger()->trace(__VA_ARGS__)
	#define ENGINE_DEBUG(...)      ::Cashew::Log::GetEngineLogger()->debug(__VA_ARGS__)
	#define ENGINE_INFO(...)       ::Cashew::Log::GetEngineLogger()->info(__VA_ARGS__)
	#define ENGINE_WARN(...)       ::Cashew::Log::GetEngineLogger()->warn(__VA_ARGS__)
	#define ENGINE_ERROR(...)      ::Cashew::Log::GetEngineLogger()->error(__VA_ARGS__)
	#define ENGINE_CRITICAL(...)   ::Cashew::Log::GetEngineLogger()->critical(__VA_ARGS__)
	
	// Client Log Macros
	#define CLIENT_TRACE(...)      ::Cashew::Log::GetClientLogger()->trace(__VA_ARGS__)
	#define CLIENT_DEBUG(...)      ::Cashew::Log::GetClientLogger()->debug(__VA_ARGS__)
	#define CLIENT_INFO(...)       ::Cashew::Log::GetClientLogger()->info(__VA_ARGS__)
	#define CLIENT_WARN(...)       ::Cashew::Log::GetClientLogger()->warn(__VA_ARGS__)
	#define CLIENT_ERROR(...)      ::Cashew::Log::GetClientLogger()->error(__VA_ARGS__)
	#define CLIENT_CRITICAL(...)   ::Cashew::Log::GetClientLogger()->critical(__VA_ARGS__)

#else
	
	// Engine Log Macros
	#define ENGINE_TRACE(...)      
	#define ENGINE_DEBUG(...)      
	#define ENGINE_INFO(...)       
	#define ENGINE_WARN(...)       
	#define ENGINE_ERROR(...)      
	#define ENGINE_CRITICAL(...)   

	// Client Log Macros
	#define CLIENT_TRACE(...)      
	#define CLIENT_DEBUG(...)      
	#define CLIENT_INFO(...)       
	#define CLIENT_WARN(...)       
	#define CLIENT_ERROR(...)      
	#define CLIENT_CRITICAL(...)   


#endif