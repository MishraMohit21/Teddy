#pragma once
#include <Teddy/Renderer/FrameBuffer.h>
#include <glm/glm.hpp>

namespace Teddy
{


	class OpenGlFrameBuffer : public FrameBuffer
	{
	public:
		OpenGlFrameBuffer(const FrameBufferSpecification& spec);
		virtual	~OpenGlFrameBuffer();


		virtual void bind() override;
		virtual void unBind() override;

		virtual const FrameBufferSpecification& GetFrameBufferSpecification() const override { return m_Specification; }
		virtual const uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; }
		
		void Resize();

		virtual void NewSize(uint32_t width, uint32_t height) override;

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_ColorAttachment =  0, m_DepthAttachment = 0;
		FrameBufferSpecification m_Specification;
	};

}