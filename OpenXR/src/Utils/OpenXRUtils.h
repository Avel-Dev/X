#pragma once
#include "OpenXRHeader.h"
#include "OpenXRUtils.h"
#include <vector>

namespace Utils
{

    // Debugbreak
#if defined(_MSC_VER)
#define DEBUG_BREAK __debugbreak()
#else
#include <signal.h>
#define DEBUG_BREAK raise(SIGTRAP)
#endif

#define OPENXR_CHECK(result, message)                      \
    if (XR_FAILED(result)) {                                      \
        std::cerr << "OpenXR Error: " << message << std::endl;    \
        std::exit(EXIT_FAILURE);                                  \
    }

    inline bool IsStringInVector(std::vector<const char*> list, const char* name) {
        bool found = false;
        for (auto& item : list) {
            if (strcmp(name, item) == 0) {
                found = true;
                break;
            }
        }
        return found;
    }

    template <typename T>
    inline bool BitwiseCheck(const T& value, const T& checkValue) {
        return ((value & checkValue) == checkValue);
    }

    XrBool32 OpenXRMessageCallbackFunction(
        XrDebugUtilsMessageSeverityFlagsEXT messageSeverity,
        XrDebugUtilsMessageTypeFlagsEXT messageType,
        const XrDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    XrDebugUtilsMessengerEXT CreateOpenXRDebugUtilsMessenger(XrInstance m_xrInstance);
    void DestroyOpenXRDebugUtilsMessenger(XrInstance m_xrInstance, XrDebugUtilsMessengerEXT debugUtilsMessenger);
} // namespace OpenXR