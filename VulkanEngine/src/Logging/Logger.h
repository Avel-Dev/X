#pragma once

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

        template<typename... Args>
        inline static void Trace(Args&&... args) {
            s_Logger->trace(std::forward<Args>(args)...);
        }

        template<typename... Args>
        inline static void Debug(Args&&... args) {
            s_Logger->debug(std::forward<Args>(args)...);
        }

        template<typename... Args>
        inline static void Info(Args&&... args) {
            s_Logger->info(std::forward<Args>(args)...);
        }

        template<typename... Args>
        inline static void Warn(Args&&... args) {
            s_Logger->warn(std::forward<Args>(args)...);
        }

        template<typename... Args>
        inline static void Error(Args&&... args) {
            s_Logger->error(std::forward<Args>(args)...);
        }

        template<typename... Args>
        inline static void Critical(Args&&... args) {
            s_Logger->critical(std::forward<Args>(args)...);
        }

    private:
        static std::shared_ptr<spdlog::logger> s_Logger;
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

#ifdef CHIKU_ENABLE_LOGGING
#define LOG_TRACE(...)    CHIKU::Logger::Trace(__VA_ARGS__)
#define LOG_DEBUG(...)    CHIKU::Logger::Debug(__VA_ARGS__)
#define LOG_INFO(...)     CHIKU::Logger::Info(__VA_ARGS__)
#define LOG_WARN(...)     CHIKU::Logger::Warn(__VA_ARGS__)
#define LOG_ERROR(...)    CHIKU::Logger::Error(__VA_ARGS__); \
DEBUG_BREAK;

#define LOG_CRITICAL(...) CHIKU::Logger::Critical(__VA_ARGS__); \
DEBUG_BREAK

#else
#define LOG_TRACE(...)    
#define LOG_DEBUG(...)    
#define LOG_INFO(...)     
#define LOG_WARN(...)     
#define LOG_ERROR(...)    
#define LOG_CRITICAL(...) 
#endif