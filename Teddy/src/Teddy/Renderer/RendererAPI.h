#pragma once

#include "glm/glm.hpp"

#include "Teddy/Renderer/VertexArray.h"

namespace Teddy
{

	class RendererAPI
	{
	public:

		enum class API
		{
			None = 0, OpenGL
		};


	public:

		virtual void Init() = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void clear() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) = 0;

		static inline const API GetAPI() { return s_API; }

	private :
		static API s_API;

	};

}