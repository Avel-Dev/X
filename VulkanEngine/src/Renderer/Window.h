#pragma once
#include "EngineHeader.h"

namespace CHIKU
{
	class Window
	{
	public:
		static uint32_t WIDTH;
		static uint32_t HEIGHT;

	public:
		Window() = default;
		void Init();

		inline GLFWwindow* GetWindow() { return m_Window; }
		inline bool WindowShouldClose() const { return glfwWindowShouldClose(m_Window); }
		inline void WindowPoolEvent() const 
		{ 
			ZoneScoped;
			glfwPollEvents(); 
		}

		void Create();

		void SetTitle(const char* title) 
		{ 
			m_Title = title;
			glfwSetWindowTitle(m_Window,m_Title);
		}

		void SetSize(uint32_t width, uint32_t height) 
		{ 
			WIDTH = width; 
			HEIGHT = height; 
			glfwSetWindowSize(m_Window, WIDTH, HEIGHT); 
		}

	private:
		void InitWindow();

	private:
		const char* m_Title = "Chiku Engine";
		GLFWwindow* m_Window = nullptr;
	};
}