#pragma once

#include <Teddy/Renderer/Texture.h>


namespace Teddy
{
	class OpneGLTexture2D : public Texture2D
	{
	public:
		OpneGLTexture2D(const std::string path);
		virtual ~OpneGLTexture2D();


		virtual uint32_t GetWidth() const  override { return m_Width; }
		virtual uint32_t GetHeight() const override {
			return m_Height;
		}

		virtual void Bind(uint32_t slot = 0) const override;

	private:

		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;

	};
}