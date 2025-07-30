#include "OpenXR.h"
#include "Vulkan/Renderer/VulkanRenderer.h"
#include "Logging/Logger.h"

namespace OpenXR
{
	OpenXR::OpenXR() : CHIKU::Application()
	{
		CreateInstance();
		GetInstanceProperties();
		GetSystemInfo();
		CreateDebugMessenger();
		LoadPFN_XrFunctions(m_xrInstance);

		XrGraphicsRequirementsVulkanKHR graphicsRequirements{ XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR };
		xrGetVulkanGraphicsRequirementsKHR(m_xrInstance, m_SystemID, &graphicsRequirements);

		m_RendererExtensions = GetInstanceExtensionsForOpenXR();

		std::vector<const char*> rendererExtensions;

		for(const auto& ext : m_RendererExtensions)
		{
			rendererExtensions.push_back(ext.c_str());
		}

		CHIKU::Renderer::AddExtions(rendererExtensions.data(),rendererExtensions.size());

		GetGraphicsInfo();
	}

	void OpenXR::Init()
	{
		Application::Init();
	
		CreateSession();
	}

	void OpenXR::Run()
	{
		Application::Run();
	}

	void OpenXR::Render()
	{
		Application::Render();
	}

	void OpenXR::LoadPFN_XrFunctions(XrInstance m_xrInstance)
	{
		OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetVulkanGraphicsRequirementsKHR", (PFN_xrVoidFunction*)&xrGetVulkanGraphicsRequirementsKHR), "Failed to get InstanceProcAddr for xrGetVulkanGraphicsRequirementsKHR.");
		OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetVulkanInstanceExtensionsKHR", (PFN_xrVoidFunction*)&xrGetVulkanInstanceExtensionsKHR), "Failed to get InstanceProcAddr for xrGetVulkanInstanceExtensionsKHR.");
		OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetVulkanDeviceExtensionsKHR", (PFN_xrVoidFunction*)&xrGetVulkanDeviceExtensionsKHR), "Failed to get InstanceProcAddr for xrGetVulkanDeviceExtensionsKHR.");
		OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetVulkanGraphicsDeviceKHR", (PFN_xrVoidFunction*)&xrGetVulkanGraphicsDeviceKHR), "Failed to get InstanceProcAddr for xrGetVulkanGraphicsDeviceKHR.");
	}

	std::vector<std::string> OpenXR::GetInstanceExtensionsForOpenXR()
	{
		uint32_t extensionNamesSize = 0;
		OPENXR_CHECK(xrGetVulkanInstanceExtensionsKHR(m_xrInstance, m_SystemID, 0, &extensionNamesSize, nullptr), "Failed to get Vulkan Instance Extensions.");

		std::vector<char> extensionNames(extensionNamesSize);
		OPENXR_CHECK(xrGetVulkanInstanceExtensionsKHR(m_xrInstance, m_SystemID, extensionNamesSize, &extensionNamesSize, extensionNames.data()), "Failed to get Vulkan Instance Extensions.");

		std::stringstream streamData(extensionNames.data());
		std::vector<std::string> extensions;
		std::string extension;

		while (std::getline(streamData, extension, ' ')) 
		{
			extensions.push_back(extension);
		}

		return extensions;
	}

