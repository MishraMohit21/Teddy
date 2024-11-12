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
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { /*TD_CORE_ASSERT(index < m_ColorAttachments.size());*/ return m_ColorAttachments[index]; }
		
		virtual void ClearAttachmentValue(uint32_t id, int value) override;


		void Invalidate();
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;
		virtual void NewSize(uint32_t width, uint32_t height) override;

	private:
		uint32_t m_RendererID = 0;

		FrameBufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;

		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};

}