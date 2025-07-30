#pragma once
#include <Vulkan/Renderer/OpenXRHeader.h>
#include <iostream>
#include <cstring>
#include <vector>

namespace CHIKU::Utils
{
    // XR_DOCS_TAG_BEGIN_Helper_Functions1
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

    // XR_DOCS_TAG_BEGIN_Helper_Functions0
    inline void OpenXRDebugBreak() {
        std::cerr << "Breakpoint here to debug." << std::endl;
        DEBUG_BREAK;
    }

    inline const char* GetXRErrorString(XrInstance xrInstance, XrResult result) {
        static char string[XR_MAX_RESULT_STRING_SIZE];
        xrResultToString(xrInstance, result, string);
        return string;
    }


    XrBool32 OpenXRMessageCallbackFunction(XrDebugUtilsMessageSeverityFlagsEXT messageSeverity, 
        XrDebugUtilsMessageTypeFlagsEXT messageType, 
        const XrDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
        void* pUserData);

    XrDebugUtilsMessengerEXT CreateOpenXRDebugUtilsMessenger(XrInstance m_xrInstance);
    void DestroyOpenXRDebugUtilsMessenger(XrInstance m_xrInstance, XrDebugUtilsMessengerEXT debugUtilsMessenger);
}

#define OPENXR_CHECK(x, y)                                                                                                                                  \
    {                                                                                                                                                       \
        XrResult result = (x);                                                                                                                              \
        if (!XR_SUCCEEDED(result)) {                                                                                                                        \
            std::cerr << "ERROR: OPENXR: " << int(result) << "(" << (m_xrInstance ? CHIKU::Utils::GetXRErrorString(m_xrInstance, result) : "") << ") " << y << std::endl; \
            CHIKU::Utils::OpenXRDebugBreak();                                                                                                                             \
        }                                                                                                                                                   \
    }
    // XR_DOCS_TAG_END_Helper_Functions0