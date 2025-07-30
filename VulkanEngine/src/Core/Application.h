#pragma once
#include "Renderer/Renderer.h"
#include "Window.h"
#include "Assets/ModelAsset.h"
#include "Core/Events/EventBus.h"

namespace CHIKU
{
	struct ApplicationData
	{
		bool framebufferResized = false;
		std::function<void(Event&)> eventHandler = nullptr;
	};

	class Application
	{
	public:
		Application();

		virtual void Init();
		virtual void Run();
		virtual void Render();
		virtual void CleanUp();

		template<typename EventType>
		static EventBus::HandlerID Subscribe(const EventBus::HandlerFunc<EventType>& handler)
		{
			return m_EventBus.Subscribe(handler);
		}

		template<typename EventType>
		static void Unsubscribe(const EventBus::HandlerID& handlerID)
		{
			m_EventBus.Unsubscribe<EventType>(handlerID);
		}
		
		static void Publish(Event& event)
		{
			m_EventBus.Publish(event);
		}

		static inline ApplicationData& GetApplicationData() { return s_Data; }

	protected:
		static EventBus m_EventBus;
		static ApplicationData s_Data;
		SHARED<ModelAsset> m_Model;


		Window m_Window;
		bool m_applicationRunning = true;
		bool m_sessionRunning = false;
	};

	Application* CreateApplication();
}