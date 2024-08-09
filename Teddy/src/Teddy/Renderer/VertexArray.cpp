#include "tdpch.h"
#include "VertexArray.h"

#include "Teddy/Renderer/Rendrer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Teddy 
{
	


	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    TD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return new OpenGLVertexArray();
		}

		TD_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}