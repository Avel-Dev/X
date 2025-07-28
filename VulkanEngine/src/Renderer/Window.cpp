#include "Window.h"
#include "Core/Application.h"
#include "Core/Events/KeyEvents.h"
#include "Core/Events/MouseEvents.h"
#include "Core/Events/WindowEvents.h"

namespace CHIKU
{
	uint32_t Window::WIDTH = 1280;
	uint32_t Window::HEIGHT = 720;

	void Window::Init()
	{
		ZoneScoped;
		InitWindow();
	}

	void Window::Create()
	{
		ZoneScoped;
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		m_Window = glfwCreateWindow(WIDTH, HEIGHT, m_Title, nullptr, nullptr);

		InitWindow();
	}

	void Window::InitWindow()
	{
		glfwSetWindowUserPointer(m_Window, &Application::GetApplicationData()); // inside YourAppClass::initWindow or similar

		glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				auto app = reinterpret_cast<ApplicationData*>(glfwGetWindowUserPointer(window));
				app->framebufferResized = true;
				WindowResizeEvent event(width,height);
				app->eventHandler(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				auto app = reinterpret_cast<ApplicationData*>(glfwGetWindowUserPointer(window));
				WindowCloseEvent event;
				app->eventHandler(event);
			});	

		glfwSetKeyCallback(m_Window,[](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				auto app = reinterpret_cast<ApplicationData*>(glfwGetWindowUserPointer(window));
				if (action == GLFW_PRESS)
				{
					KeyPressedEvent event(static_cast<KeyCode>(key));
					app->eventHandler(event);
				}
				else if (action == GLFW_RELEASE)
				{
					KeyReleasedEvent event(static_cast<KeyCode>(key));
					app->eventHandler(event);
				}
				else if (action == GLFW_REPEAT)
				{
					KeyTypedEvent event(static_cast<KeyCode>(key));
					app->eventHandler(event);
				}

			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				auto app = reinterpret_cast<ApplicationData*>(glfwGetWindowUserPointer(window));
				if (action == GLFW_PRESS)
				{
					MousePressedEvent event(static_cast<MouseCode>(button));
					app->eventHandler(event);
				}
				else if (action == GLFW_RELEASE)
				{
					MouseReleasedEvent event(static_cast<MouseCode>(button));
					app->eventHandler(event);
				}
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
			{
				auto app = reinterpret_cast<ApplicationData*>(glfwGetWindowUserPointer(window));
				MouseMovedEvent event(xpos, ypos);
				app->eventHandler(event);
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				auto app = reinterpret_cast<ApplicationData*>(glfwGetWindowUserPointer(window));
				MouseScrolledEvent event(xoffset, yoffset);
				app->eventHandler(event);
			});
	}
}