	void OpenXR::CreateInstance()
	{
		XrApplicationInfo AI;
		AI.applicationVersion = 1;
		AI.engineVersion = 1;
		AI.apiVersion = XR_CURRENT_API_VERSION;
		strncpy(AI.applicationName, "OpenXR Tutorial Chapter 2", XR_MAX_APPLICATION_NAME_SIZE);
		strncpy(AI.engineName, "OpenXR Engine", XR_MAX_ENGINE_NAME_SIZE);

		// Get all the API Layers from the OpenXR runtime.
		uint32_t apiLayerCount = 0;
		std::vector<XrApiLayerProperties> apiLayerProperties;
		OPENXR_CHECK(xrEnumerateApiLayerProperties(0, &apiLayerCount, nullptr), "Failed to enumerate ApiLayerProperties.");
		apiLayerProperties.resize(apiLayerCount, { XR_TYPE_API_LAYER_PROPERTIES });
		OPENXR_CHECK(xrEnumerateApiLayerProperties(apiLayerCount, &apiLayerCount, apiLayerProperties.data()), "Failed to enumerate ApiLayerProperties.");

		// Check the requested API layers against the ones from the OpenXR. If found add it to the Active API Layers.
		for (auto& requestLayer : m_ApiLayers)
		{
			for (auto& layerProperty : apiLayerProperties)
			{
				// strcmp returns 0 if the strings match.
				if (strcmp(requestLayer.c_str(), layerProperty.layerName) == 0)
				{
					m_ActiveAPILayers.push_back(requestLayer.c_str());
					break;
				}
			}
		}

		// Get all the Instance Extensions from the OpenXR instance.
		uint32_t extensionCount = 0;
		std::vector<XrExtensionProperties> extensionProperties;
		OPENXR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr), "Failed to enumerate InstanceExtensionProperties.");
		extensionProperties.resize(extensionCount, { XR_TYPE_EXTENSION_PROPERTIES });
		OPENXR_CHECK(xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, extensionProperties.data()), "Failed to enumerate InstanceExtensionProperties.");

		// Check the requested Instance Extensions against the ones from the OpenXR runtime.
		// If an extension is found add it to Active Instance Extensions.
		// Log error if the Instance Extension is not found.
		for (auto& requestedInstanceExtension : m_InstanceExtensions) {
			bool found = false;
			for (auto& extensionProperty : extensionProperties) {
				// strcmp returns 0 if the strings match.
				if (strcmp(requestedInstanceExtension.c_str(), extensionProperty.extensionName) != 0) {
					continue;
				}
				else {
					m_ActiveInstanceExtensions.push_back(requestedInstanceExtension.c_str());
					found = true;
					break;
				}
			}
			if (!found) {
				std::cout << "Failed to find OpenXR instance extension: " << requestedInstanceExtension;
			}
		}

		XrInstanceCreateInfo instanceCI{ XR_TYPE_INSTANCE_CREATE_INFO };
		instanceCI.createFlags = 0;
		instanceCI.applicationInfo = AI;
		instanceCI.enabledApiLayerCount = static_cast<uint32_t>(m_ActiveAPILayers.size());
		instanceCI.enabledApiLayerNames = m_ActiveAPILayers.data();
		instanceCI.enabledExtensionCount = static_cast<uint32_t>(m_ActiveInstanceExtensions.size());
		instanceCI.enabledExtensionNames = m_ActiveInstanceExtensions.data();
		OPENXR_CHECK(xrCreateInstance(&instanceCI, &m_xrInstance), "Failed to create Instance.");
	}

	void OpenXR::GetInstanceProperties()
	{
		XrInstanceProperties instanceProperties{ XR_TYPE_INSTANCE_PROPERTIES };
		OPENXR_CHECK(xrGetInstanceProperties(m_xrInstance, &instanceProperties), "Failed to get InstanceProperties.");

		std::cout << "OpenXR Runtime: " << instanceProperties.runtimeName << " - "
			<< XR_VERSION_MAJOR(instanceProperties.runtimeVersion) << "."
			<< XR_VERSION_MINOR(instanceProperties.runtimeVersion) << "."
			<< XR_VERSION_PATCH(instanceProperties.runtimeVersion);
	}

	void OpenXR::CreateDebugMessenger()
	{
		if (Utils::IsStringInVector(m_ActiveInstanceExtensions, XR_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
			m_DebugUtilsMessenger = Utils::CreateOpenXRDebugUtilsMessenger(m_xrInstance);  // From OpenXRDebugUtils.h.
		}
	}

	void OpenXR::GetSystemInfo()
	{
		// Get the XrSystemId from the instance and the supplied XrFormFactor.
		XrSystemGetInfo systemGI{ XR_TYPE_SYSTEM_GET_INFO };
		systemGI.formFactor = m_FormFactor;
		OPENXR_CHECK(xrGetSystem(m_xrInstance, &systemGI, &m_SystemID), "Failed to get SystemID.");

		// Get the System's properties for some general information about the hardware and the vendor.
		OPENXR_CHECK(xrGetSystemProperties(m_xrInstance, m_SystemID, &m_SystemProperties), "Failed to get SystemProperties.");
	}

	void OpenXR::GetGraphicsInfo()
	{
		PFN_xrGetVulkanGraphicsRequirements2KHR pfnGetVulkanGraphicsRequirements2KHR = nullptr;
		xrGetInstanceProcAddr(
			m_xrInstance, // your XrInstance
			"xrGetVulkanGraphicsRequirements2KHR",
			(PFN_xrVoidFunction*)(&pfnGetVulkanGraphicsRequirements2KHR));

		if (pfnGetVulkanGraphicsRequirements2KHR) {
			XrResult result = pfnGetVulkanGraphicsRequirements2KHR(m_xrInstance, m_SystemID, &m_GraphicsRequirements);
			if (XR_FAILED(result)) {
				LOG_ERROR("" + result);
			}
		}
		else
		{
			LOG_ERROR("Method not found : xrGetVulkanGraphicsRequirementsKHR");
		}
	}

	void OpenXR::CreateSession()
	{
		XrSessionCreateInfo sessionCI{ XR_TYPE_SESSION_CREATE_INFO };
		XrGraphicsBindingVulkanKHR graphicsBinding;

		graphicsBinding = { XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR };
		graphicsBinding.instance = CHIKU::VulkanRenderer::GetVulkanInstance();
		graphicsBinding.physicalDevice = CHIKU::VulkanRenderer::GetVulkanPhysicalDevice();
		graphicsBinding.device = CHIKU::VulkanRenderer::GetVulkanDevice();
		graphicsBinding.queueFamilyIndex = CHIKU::VulkanRenderer::GetGraphicsQueueFamilyIndex();
		graphicsBinding.queueIndex = 0;

		sessionCI.next = &graphicsBinding;
		sessionCI.createFlags = 0;
		sessionCI.systemId = m_SystemID;

		XrResult result = xrCreateSession(m_xrInstance, &sessionCI, &m_Session);
		
		OPENXR_CHECK(result, "Failed to create Session.");

	}

	void OpenXR::PollEvents()
	{
		// Poll OpenXR for a new event.
		XrEventDataBuffer eventData{ XR_TYPE_EVENT_DATA_BUFFER };
		auto XrPollEvents = [&]() -> bool {
			eventData = { XR_TYPE_EVENT_DATA_BUFFER };
			return xrPollEvent(m_xrInstance, &eventData) == XR_SUCCESS;
			};

		while (XrPollEvents()) {
			switch (eventData.type) {
				// Log the number of lost events from the runtime.
			case XR_TYPE_EVENT_DATA_EVENTS_LOST: {
				XrEventDataEventsLost* eventsLost = reinterpret_cast<XrEventDataEventsLost*>(&eventData);
				std::cout << "OPENXR: Events Lost: " << eventsLost->lostEventCount;
				break;
			}
											   // Log that an instance loss is pending and shutdown the application.
			case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
				XrEventDataInstanceLossPending* instanceLossPending = reinterpret_cast<XrEventDataInstanceLossPending*>(&eventData);
				std::cout << "OPENXR: Instance Loss Pending at: " << instanceLossPending->lossTime;
				m_sessionRunning = false;
				m_applicationRunning = false;
				break;
			}
														 // Log that the interaction profile has changed.
			case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED: {
				XrEventDataInteractionProfileChanged* interactionProfileChanged = reinterpret_cast<XrEventDataInteractionProfileChanged*>(&eventData);
				std::cout << "OPENXR: Interaction Profile changed for Session: " << interactionProfileChanged->session;
				if (interactionProfileChanged->session != m_Session) {
					std::cout << "XrEventDataInteractionProfileChanged for unknown Session";
					break;
				}
				break;
			}
															   // Log that there's a reference space change pending.
			case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING: {
				XrEventDataReferenceSpaceChangePending* referenceSpaceChangePending = reinterpret_cast<XrEventDataReferenceSpaceChangePending*>(&eventData);
				std::cout << "OPENXR: Reference Space Change pending for Session: " << referenceSpaceChangePending->session;
				if (referenceSpaceChangePending->session != m_Session) {
					std::cout << "XrEventDataReferenceSpaceChangePending for unknown Session";
					break;
				}
				break;
			}
																  // Session State changes:
			case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
				XrEventDataSessionStateChanged* sessionStateChanged = reinterpret_cast<XrEventDataSessionStateChanged*>(&eventData);
				if (sessionStateChanged->session != m_Session) {
					std::cout << "XrEventDataSessionStateChanged for unknown Session";
					break;
				}

				if (sessionStateChanged->state == XR_SESSION_STATE_READY) {
					// SessionState is ready. Begin the XrSession using the XrViewConfigurationType.
					XrSessionBeginInfo sessionBeginInfo{ XR_TYPE_SESSION_BEGIN_INFO };
					sessionBeginInfo.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
					OPENXR_CHECK(xrBeginSession(m_Session, &sessionBeginInfo), "Failed to begin Session.");
					m_sessionRunning = true;
				}
				if (sessionStateChanged->state == XR_SESSION_STATE_STOPPING) {
					// SessionState is stopping. End the XrSession.
					OPENXR_CHECK(xrEndSession(m_Session), "Failed to end Session.");
					m_sessionRunning = false;
				}
				if (sessionStateChanged->state == XR_SESSION_STATE_EXITING) {
					// SessionState is exiting. Exit the application.
					m_sessionRunning = false;
					m_applicationRunning = false;
				}
				if (sessionStateChanged->state == XR_SESSION_STATE_LOSS_PENDING) {
					// SessionState is loss pending. Exit the application.
					// It's possible to try a reestablish an XrInstance and XrSession, but we will simply exit here.
					m_sessionRunning = false;
					m_applicationRunning = false;
				}
				// Store state for reference across the application.
				m_SessionState = sessionStateChanged->state;
				break;
			}
			default: {
				break;
			}
			}
		}
	}

	void OpenXR::CleanUp()
	{
		DestroyDebugMessenger();
		DestroyInstance();
		DestroySession();

		Application::CleanUp();
	}

	void OpenXR::DestroySession()
	{
		OPENXR_CHECK(xrDestroySession(m_Session), "Failed to destroy Session.");
	}

	void OpenXR::DestroyInstance()
	{
		OPENXR_CHECK(xrDestroyInstance(m_xrInstance), "Failed to destroy Instance.");
	}

	void OpenXR::DestroyDebugMessenger()
	{
		// Check that "XR_EXT_debug_utils" is in the active Instance Extensions before destroying the XrDebugUtilsMessengerEXT.
		if (m_DebugUtilsMessenger != XR_NULL_HANDLE) {
			Utils::DestroyOpenXRDebugUtilsMessenger(m_xrInstance, m_DebugUtilsMessenger);  // From OpenXRDebugUtils.h.
		}
	}

} // namespace Utils

namespace CHIKU
{
	// Factory function to create an instance of the OpenXR application.
	CHIKU::Application* CreateApplication()
	{
		return new OpenXR::OpenXR();
	}
}