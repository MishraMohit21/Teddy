#include "tdpch.h"
#include "Rendrer.h"

#include "Teddy/Renderer/RendererCommand.h"

namespace Teddy
{
	void Renderer::BeginScene()
	{
	}

	void Renderer::EndScene()
	{
	}
	
	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		RendererCommand::DrawIndexed(vertexArray);
	}
}