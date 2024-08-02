#include "tdpch.h"
#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
 
namespace Teddy
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) 
		: m_windowHandle(windowHandle)
	{
		TD_CORE_ASSERT(windowHandle , "WindowHandle in null or not defined");
	}
	void OpenGLContext::init()
	{

		glfwMakeContextCurrent(m_windowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		TD_CORE_ASSERT(status, "Failed to initialize Glad!");

		TD_CORE_INFO("OpenGL Info:");
		TD_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		TD_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		TD_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

	}
	void OpenGLContext::swapbuffer()
	{
		glfwSwapBuffers(m_windowHandle);
	}
}