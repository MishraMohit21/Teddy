#include "tdpch.h"
#include "FrameBuffer.h"
#include "Teddy/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGlFrameBuffer.h"

namespace Teddy
{
	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    TD_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGlFrameBuffer>(spec);
		}

		TD_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}