#include "tdpch.h"
#include "Texture.h"

#include "Teddy/Renderer/Rendrer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Teddy
{
	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    TD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpneGLTexture2D>(path);
		}

		TD_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}