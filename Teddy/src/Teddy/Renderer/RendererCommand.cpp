#include "tdpch.h"
#include "RendererCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Teddy
{
	RendererAPI* RendererCommand::s_RendererAPI = new OpenGLRendererAPI;
}