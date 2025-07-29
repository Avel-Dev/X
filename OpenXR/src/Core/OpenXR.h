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
		void GetGraphicsInfo();
		void CreateSession();

		void PollEvents();

		void DestroySession();
		void DestroyInstance();
		void DestroyDebugMessenger();


	private:
		XrInstance m_xrInstance = {};;
		XrDebugUtilsMessengerEXT m_DebugMessenger = {};
		XrFormFactor m_FormFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
		XrSystemId m_SystemID = {};
		XrSystemProperties m_SystemProperties = { XR_TYPE_SYSTEM_PROPERTIES };
		XrDebugUtilsMessengerEXT m_DebugUtilsMessenger = {};
		XrGraphicsRequirementsVulkanKHR m_GraphicsRequirements{ XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR };
		XrSession m_Session = XR_NULL_HANDLE;
		XrSessionState m_SessionState = XR_SESSION_STATE_UNKNOWN;

		std::vector<const char*> m_ActiveAPILayers = {};
		std::vector<const char*> m_ActiveInstanceExtensions = {};
		std::vector<std::string> m_ApiLayers = {};
		std::vector<std::string> m_InstanceExtensions = { XR_EXT_DEBUG_UTILS_EXTENSION_NAME, XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME };
	};
}