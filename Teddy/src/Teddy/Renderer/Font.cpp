#include "tdpch.h"
#include "Font.h"

#include <Trex/Atlas.hpp>

namespace Teddy
{
	Ref<Font> Font::s_DefaultFont;

	void Font::Init()
	{
		s_DefaultFont = Font::Create("assets/Fonts/openSans/OpenSans-Regular.ttf");
	}

	void Font::Shutdown()
	{
		s_DefaultFont.reset();
	}

	Ref<Font> Font::GetDefault()
	{
		return s_DefaultFont;
	}

	Font::Font(const std::string& filepath)
		: m_Filepath(filepath)
	{
		// Create the Trex Atlas from the font file.
		// For now, using a fixed size of 48. This could be a parameter later.
		m_Atlas = CreateRef<Trex::Atlas>(filepath.c_str(), 48);

		// Get atlas bitmap properties
		uint32_t width = m_Atlas->GetBitmap().Width();
		uint32_t height = m_Atlas->GetBitmap().Height();
		auto& pixels = m_Atlas->GetBitmap().Data();

		

		// Create a single-channel GPU texture
		m_AtlasTexture = Texture2D::Create(width, height, true);
		if (!pixels.empty())
		{
			m_AtlasTexture->SetData(pixels.data(), pixels.size());
		}
	}

	Ref<Font> Font::Create(const std::string& filepath)
	{
		return CreateRef<Font>(filepath);
	}
}
