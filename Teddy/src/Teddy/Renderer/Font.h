#pragma once

#include "Teddy/Core/Core.h"
#include "Teddy/Renderer/Texture.h"

#include <string>
#include <vector>

// Forward declaration
namespace Trex { class Atlas; }

namespace Teddy
{
	class Font
	{
	public:
		Font(const std::string& filepath);
		~Font() = default;

		static void Init();
		static void Shutdown();

		Ref<Trex::Atlas> GetAtlas() const { return m_Atlas; }
		Ref<Texture2D> GetAtlasTexture() const { return m_AtlasTexture; }
		const std::string& GetFilepath() const { return m_Filepath; }

		static Ref<Font> GetDefault();

		static Ref<Font> Create(const std::string& filepath);

	private:
		std::string m_Filepath;
		Ref<Trex::Atlas> m_Atlas;
		Ref<Texture2D> m_AtlasTexture;

		static Ref<Font> s_DefaultFont;
	};
}
