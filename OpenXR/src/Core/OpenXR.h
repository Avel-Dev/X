#pragma once

#include "Application.h"
#include "Utils/OpenXRUtils.h"

#define XR_USE_GRAPHICS_API_VULKAN
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

namespace OpenXR
{
	class OpenXR : public CHIKU::Application
	{
	public:
		OpenXR();
		void Init() override;
		void Run() override;
		void Render() override;
		void CleanUp() override;

	private:
		void CreateInstance();
		void GetInstanceProperties();
		void CreateDebugMessenger();
		void GetSystemInfo();
		void CreateSession();

		void PollEvents();

		void DestroySession();
		void DestroyInstance();
		void DestroyDebugMessenger();


	private:
		XrInstance m_xrInstance = {};;
		XrDebugUtilsMessengerEXT m_debugMessenger = {};
		XrFormFactor m_formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
		XrSystemId m_systemID = {};
		XrSystemProperties m_systemProperties = { XR_TYPE_SYSTEM_PROPERTIES };
		XrDebugUtilsMessengerEXT m_debugUtilsMessenger = {};
		XrSession m_session = XR_NULL_HANDLE;
		XrSessionState m_sessionState = XR_SESSION_STATE_UNKNOWN;

		std::vector<const char*> m_activeAPILayers = {};
		std::vector<const char*> m_activeInstanceExtensions = {};
		std::vector<std::string> m_apiLayers = {};
		std::vector<std::string> m_instanceExtensions = { XR_EXT_DEBUG_UTILS_EXTENSION_NAME, XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME };
	};
}