#pragma once

#include "Teddy/Renderer/RendererAPI.h"

namespace Teddy
{

	class OpenGLRendererAPI : public RendererAPI
	{
	public:

		virtual void Init() override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void clear() override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
	};

}