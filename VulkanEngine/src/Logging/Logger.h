#pragma once
#ifdef  CHIKU_ENABLE_LOGGING

#include "EngineHeader.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>

namespace CHIKU
{
    class Logger {
    public:
        static void Init(const std::string& name = "Engine") {
            if (!s_Logger) {
                s_Logger = spdlog::stdout_color_mt(name);
                s_Logger->set_level(spdlog::level::trace); // change to info/debug if needed
            }
        }
        static void Shutdown()
        {
            spdlog::shutdown();
        }

        static SHARED<spdlog::logger>& Get() { return s_Logger; }

    private:
        static SHARED<spdlog::logger> s_Logger;
    };
}

// Debugbreak
#if defined(_MSC_VER)
#define DEBUG_BREAK __debugbreak()
#else
#include <signal.h>
#define DEBUG_BREAK raise(SIGTRAP)
#endif

#define ASSERT(result, message)                      \
    if (XR_FAILED(result)) {                                      \
        std::cerr << "OpenXR Error: " << message << std::endl;    \
        std::exit(EXIT_FAILURE);                                  \
    }

#define LOG_TRACE(...)    CHIKU::Logger::Get()->trace(__VA_ARGS__)
#define LOG_DEBUG(...)    CHIKU::Logger::Get()->debug(__VA_ARGS__)
#define LOG_INFO(...)     CHIKU::Logger::Get()->info(__VA_ARGS__)
#define LOG_WARN(...)     CHIKU::Logger::Get()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    CHIKU::Logger::Get()->error(__VA_ARGS__); \
DEBUG_BREAK

#define LOG_CRITICAL(...) CHIKU::Logger::Get()->critical(__VA_ARGS__); \
DEBUG_BREAK

#else
#define LOG_TRACE(...)    
#define LOG_DEBUG(...)    
#define LOG_INFO(...)     
#define LOG_WARN(...)     
#define LOG_ERROR(...)    
#define LOG_CRITICAL(...) 
#endif