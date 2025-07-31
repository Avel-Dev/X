#include "OpenXR.h"

namespace CHIKU
{
	OpenXR* OpenXR::s_Instance = nullptr;

	//std::vector<std::string> OpenXR::GetVulkanRequiredExtensions()
	//{
	//	if (s_Instance->m_VulkanRequiredExtensions.size() <= 0)
	//	{
	//		s_Instance->GenerateVulkanRequiredExtensions();
	//	}
	//	return s_Instance->m_VulkanRequiredExtensions;
	//}

	void OpenXR::mInit()
	{
		CreateInstance();
		CreateDebugMessenger();

		GetInstanceProperties();
		GenerateSystemInfo();
	}

	void OpenXR::mRun()
	{
		CreateSession();

		while (m_applicationRunning) {
			PollSystemEvents();
			PollEvents();
			if (m_sessionRunning) {
				// Draw Frame.
			}
		}

		DestroySession();
	}

	void OpenXR::mCleanUp()
	{
		DestroyDebugMessenger();
		DestroyInstance();
	}

	//uint32_t OpenXR::mGetAPIVersion()
	//{
	//	XrGraphicsRequirementsVulkanKHR graphicsRequirements{ XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR };
	//	OPENXR_CHECK(xrGetVulkanGraphicsRequirementsKHR(m_xrInstance, m_systemID, &graphicsRequirements), "Failed to get Graphics Requirements for Vulkan.");

	//	return VK_MAKE_API_VERSION(0, XR_VERSION_MAJOR(graphicsRequirements.minApiVersionSupported), XR_VERSION_MINOR(graphicsRequirements.minApiVersionSupported), 0);
	//}

	//VkPhysicalDevice OpenXR::mGetXRPhysicalDevice()
	//{
	//	VkPhysicalDevice physicalDeviceFromXR;
	//	OPENXR_CHECK(xrGetVulkanGraphicsDeviceKHR(m_xrInstance, m_systemID, (VkInstance)Renderer::GetInstance(), &physicalDeviceFromXR), "Failed to get Graphics Device for Vulkan.");
	//	return physicalDeviceFromXR;
	//}

	//std::vector<std::string> OpenXR::mGetDeviceExtensionsForOpenXR()
	//{
	//	uint32_t extensionNamesSize = 0;
	//	OPENXR_CHECK(xrGetVulkanDeviceExtensionsKHR(m_xrInstance, m_systemID, 0, &extensionNamesSize, nullptr), "Failed to get Vulkan Device Extensions.");

	//	std::vector<char> extensionNames(extensionNamesSize);
	//	OPENXR_CHECK(xrGetVulkanDeviceExtensionsKHR(m_xrInstance, m_systemID, extensionNamesSize, &extensionNamesSize, extensionNames.data()), "Failed to get Vulkan Device Extensions.");

	//	std::stringstream streamData(extensionNames.data());
	//	std::vector<std::string> extensions;
	//	std::string extension;
	//	while (std::getline(streamData, extension, ' ')) {
	//		extensions.push_back(extension);
	//	}
	//	return extensions;
	//}


	enum GraphicsAPI_Type : uint8_t {
		UNKNOWN,
		D3D11,
		D3D12,
		OPENGL,
		OPENGL_ES,
		VULKAN
	};

	const char* GetGraphicsAPIInstanceExtensionString(GraphicsAPI_Type type) {
#if defined(XR_USE_GRAPHICS_API_D3D11)
		if (type == D3D11) {
			return XR_KHR_D3D11_ENABLE_EXTENSION_NAME;
		}
#endif
#if defined(XR_USE_GRAPHICS_API_D3D12)
		if (type == D3D12) {
			return XR_KHR_D3D12_ENABLE_EXTENSION_NAME;
		}
#endif
#if defined(XR_USE_GRAPHICS_API_OPENGL)
		if (type == OPENGL) {
			return XR_KHR_OPENGL_ENABLE_EXTENSION_NAME;
		}
#endif
#if defined(XR_USE_GRAPHICS_API_OPENGL_ES)
		if (type == OPENGL_ES) {
			return XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME;
		}
#endif
#if defined(XR_USE_GRAPHICS_API_VULKAN)
		if (type == VULKAN) {
			return XR_KHR_VULKAN_ENABLE_EXTENSION_NAME;
		}
#endif
		std::cerr << "ERROR: Unknown Graphics API." << std::endl;
		DEBUG_BREAK;
		return nullptr;
	}

