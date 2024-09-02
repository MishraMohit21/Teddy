#include "tdpch.h"
#include "Teddy/Renderer/RenderCommand.h"

namespace Teddy {

	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();

}