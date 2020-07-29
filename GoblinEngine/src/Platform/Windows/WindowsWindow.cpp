#include "pch.h"
#include "WindowsWindow.h"

#include "Engine/Event/AppEvent.h"
#include "Engine/Event/KeyEvent.h"
#include "Engine/Event/MouseEvent.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace GoblinEngine
{
	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}
	
	WindowsWindow::~WindowsWindow()
	{
		Shutdow();
	}
	
	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(_glfwWindow);
	}
	
	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
		{
			glfwSwapInterval(1);
		}
		else
		{
			glfwSwapInterval(0);
		}

		_data.vSync = enabled;
	}
	
	bool WindowsWindow::IsVSync() const
	{
		return _data.vSync;
	}


	void WindowsWindow::Init(const WindowProps& windowProps)
	{
		_data.title = windowProps.title;
		_data.width = windowProps.width;
		_data.height = windowProps.height;

		GE_CORE_INFO("Creating window {0} ({1} {2})", 
			_data.title, _data.width, _data.height);

		if (GLFW_NOT_INITIALIZED)
		{
			int success = glfwInit();
			GE_CORE_ASSERT(success, "Could not intialize GLFW!")
			
			glfwSetErrorCallback([](int error, const char* description) 
			{
				GE_CORE_ERROR("GLFW ERROR ({0}): {1}", error, description)
			});
		}

		_glfwWindow = glfwCreateWindow((int)_data.width, (int)_data.height, _data.title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(_glfwWindow);
		
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)
		GE_CORE_ASSERT("Failed to initialize GLAD!", status);

		glfwSetWindowUserPointer(_glfwWindow, &_data);
		SetVSync(true);

		glfwSetWindowSizeCallback(_glfwWindow, [](GLFWwindow* window, int width, int height)
		{
			auto& _data = *(WindowData*)glfwGetWindowUserPointer(window);
			_data.width = width;
			_data.height = height;

			auto event = WindowResizeEvent(width, height);
			_data.eventCallback(event);
		});

		glfwSetWindowCloseCallback(_glfwWindow, [](GLFWwindow* window)
		{
			auto& _data = *(WindowData*)glfwGetWindowUserPointer(window);
			auto event = WindowCloseEvent();
			_data.eventCallback(event);
		});

		glfwSetKeyCallback(_glfwWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			auto& _data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			KeyEvent* event;

			switch (scancode)
			{
			case GLFW_PRESS:
				event = &KeyPressedEvent(key, 0);
				break;
			case GLFW_RELEASE:
				event = &KeyReleasedEvent(key);
				break;
			case GLFW_REPEAT:
				event = &KeyPressedEvent(key, 1);
				break;
			default:
				return;
			}

			_data.eventCallback(*event);
		});

		glfwSetMouseButtonCallback(_glfwWindow, [](GLFWwindow* window, int button, int action, int mods)
		{
			auto& _data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseButtonEvent* event;

			switch (action)
			{
			case GLFW_PRESS:
				event = &MouseButtonPressedEvent(button);
				break;
			case GLFW_RELEASE:
				event = &MouseButtonReleasedEvent(button);
				break;
			default:
				break;
			}

			_data.eventCallback(*event);
		});

		glfwSetScrollCallback(_glfwWindow, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			auto& _data = *(WindowData*)glfwGetWindowUserPointer(window);
			auto event = MouseMovedEvent(xOffset, yOffset);
			_data.eventCallback(event);
		});

		glfwSetCursorPosCallback(_glfwWindow, [](GLFWwindow* window, double xPos, double yPos)
		{
			auto& _data = *(WindowData*)glfwGetWindowUserPointer(window);
			auto event = MouseMovedEvent(xPos, yPos);
			_data.eventCallback(event);
		});
	}
	void WindowsWindow::Shutdow()
	{
		glfwPollEvents();
	}
}