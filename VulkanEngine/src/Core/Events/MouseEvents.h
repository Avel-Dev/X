#pragma once
#include "Event.h"
#include "Input/MouseCode.h"

namespace CHIKU
{
#define MOUSE_PRESSED_EVENT STR(MouseButtonPressed)
#define MOUSE_RELEASED_EVENT STR(MouseButtonReleased)
#define MOUSE_MOVED_EVENT STR(MouseMoved)
#define MOUSE_SCROLLED_EVENT STR(MouseScrolled)

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(double xpos, double ypos)
		{
			X = xpos;
			Y = ypos;
		}

		int GetCategoryFlags() const override { return EventCategoryMouse | EventCategoryInput; }
		EventType GetEventType() const override { return EventType::MouseMoved; }
		const char* GetName() const override { return MOUSE_MOVED_EVENT; }

		double X = 0.0f;
		double Y = 0.0f;
	};


	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(double xoffset, double yoffset)
		{
			XOffset = xoffset;
			YOffset = yoffset;
		}

		int GetCategoryFlags() const override { return EventCategoryMouse | EventCategoryInput; }
		EventType GetEventType() const override { return EventType::MouseScrolled; }
		const char* GetName() const override { return MOUSE_SCROLLED_EVENT; }

		double XOffset = 0;
		double YOffset = 0;
	};

	class MouseButtonEvent : public Event
	{
	public:
		MouseButtonEvent(MouseCode button) : m_Button(button) {}
		MouseCode GetMouseCode() const { return m_Button; }

		int GetCategoryFlags() const override { return EventCategoryMouse | EventCategoryInput; }

	protected:
		MouseCode m_Button;
	};

	class MousePressedEvent : public MouseButtonEvent
	{
	public:
		MousePressedEvent(MouseCode button) : MouseButtonEvent(button) {}

		EventType GetEventType() const override { return EventType::MouseButtonPressed; }
		const char* GetName() const override { return MOUSE_PRESSED_EVENT; }
	};

	class MouseReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseReleasedEvent(MouseCode button) : MouseButtonEvent(button) {}

		EventType GetEventType() const override { return EventType::MouseButtonReleased; }
		const char* GetName() const override { return MOUSE_RELEASED_EVENT; }
	};

}