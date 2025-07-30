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
        OpenXR()
        {
        }

        ~OpenXR() = default;

        static void Init()
        {
            s_Instance = new OpenXR();
            s_Instance->mInit();
		}

		static uint32_t GetAPIVersion() { return s_Instance->mGetAPIVersion(); }

        uint32_t mGetAPIVersion()
        {
            XrGraphicsRequirementsVulkanKHR graphicsRequirements{ XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR };
            OPENXR_CHECK(xrGetVulkanGraphicsRequirementsKHR(m_xrInstance, m_systemID, &graphicsRequirements), "Failed to get Graphics Requirements for Vulkan.");

            return VK_MAKE_API_VERSION(0, XR_VERSION_MAJOR(graphicsRequirements.minApiVersionSupported), XR_VERSION_MINOR(graphicsRequirements.minApiVersionSupported), 0);
        }

        static void Run()
        {
            s_Instance->mRun();
		}

        static XrInstance GetInstance()
        {
            return s_Instance->mGetInstance();
		}

        static void CleanUp()
        {
            s_Instance->mCleanUp();
            delete s_Instance;
            s_Instance = nullptr;
        }

        void mInit()
        {
            CreateInstance();
            CreateDebugMessenger();

            GetInstanceProperties();
            GetSystemID();

            LoadPFN_XrFunctions();
        }

        void mRun()
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

        void mCleanUp()
        {
            DestroyDebugMessenger();
            DestroyInstance();
		}
        XrInstance mGetInstance() { return m_xrInstance; }

        static std::vector<std::string> GetVulkanRequiredExtensions()
        {
            if (s_Instance->m_VulkanRequiredExtensions.size() <= 0)
            {
                s_Instance->GenerateVulkanRequiredExtensions();
            }
            return s_Instance->m_VulkanRequiredExtensions;
        }

    private:


        void CreateInstance()
        {
            XrApplicationInfo AI;
            strncpy(AI.applicationName, "OpenXR Tutorial Chapter 2", XR_MAX_APPLICATION_NAME_SIZE);
            AI.applicationVersion = 1;
            strncpy(AI.engineName, "OpenXR Engine", XR_MAX_ENGINE_NAME_SIZE);
            AI.engineVersion = 1;
            AI.apiVersion = XR_CURRENT_API_VERSION;

            m_instanceExtensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
            m_instanceExtensions.push_back(XR_KHR_VULKAN_ENABLE_EXTENSION_NAME);

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
                if (!found)
                {
                    LOG_ERROR("Failed to find OpenXR instance extension: {}", requestedInstanceExtension);
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

        void DestroyInstance() {
            OPENXR_CHECK(xrDestroyInstance(m_xrInstance), "Failed to destroy Instance.");
        }

        void CreateDebugMessenger() {
            // Check that "XR_EXT_debug_utils" is in the active Instance Extensions before creating an XrDebugUtilsMessengerEXT.
            if (Utils::IsStringInVector(m_activeInstanceExtensions, XR_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
                m_debugUtilsMessenger = Utils::CreateOpenXRDebugUtilsMessenger(m_xrInstance);  // From OpenXRDebugUtils.h.
            }
        }

        void DestroyDebugMessenger() {
            // Check that "XR_EXT_debug_utils" is in the active Instance Extensions before destroying the XrDebugUtilsMessengerEXT.
            if (m_debugUtilsMessenger != XR_NULL_HANDLE) {
                Utils::DestroyOpenXRDebugUtilsMessenger(m_xrInstance, m_debugUtilsMessenger);  // From OpenXRDebugUtils.h.
            }
        }

        void GetInstanceProperties() {
            XrInstanceProperties instanceProperties{ XR_TYPE_INSTANCE_PROPERTIES };
            OPENXR_CHECK(xrGetInstanceProperties(m_xrInstance, &instanceProperties), "Failed to get InstanceProperties.");

            std::cout << "OpenXR Runtime: " << instanceProperties.runtimeName << " - "
                << XR_VERSION_MAJOR(instanceProperties.runtimeVersion) << "."
                << XR_VERSION_MINOR(instanceProperties.runtimeVersion) << "."
                << XR_VERSION_PATCH(instanceProperties.runtimeVersion);
        }

        void GetSystemID() {
            // Get the XrSystemId from the instance and the supplied XrFormFactor.
            XrSystemGetInfo systemGI{ XR_TYPE_SYSTEM_GET_INFO };
            systemGI.formFactor = m_formFactor;
            OPENXR_CHECK(xrGetSystem(m_xrInstance, &systemGI, &m_systemID), "Failed to get SystemID.");

            // Get the System's properties for some general information about the hardware and the vendor.
            OPENXR_CHECK(xrGetSystemProperties(m_xrInstance, m_systemID, &m_systemProperties), "Failed to get SystemProperties.");
        }

        void CreateSession()
        {
            XrSessionCreateInfo sessionCI{ XR_TYPE_SESSION_CREATE_INFO };

            sessionCI.next = Renderer::GetGraphicsBinding();
            sessionCI.createFlags = 0;
            sessionCI.systemId = m_systemID;

            OPENXR_CHECK(xrCreateSession(m_xrInstance, &sessionCI, &m_session), "Failed to create Session.");
        }

        void DestroySession()
        {
            OPENXR_CHECK(xrDestroySession(m_session), "Failed to destroy Session.");
        }

        void PollEvents()
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

        void PollSystemEvents()
        {
        }

        void LoadPFN_XrFunctions()
        {
            OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetVulkanGraphicsRequirementsKHR", (PFN_xrVoidFunction*)&xrGetVulkanGraphicsRequirementsKHR), "Failed to get InstanceProcAddr for xrGetVulkanGraphicsRequirementsKHR.");
            OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetVulkanInstanceExtensionsKHR", (PFN_xrVoidFunction*)&xrGetVulkanInstanceExtensionsKHR), "Failed to get InstanceProcAddr for xrGetVulkanInstanceExtensionsKHR.");
            OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetVulkanDeviceExtensionsKHR", (PFN_xrVoidFunction*)&xrGetVulkanDeviceExtensionsKHR), "Failed to get InstanceProcAddr for xrGetVulkanDeviceExtensionsKHR.");
            OPENXR_CHECK(xrGetInstanceProcAddr(m_xrInstance, "xrGetVulkanGraphicsDeviceKHR", (PFN_xrVoidFunction*)&xrGetVulkanGraphicsDeviceKHR), "Failed to get InstanceProcAddr for xrGetVulkanGraphicsDeviceKHR.");
        }


        // XR_DOCS_TAG_BEGIN_GraphicsAPI_Vulkan_GetInstanceExtensionsForOpenXR
        std::vector<std::string> GetInstanceExtensionsForOpenXR(XrInstance m_xrInstance, XrSystemId systemId) {
            uint32_t extensionNamesSize = 0;
            OPENXR_CHECK(xrGetVulkanInstanceExtensionsKHR(m_xrInstance, systemId, 0, &extensionNamesSize, nullptr), "Failed to get Vulkan Instance Extensions.");

            std::vector<char> extensionNames(extensionNamesSize);
            OPENXR_CHECK(xrGetVulkanInstanceExtensionsKHR(m_xrInstance, systemId, extensionNamesSize, &extensionNamesSize, extensionNames.data()), "Failed to get Vulkan Instance Extensions.");

            std::stringstream streamData(extensionNames.data());
            std::vector<std::string> extensions;
            std::string extension;
            while (std::getline(streamData, extension, ' ')) {
                extensions.push_back(extension);
            }
            return extensions;
        }

        void GenerateVulkanRequiredExtensions()
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
        }

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

        PFN_xrGetVulkanGraphicsRequirementsKHR xrGetVulkanGraphicsRequirementsKHR;
        PFN_xrGetVulkanInstanceExtensionsKHR xrGetVulkanInstanceExtensionsKHR;
        PFN_xrGetVulkanDeviceExtensionsKHR xrGetVulkanDeviceExtensionsKHR;
        PFN_xrGetVulkanGraphicsDeviceKHR xrGetVulkanGraphicsDeviceKHR;


    };
}