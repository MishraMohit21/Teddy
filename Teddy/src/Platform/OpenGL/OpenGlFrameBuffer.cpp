#include "tdpch.h"
#include "OpenGlFrameBuffer.h"

#include <glad/glad.h>

namespace Teddy
{

	const uint32_t s_MaxFrameBufferSize = 8196;

	namespace Utils
	{

		static GLenum MultiSampleChecker(bool multiSample)
		{
			return multiSample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTexture(bool multiSample, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(MultiSampleChecker(multiSample), count, outID);
		}

		static void BindTexture(bool multiSample, uint32_t id)
		{
			glBindTexture(MultiSampleChecker(multiSample), id);
		}

		static void AttachColorTexture(uint32_t id, int sample, GLenum  internalFormat,GLenum format, uint32_t width, uint32_t height, int index)
		{
			bool multiSample = sample > 1;

			if (multiSample)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sample, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, MultiSampleChecker(multiSample), id, 0);
		}

		static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, MultiSampleChecker(multisampled), id, 0);
		}



		static bool isDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case FramebufferTextureFormat::DEPTH24STENCIL8: return true;
			}
			return false;
		}


		static GLenum OPENGLAPICONVERTER(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case FramebufferTextureFormat::RGBA8: return GL_RGBA8;
				case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
			}
			TD_CORE_ASSERT(false, "Not Valid Texture Format");
			return 0;
		}

	}

	OpenGlFrameBuffer::OpenGlFrameBuffer(const FrameBufferSpecification& spec)
		: m_Specification(spec)
	{

		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!Utils::isDepthFormat(spec.TextureFormat))
				m_ColorAttachmentSpecifications.emplace_back(spec);
			else
				m_DepthAttachmentSpecification = spec;
		}

		Invalidate();
	}


	OpenGlFrameBuffer::~OpenGlFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(1, &m_DepthAttachment);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
	}

	void OpenGlFrameBuffer::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void OpenGlFrameBuffer::unBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}


	void OpenGlFrameBuffer::ClearAttachmentValue(uint32_t id, int value)
	{

		auto& textureSpec = m_ColorAttachmentSpecifications[id];
		glClearTexImage(m_ColorAttachments[id], 0, Utils::OPENGLAPICONVERTER(textureSpec.TextureFormat), GL_INT, &value);
	}

	void OpenGlFrameBuffer::Invalidate()
	{

		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(1, &m_DepthAttachment);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());


			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	
		bool multisample = m_Specification.Samples > 1;

		if (m_ColorAttachmentSpecifications.size())
		{

			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			Utils::CreateTexture(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());

			for (size_t i = 0; i < m_ColorAttachmentSpecifications.size(); i++)
			{
				Utils::BindTexture(multisample, m_ColorAttachments[i]);
				switch (m_ColorAttachmentSpecifications[i].TextureFormat)
				{
				case FramebufferTextureFormat::RGBA8:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA8,  GL_RGBA, m_Specification.Width, m_Specification.Height, i);
					break;
				case FramebufferTextureFormat::RED_INTEGER:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_R32I, GL_RED_INTEGER, m_Specification.Width, m_Specification.Height, i);
					break;

				}
				
			}
		}

		if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
		{
			Utils::CreateTexture(multisample, &m_DepthAttachment, 1);
			Utils::BindTexture(multisample, m_DepthAttachment);
			switch (m_DepthAttachmentSpecification.TextureFormat)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:
				Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
				break;
			}
		}
		if (m_ColorAttachments.size() > 1)
		{
			//TD_CORE_ASSERT(m_ColorAttachments.size() <= 4);
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			// Only depth-pass
			glDrawBuffer(GL_NONE);
		}


		//glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
		//glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);
		//glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
		//glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
		//glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

		TD_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	int OpenGlFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;
	}

	void OpenGlFrameBuffer::NewSize(uint32_t width, uint32_t height)
	{

		if (width < 0 || height < 0 || width > s_MaxFrameBufferSize || height > s_MaxFrameBufferSize)
			return;

		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	}



}