#pragma once

#include "Application.h"
#define XR_USE_GRAPHICS_API_VULKAN
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

namespace CHIKU
{
	class OpenXR : public Application
	{
	public:
		OpenXR();
		void Init() override;
		void Run() override;
		void Render() override;
		void CleanUp() override;

	private:
		void CreateInstance();
		void DestroyInstance();
		void CreateDebugMessenger();
		void DestroyDebugMessenger();
		void GetInstanceProperties();
		void GetSystemID();

	private:
		XrInstance m_Instance = {};;
		XrDebugUtilsMessengerEXT m_debugMessenger;
		XrFormFactor m_formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
		XrSystemId m_systemID = {};
		XrSystemProperties m_systemProperties = { XR_TYPE_SYSTEM_PROPERTIES };

		std::vector<const char*> m_activeAPILayers = {};
		std::vector<const char*> m_activeInstanceExtensions = {};
		std::vector<std::string> m_apiLayers = {};
		std::vector<std::string> m_instanceExtensions = { XR_EXT_DEBUG_UTILS_EXTENSION_NAME, GetGraphicsAPIInstanceExtensionString(XR_TUTORIAL_GRAPHICS_API)};
	};
}