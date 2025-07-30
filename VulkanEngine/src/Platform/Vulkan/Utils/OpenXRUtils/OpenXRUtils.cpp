#include "OpenXRUtils.h"

namespace CHIKU::Utils
{
    XrBool32 OpenXRMessageCallbackFunction(XrDebugUtilsMessageSeverityFlagsEXT messageSeverity, 
        XrDebugUtilsMessageTypeFlagsEXT messageType, 
        const XrDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
        void* pUserData)
    {
        auto GetMessageSeverityString = [](XrDebugUtilsMessageSeverityFlagsEXT messageSeverity) -> std::string {
            bool separator = false;

            std::string msgFlags;
            if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)) {
                msgFlags += "VERBOSE";
                separator = true;
            }
            if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)) {
                if (separator) {
                    msgFlags += ",";
                }
                msgFlags += "INFO";
                separator = true;
            }
            if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)) {
                if (separator) {
                    msgFlags += ",";
                }
                msgFlags += "WARN";
                separator = true;
            }
            if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)) {
                if (separator) {
                    msgFlags += ",";
                }
                msgFlags += "ERROR";
            }
            return msgFlags;
            };

        auto GetMessageTypeString = [](XrDebugUtilsMessageTypeFlagsEXT messageType) -> std::string {
            bool separator = false;

            std::string msgFlags;
            if (BitwiseCheck(messageType, XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)) {
                msgFlags += "GEN";
                separator = true;
            }
            if (BitwiseCheck(messageType, XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)) {
                if (separator) {
                    msgFlags += ",";
                }
                msgFlags += "SPEC";
                separator = true;
            }
            if (BitwiseCheck(messageType, XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)) {
                if (separator) {
                    msgFlags += ",";
                }
                msgFlags += "PERF";
            }
            return msgFlags;
            };

        std::string functionName = (pCallbackData->functionName) ? pCallbackData->functionName : "";
        std::string messageSeverityStr = GetMessageSeverityString(messageSeverity);
        std::string messageTypeStr = GetMessageTypeString(messageType);
        std::string messageId = (pCallbackData->messageId) ? pCallbackData->messageId : "";
        std::string message = (pCallbackData->message) ? pCallbackData->message : "";

        std::stringstream errorMessage;
        errorMessage << functionName << "(" << messageSeverityStr << " / " << messageTypeStr << "): msgNum: " << messageId << " - " << message;

        std::cerr << errorMessage.str() << std::endl;
        if (BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)) {
            DEBUG_BREAK;
        }
        return XrBool32();
    }


    XrDebugUtilsMessengerEXT CreateOpenXRDebugUtilsMessenger(XrInstance m_xrInstance)
    {
        XrDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCI{ XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        debugUtilsMessengerCI.messageSeverities = XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugUtilsMessengerCI.messageTypes = XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT;
        debugUtilsMessengerCI.userCallback = (PFN_xrDebugUtilsMessengerCallbackEXT)OpenXRMessageCallbackFunction;
        debugUtilsMessengerCI.userData = nullptr;

        XrDebugUtilsMessengerEXT debugUtilsMessenger{};
        PFN_xrCreateDebugUtilsMessengerEXT xrCreateDebugUtilsMessengerEXT;
        OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, 
            "xrCreateDebugUtilsMessengerEXT", 
            (PFN_xrVoidFunction*)&xrCreateDebugUtilsMessengerEXT), 
            "Failed to get InstanceProcAddr.");

        OPENXR_CHECK(xrCreateDebugUtilsMessengerEXT(m_xrInstance, &debugUtilsMessengerCI, &debugUtilsMessenger), "Failed to create DebugUtilsMessenger.");
        return debugUtilsMessenger;
    }

    void DestroyOpenXRDebugUtilsMessenger(XrInstance m_xrInstance, XrDebugUtilsMessengerEXT debugUtilsMessenger)
    {
        PFN_xrDestroyDebugUtilsMessengerEXT xrDestroyDebugUtilsMessengerEXT;
        OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrDestroyDebugUtilsMessengerEXT", (PFN_xrVoidFunction*)&xrDestroyDebugUtilsMessengerEXT), "Failed to get InstanceProcAddr.");
        OPENXR_CHECK(xrDestroyDebugUtilsMessengerEXT(debugUtilsMessenger), "Failed to destroy DebugUtilsMessenger.");
    }
}