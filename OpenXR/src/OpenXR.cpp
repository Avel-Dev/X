#include "OpenXR.h"

#define CHECK_XR_RESULT(result, message)                          \
    if (XR_FAILED(result)) {                                      \
        std::cerr << "OpenXR Error: " << message << std::endl;    \
        std::exit(EXIT_FAILURE);                                  \
    }

namespace CHIKU
{

#define CHECK_XR_RESULT(result, message)                          \
    if (XR_FAILED(result)) {                                      \
        std::cerr << "OpenXR Error: " << message << std::endl;    \
        std::exit(EXIT_FAILURE);                                  \
    }

	OpenXR::OpenXR() :Application()
	{
	}

	void OpenXR::Init()
	{
		Application::Init();

		XrApplicationInfo AI;
		strncpy(AI.applicationName, "OpenXR Tutorial Chapter 2", XR_MAX_APPLICATION_NAME_SIZE);
		AI.applicationVersion = 1;
		strncpy(AI.engineName, "OpenXR Engine", XR_MAX_ENGINE_NAME_SIZE);
		AI.engineVersion = 1;
		AI.apiVersion = XR_CURRENT_API_VERSION;
		
		m_instanceExtensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
		// Ensure m_apiType is already defined when we call this line.
		m_instanceExtensions.push_back(XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME);

		// Get all the API Layers from the OpenXR runtime.
		uint32_t apiLayerCount = 0;
		std::vector<XrApiLayerProperties> apiLayerProperties;
		CHECK_XR_RESULT(xrEnumerateApiLayerProperties(0, &apiLayerCount, nullptr), "Failed to enumerate ApiLayerProperties.");
		apiLayerProperties.resize(apiLayerCount, { XR_TYPE_API_LAYER_PROPERTIES });
		CHECK_XR_RESULT(xrEnumerateApiLayerProperties(apiLayerCount, &apiLayerCount, apiLayerProperties.data()), "Failed to enumerate ApiLayerProperties.");

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
		CHECK_XR_RESULT(xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr), "Failed to enumerate InstanceExtensionProperties.");

		extensionProperties.resize(extensionCount, { XR_TYPE_EXTENSION_PROPERTIES });
		CHECK_XR_RESULT(xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, extensionProperties.data()), "Failed to enumerate InstanceExtensionProperties.");

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
		CHECK_XR_RESULT(xrCreateInstance(&instanceCI, &m_Instance), "Failed to create Instance.");

		XrInstanceProperties instanceProperties{ XR_TYPE_INSTANCE_PROPERTIES };
		CHECK_XR_RESULT(xrGetInstanceProperties(m_Instance, &instanceProperties), "Failed to get InstanceProperties.");

		std::cout << "OpenXR Runtime: " << instanceProperties.runtimeName << " - "
			<< XR_VERSION_MAJOR(instanceProperties.runtimeVersion) << "."
			<< XR_VERSION_MINOR(instanceProperties.runtimeVersion) << "."
			<< XR_VERSION_PATCH(instanceProperties.runtimeVersion);

	}

	void OpenXR::Run()
	{
		Application::Run();
	}

	void OpenXR::Render()
	{
		Application::Render();
	}

	void OpenXR::CleanUp()
	{
		DestroyInstance();

		Application::CleanUp();
	}


	void OpenXR::DestroyInstance()
	{
		CHECK_XR_RESULT(xrDestroyInstance(m_Instance), "Failed to destroy Instance.");
	}

	void OpenXR::CreateDebugMessenger()
	{
	}

	void OpenXR::DestroyDebugMessenger()
	{
	}

	void OpenXR::GetInstanceProperties()
	{
	}

	void OpenXR::GetSystemID()
	{
	}

} // namespace CHIKU