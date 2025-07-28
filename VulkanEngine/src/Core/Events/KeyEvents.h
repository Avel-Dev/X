#pragma once
#include "Event.h"
#include "Input/Keycode.h"

namespace CHIKU
{
#define KEY_PRESSED_EVENT STR(KeyPressed)
#define KEY_RELEASED_EVENT STR(KeyReleased)
#define KEY_TYPED_EVENT STR(KeyTyped)

    class  KeyEvent : public Event
    {
    public:
        KeyEvent(KeyCode keycode) : m_KeyCode(keycode) {}
        KeyCode GetKeyCode() const { return m_KeyCode; }
        const char* GetKeyName() const { return KeyCodeToString(m_KeyCode); }

        int GetCategoryFlags() const override { return EventCategoryKeyboard | EventCategoryInput; }

    protected:
        KeyCode m_KeyCode;
    };

    class KeyPressedEvent : public KeyEvent
    {
    public:
        KeyPressedEvent(KeyCode keycode) : KeyEvent(keycode) {}

        EventType GetEventType() const override { return EventType::KeyPressed; }
        const char* GetName() const override { return KEY_PRESSED_EVENT; }
    };

    class KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(KeyCode keycode) : KeyEvent(keycode) {}

        EventType GetEventType() const override { return EventType::KeyReleased; }
        const char* GetName() const override { return KEY_RELEASED_EVENT; }
    };

    class KeyTypedEvent : public KeyEvent
    {
    public:
        KeyTypedEvent(KeyCode keycode) : KeyEvent(keycode) {}

        EventType GetEventType() const override { return EventType::KeyTyped; }
        const char* GetName() const override { return KEY_TYPED_EVENT; }
    };
}