	void OpenXR::CreateInstance()
	{
		XrApplicationInfo AI;
		strncpy(AI.applicationName, "OpenXR Tutorial Chapter 2", XR_MAX_APPLICATION_NAME_SIZE);
		AI.applicationVersion = 1;
		strncpy(AI.engineName, "OpenXR Engine", XR_MAX_ENGINE_NAME_SIZE);
		AI.engineVersion = 1;
		AI.apiVersion = XR_CURRENT_API_VERSION;
		m_instanceExtensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
		// Ensure m_apiType is already defined when we call this line.
		m_instanceExtensions.push_back(GetGraphicsAPIInstanceExtensionString(VULKAN));

		// Get all the API Layers from the OpenXR runtime.
		uint32_t apiLayerCount = 0;
		std::vector<XrApiLayerProperties> apiLayerProperties;
		OPENXR_CHECK(xrEnumerateApiLayerProperties(0, &apiLayerCount, nullptr), "Failed to enumerate ApiLayerProperties.");
		apiLayerProperties.resize(apiLayerCount, { XR_TYPE_API_LAYER_PROPERTIES });
		OPENXR_CHECK(xrEnumerateApiLayerProperties(apiLayerCount, &apiLayerCount, apiLayerProperties.data()), "Failed to enumerate ApiLayerProperties.");

		// Check the requested API layers against the ones from the OpenXR. If found add it to the Active API Layers.
		for (auto& requestLayer : m_apiLayers) {
			for (auto& layerProperty : apiLayerProperties) {
				// strcmp returns 0 if the strings match.
				if (strcmp(requestLayer.c_str(), layerProperty.layerName) != 0) {
					continue;
				}
				else {
					m_activeAPILayers.push_back(requestLayer.c_str());
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
		for (auto& requestedInstanceExtension : m_instanceExtensions) {
			bool found = false;
			for (auto& extensionProperty : extensionProperties) {
				// strcmp returns 0 if the strings match.
				if (strcmp(requestedInstanceExtension.c_str(), extensionProperty.extensionName) != 0) {
					continue;
				}
				else {
					m_activeInstanceExtensions.push_back(requestedInstanceExtension.c_str());
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
		instanceCI.enabledApiLayerCount = static_cast<uint32_t>(m_activeAPILayers.size());
		instanceCI.enabledApiLayerNames = m_activeAPILayers.data();
		instanceCI.enabledExtensionCount = static_cast<uint32_t>(m_activeInstanceExtensions.size());
		instanceCI.enabledExtensionNames = m_activeInstanceExtensions.data();
		OPENXR_CHECK(xrCreateInstance(&instanceCI, &m_xrInstance), "Failed to create Instance.");


	}

	void OpenXR::DestroyInstance()
	{
		OPENXR_CHECK(xrDestroyInstance(m_xrInstance), "Failed to destroy Instance.");
	}

	// XR_DOCS_TAG_BEGIN_OpenXRMessageCallbackFunction
	XrBool32 OpenXRMessageCallbackFunction(XrDebugUtilsMessageSeverityFlagsEXT messageSeverity, XrDebugUtilsMessageTypeFlagsEXT messageType, const XrDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		// Lambda to covert an XrDebugUtilsMessageSeverityFlagsEXT to std::string. Bitwise check to concatenate multiple severities to the output string.
		auto GetMessageSeverityString = [](XrDebugUtilsMessageSeverityFlagsEXT messageSeverity) -> std::string {
			bool separator = false;

			std::string msgFlags;
			if (Utils::BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)) {
				msgFlags += "VERBOSE";
				separator = true;
			}
			if (Utils::BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)) {
				if (separator) {
					msgFlags += ",";
				}
				msgFlags += "INFO";
				separator = true;
			}
			if (Utils::BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)) {
				if (separator) {
					msgFlags += ",";
				}
				msgFlags += "WARN";
				separator = true;
			}
			if (Utils::BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)) {
				if (separator) {
					msgFlags += ",";
				}
				msgFlags += "ERROR";
			}
			return msgFlags;
			};

		// Lambda to covert an XrDebugUtilsMessageTypeFlagsEXT to std::string. Bitwise check to concatenate multiple types to the output string.
		auto GetMessageTypeString = [](XrDebugUtilsMessageTypeFlagsEXT messageType) -> std::string {
			bool separator = false;

			std::string msgFlags;
			if (Utils::BitwiseCheck(messageType, XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)) {
				msgFlags += "GEN";
				separator = true;
			}
			if (Utils::BitwiseCheck(messageType, XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)) {
				if (separator) {
					msgFlags += ",";
				}
				msgFlags += "SPEC";
				separator = true;
			}
			if (Utils::BitwiseCheck(messageType, XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)) {
				if (separator) {
					msgFlags += ",";
				}
				msgFlags += "PERF";
			}
			return msgFlags;
			};

		// Collect message data.
		std::string functionName = (pCallbackData->functionName) ? pCallbackData->functionName : "";
		std::string messageSeverityStr = GetMessageSeverityString(messageSeverity);
		std::string messageTypeStr = GetMessageTypeString(messageType);
		std::string messageId = (pCallbackData->messageId) ? pCallbackData->messageId : "";
		std::string message = (pCallbackData->message) ? pCallbackData->message : "";

		// String stream final message.
		std::stringstream errorMessage;
		errorMessage << functionName << "(" << messageSeverityStr << " / " << messageTypeStr << "): msgNum: " << messageId << " - " << message;

		// Log and debug break.
		std::cerr << errorMessage.str() << std::endl;
		if (Utils::BitwiseCheck(messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)) {
			DEBUG_BREAK;
		}
		return XrBool32();

	}

	XrDebugUtilsMessengerEXT CreateOpenXRDebugUtilsMessenger(XrInstance m_xrInstance) 
	{
		// Fill out a XrDebugUtilsMessengerCreateInfoEXT structure specifying all severities and types.
		// Set the userCallback to OpenXRMessageCallbackFunction().
		XrDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCI{ XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
		debugUtilsMessengerCI.messageSeverities = XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugUtilsMessengerCI.messageTypes = XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT;
		debugUtilsMessengerCI.userCallback = (PFN_xrDebugUtilsMessengerCallbackEXT)OpenXRMessageCallbackFunction;
		debugUtilsMessengerCI.userData = nullptr;

		// Load xrCreateDebugUtilsMessengerEXT() function pointer as it is not default loaded by the OpenXR loader.
		XrDebugUtilsMessengerEXT debugUtilsMessenger{};
		PFN_xrCreateDebugUtilsMessengerEXT xrCreateDebugUtilsMessengerEXT;
		OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrCreateDebugUtilsMessengerEXT", (PFN_xrVoidFunction*)&xrCreateDebugUtilsMessengerEXT), "Failed to get InstanceProcAddr.");

		// Finally create and return the XrDebugUtilsMessengerEXT.
		OPENXR_CHECK(xrCreateDebugUtilsMessengerEXT(m_xrInstance, &debugUtilsMessengerCI, &debugUtilsMessenger), "Failed to create DebugUtilsMessenger.");
		return debugUtilsMessenger;
	}

	void OpenXR::CreateDebugMessenger()
	{
		// Check that "XR_EXT_debug_utils" is in the active Instance Extensions before creating an XrDebugUtilsMessengerEXT.
		if (Utils::IsStringInVector(m_activeInstanceExtensions, XR_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
			m_debugUtilsMessenger = CreateOpenXRDebugUtilsMessenger(m_xrInstance);  // From OpenXRDebugUtils.h.
		}
	}

	void DestroyOpenXRDebugUtilsMessenger(XrInstance m_xrInstance, XrDebugUtilsMessengerEXT debugUtilsMessenger)
	{
		PFN_xrDestroyDebugUtilsMessengerEXT xrDestroyDebugUtilsMessengerEXT;
		OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrDestroyDebugUtilsMessengerEXT", (PFN_xrVoidFunction*)&xrDestroyDebugUtilsMessengerEXT), "Failed to get InstanceProcAddr.");
		OPENXR_CHECK(xrDestroyDebugUtilsMessengerEXT(debugUtilsMessenger), "Failed to destroy DebugUtilsMessenger.");
	}

	void OpenXR::DestroyDebugMessenger()
	{
		// Check that "XR_EXT_debug_utils" is in the active Instance Extensions before destroying the XrDebugUtilsMessengerEXT.
		if (m_debugUtilsMessenger != XR_NULL_HANDLE) {
			DestroyOpenXRDebugUtilsMessenger(m_xrInstance, m_debugUtilsMessenger);  // From OpenXRDebugUtils.h.
		}
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

	void OpenXR::GenerateSystemInfo()
	{
		// Get the XrSystemId from the instance and the supplied XrFormFactor.
		XrSystemGetInfo systemGI{ XR_TYPE_SYSTEM_GET_INFO };
		systemGI.formFactor = m_formFactor;
		OPENXR_CHECK(xrGetSystem(m_xrInstance, &systemGI, &m_systemID), "Failed to get SystemID.");

		// Get the System's properties for some general information about the hardware and the vendor.
		OPENXR_CHECK(xrGetSystemProperties(m_xrInstance, m_systemID, &m_systemProperties), "Failed to get SystemProperties.");
	}

	void OpenXR::CreateSession()
	{
		XrSessionCreateInfo sessionCI{ XR_TYPE_SESSION_CREATE_INFO };

		auto binding = Renderer::GetGraphicsBinding();
		sessionCI.next = binding;
		sessionCI.createFlags = 0;
		sessionCI.systemId = m_systemID;

		OPENXR_CHECK(xrCreateSession(m_xrInstance, &sessionCI, &m_session), "Failed to create Session.");
	}

	void OpenXR::DestroySession()
	{
		OPENXR_CHECK(xrDestroySession(m_session), "Failed to destroy Session.");
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
				std::cout << "OPENXR: Events Lost: " << eventsLost->lostEventCount << "\n";
				break;
			}
											   // Log that an instance loss is pending and shutdown the application.
			case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
				XrEventDataInstanceLossPending* instanceLossPending = reinterpret_cast<XrEventDataInstanceLossPending*>(&eventData);
				std::cout << "OPENXR: Instance Loss Pending at: " << instanceLossPending->lossTime << "\n";
				m_sessionRunning = false;
				m_applicationRunning = false;
				break;
			}
														 // Log that the interaction profile has changed.
			case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED: {
				XrEventDataInteractionProfileChanged* interactionProfileChanged = reinterpret_cast<XrEventDataInteractionProfileChanged*>(&eventData);
				std::cout << "OPENXR: Interaction Profile changed for Session: " << interactionProfileChanged->session << "\n";
				if (interactionProfileChanged->session != m_session) {
					std::cout << "XrEventDataInteractionProfileChanged for unknown Session";
					break;
				}
				break;
			}
															   // Log that there's a reference space change pending.
			case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING: {
				XrEventDataReferenceSpaceChangePending* referenceSpaceChangePending = reinterpret_cast<XrEventDataReferenceSpaceChangePending*>(&eventData);
				std::cout << "OPENXR: Reference Space Change pending for Session: " << referenceSpaceChangePending->session << "\n";
				if (referenceSpaceChangePending->session != m_session) {
					std::cout << "XrEventDataReferenceSpaceChangePending for unknown Session" << "\n";
					break;
				}
				break;
			}
																  // Session State changes:
			case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
				XrEventDataSessionStateChanged* sessionStateChanged = reinterpret_cast<XrEventDataSessionStateChanged*>(&eventData);
				if (sessionStateChanged->session != m_session) {
					std::cout << "XrEventDataSessionStateChanged for unknown Session" << "\n";
					break;
				}

				if (sessionStateChanged->state == XR_SESSION_STATE_READY) {
					// SessionState is ready. Begin the XrSession using the XrViewConfigurationType.
					XrSessionBeginInfo sessionBeginInfo{ XR_TYPE_SESSION_BEGIN_INFO };
					sessionBeginInfo.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
					OPENXR_CHECK(xrBeginSession(m_session, &sessionBeginInfo), "Failed to begin Session.");
					m_sessionRunning = true;
				}
				if (sessionStateChanged->state == XR_SESSION_STATE_STOPPING) {
					// SessionState is stopping. End the XrSession.
					OPENXR_CHECK(xrEndSession(m_session), "Failed to end Session.");
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
				m_sessionState = sessionStateChanged->state;
				break;
			}
			default: {
				break;
			}
			}
		}
	}

	void OpenXR::PollSystemEvents()
	{

	}

	//void OpenXR::LoadPFN_XrFunctions()
	//{
	//	OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetVulkanGraphicsRequirementsKHR", (PFN_xrVoidFunction*)&xrGetVulkanGraphicsRequirementsKHR), "Failed to get InstanceProcAddr for xrGetVulkanGraphicsRequirementsKHR.");
	//	OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetVulkanInstanceExtensionsKHR", (PFN_xrVoidFunction*)&xrGetVulkanInstanceExtensionsKHR), "Failed to get InstanceProcAddr for xrGetVulkanInstanceExtensionsKHR.");
	//	OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetVulkanDeviceExtensionsKHR", (PFN_xrVoidFunction*)&xrGetVulkanDeviceExtensionsKHR), "Failed to get InstanceProcAddr for xrGetVulkanDeviceExtensionsKHR.");
	//	OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetVulkanGraphicsDeviceKHR", (PFN_xrVoidFunction*)&xrGetVulkanGraphicsDeviceKHR), "Failed to get InstanceProcAddr for xrGetVulkanGraphicsDeviceKHR.");
	//}

	//std::vector<std::string> OpenXR::GetInstanceExtensionsForOpenXR(XrInstance m_xrInstance, XrSystemId systemId)
	//{
	//	uint32_t extensionNamesSize = 0;
	//	OPENXR_CHECK(xrGetVulkanInstanceExtensionsKHR(m_xrInstance, systemId, 0, &extensionNamesSize, nullptr), "Failed to get Vulkan Instance Extensions.");

	//	std::vector<char> extensionNames(extensionNamesSize);
	//	OPENXR_CHECK(xrGetVulkanInstanceExtensionsKHR(m_xrInstance, systemId, extensionNamesSize, &extensionNamesSize, extensionNames.data()), "Failed to get Vulkan Instance Extensions.");

	//	std::stringstream streamData(extensionNames.data());
	//	std::vector<std::string> extensions;
	//	std::string extension;
	//	while (std::getline(streamData, extension, ' ')) {
	//		extensions.push_back(extension);
	//	}
	//	return extensions;
	//}

	/*void OpenXR::GenerateVulkanRequiredExtensions()
	{
		uint32_t instanceExtensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr), "Failed to enumerate InstanceExtensionProperties.";

		std::vector<VkExtensionProperties> instanceExtensionProperties;
		instanceExtensionProperties.resize(instanceExtensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, instanceExtensionProperties.data());
		const std::vector<std::string>& openXrInstanceExtensionNames = GetInstanceExtensionsForOpenXR(m_xrInstance, m_systemID);

		for (const std::string& requestExtension : openXrInstanceExtensionNames)
		{
			for (const VkExtensionProperties& extensionProperty : instanceExtensionProperties)
			{
				if (strcmp(requestExtension.c_str(), extensionProperty.extensionName))
				{
					continue;
				}
				else
				{
					m_VulkanRequiredExtensions.push_back(requestExtension.c_str());
				}
				break;
			}
		}
	}*/
}
