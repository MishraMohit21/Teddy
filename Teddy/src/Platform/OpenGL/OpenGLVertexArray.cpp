#include "tdpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Teddy
{

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case Teddy::ShaderDataType::Float:    return GL_FLOAT;
		case Teddy::ShaderDataType::Float2:   return GL_FLOAT;
		case Teddy::ShaderDataType::Float3:   return GL_FLOAT;
		case Teddy::ShaderDataType::Float4:   return GL_FLOAT;
		case Teddy::ShaderDataType::Mat3:     return GL_FLOAT;
		case Teddy::ShaderDataType::Mat4:     return GL_FLOAT;
		case Teddy::ShaderDataType::Int:      return GL_INT;
		case Teddy::ShaderDataType::Int2:     return GL_INT;
		case Teddy::ShaderDataType::Int3:     return GL_INT;
		case Teddy::ShaderDataType::Int4:     return GL_INT;
		case Teddy::ShaderDataType::Bool:     return GL_BOOL;
		}

		TD_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{	
		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}


	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}


	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}


	void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer> vertexBuffer)
	{
		glBindVertexArray(m_RendererID);

		vertexBuffer->Bind();

		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset);
			index++;
		}
		m_VertexBuffers.push_back(vertexBuffer);	
	}


	void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer> indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

}