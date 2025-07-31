#pragma once
#include "OpenXRHeader.h"
#include <Renderer/Renderer.h>
#include <Vulkan/Utils/OpenXRUtils/OpenXRUtils.h>
#include <Logging/Logger.h>

namespace CHIKU
{
    class OpenXR
    {
    public:
		OpenXR() = default; 
        ~OpenXR() = default;

        static void Init()
        {
            s_Instance = new OpenXR();
            s_Instance->mInit();
		}

		//static uint32_t GetAPIVersion() { return s_Instance->mGetAPIVersion(); }
        static void Run() { s_Instance->mRun(); }
        static XrInstance GetInstance() { return s_Instance->m_xrInstance; }
        //static std::vector<std::string> GetVulkanRequiredExtensions();
        //static VkPhysicalDevice GetXRPhysicalDevice() { return s_Instance->mGetXRPhysicalDevice(); }
        //static std::vector<std::string> GetDeviceExtensionsForOpenXR() { return s_Instance->mGetDeviceExtensionsForOpenXR();  }
		static XrSystemId GetSystemID() { return s_Instance->m_systemID; }

        static void CleanUp() 
        { 
            s_Instance->mCleanUp(); 
            delete s_Instance;
            s_Instance = nullptr;
        }

    private:
        void mInit();
        void mRun();
        void mCleanUp();
        //uint32_t mGetAPIVersion();
        //VkPhysicalDevice mGetXRPhysicalDevice();

        //std::vector<std::string> mGetDeviceExtensionsForOpenXR();

        void CreateInstance();
        void DestroyInstance();
        void CreateDebugMessenger();
        void DestroyDebugMessenger();
        void GetInstanceProperties();
        void GenerateSystemInfo();
        void CreateSession();
        void DestroySession();
        void PollEvents();
        void PollSystemEvents();
        //void LoadPFN_XrFunctions();

        //std::vector<std::string> GetInstanceExtensionsForOpenXR(XrInstance m_xrInstance, XrSystemId systemId);
        //void GenerateVulkanRequiredExtensions();

    private:
        static OpenXR* s_Instance;

        XrInstance m_xrInstance = {};
        std::vector<const char*> m_activeAPILayers = {};
        std::vector<const char*> m_activeInstanceExtensions = {};

        std::vector<std::string> m_VulkanRequiredExtensions = {};
        std::vector<std::string> m_apiLayers = {};
        std::vector<std::string> m_instanceExtensions = {};

        XrDebugUtilsMessengerEXT m_debugUtilsMessenger = {};

        XrFormFactor m_formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
        XrSystemId m_systemID = {};
        XrSystemProperties m_systemProperties = { XR_TYPE_SYSTEM_PROPERTIES };

        XrSession m_session = XR_NULL_HANDLE;
        XrSessionState m_sessionState = XR_SESSION_STATE_UNKNOWN;
        bool m_applicationRunning = true;
        bool m_sessionRunning = false;
    };
}