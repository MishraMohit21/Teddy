#include "tdpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Teddy {

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;

}