#pragma once

namespace Teddy
{

	struct FrameBufferSpecification
	{
		uint32_t Width, Height;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};

	class FrameBuffer
	{
	public: 

		virtual ~FrameBuffer() = default;

		virtual void bind() = 0;
		virtual void unBind() = 0;

		virtual void NewSize(uint32_t width, uint32_t height) = 0;


		virtual const FrameBufferSpecification& GetFrameBufferSpecification() const = 0;
		virtual const uint32_t GetColorAttachmentRendererID() const = 0;


		static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);
	};

}