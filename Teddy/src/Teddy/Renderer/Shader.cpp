#include "tdpch.h"
#include "Shader.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Rendrer.h"

namespace Teddy
{
		
	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    TD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return new OpenGLShader(vertexSrc, fragmentSrc);
		}

		TD_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}