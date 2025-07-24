#pragma once
#include "Event.h"
#include "VulkanEngine/Input/Keycode.h"

namespace CHIKU
{
    class  KeyEvent : public Event
    {
    public:
        KeyEvent(KeyCode keycode) : m_KeyCode(keycode) {}
        KeyCode GetKeyCode() const { return m_KeyCode; }

        int GetCategoryFlags() const override { return EventCategoryKeyboard | EventCategoryInput; }

    protected:
        KeyCode m_KeyCode;
    };

    class KeyPressedEvent : public KeyEvent
    {
    public:
        KeyPressedEvent(KeyCode keycode) : KeyEvent(keycode) {}

        EventType GetEventType() const override { return EventType::KeyPressed; }
        const char* GetName() const override { return "KeyPressed"; }
    };

    class KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(KeyCode keycode) : KeyEvent(keycode) {}

        EventType GetEventType() const override { return EventType::KeyReleased; }
        const char* GetName() const override { return "KeyReleased"; }
    };

    class KeyTypedEvent : public KeyEvent
    {
    public:
        KeyTypedEvent(KeyCode keycode) : KeyEvent(keycode) {}

        EventType GetEventType() const override { return EventType::KeyTyped; }
        const char* GetName() const override { return "KeyTyped"; }
    }
}