#pragma once

#include <Teddy/Renderer/GraphicContext.h>

struct GLFWwindow;

namespace Teddy
{

	class OpenGLContext : public GraphicContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void init() override;
		virtual void swapbuffer() override;

	private:
		GLFWwindow* m_windowHandle;
	};

}