#pragma once
#include "Event.h"


namespace CHIKU
{
#define WINDOW_RESIZE_EVENT STR(WindowResize)
#define WINDOW_CLOSE_EVENT STR(WindowClose)

	class WindowEvent : public Event
	{
	public:
		WindowEvent() = default;
		int GetCategoryFlags() const override { return EventCategoryApplication ; }
	};

	class WindowResizeEvent : public WindowEvent
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height) : WindowEvent() 
		{
			m_Width = width;
			m_Height = height;
		}

		EventType GetEventType() const override { return EventType::WindowResize; }
		const char* GetName() const override { return WINDOW_RESIZE_EVENT; }

		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
	};

	class WindowCloseEvent : public WindowEvent
	{
	public:
		WindowCloseEvent() : WindowEvent() {}

		EventType GetEventType() const override { return EventType::WindowClose; }
		const char* GetName() const override { return WINDOW_CLOSE_EVENT; }
	};
}