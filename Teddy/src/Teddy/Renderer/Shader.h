#pragma once

#include "glm/glm.hpp"

namespace Teddy
{

	class Shader
	{
	public:
		//Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~Shader() = default;

		virtual void Bind() const  = 0;
		virtual void Unbind() const  = 0;

		static Shader* Create(const std::string& path);

		static Shader* Create(const std ::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	};
	 
	class ShaderLibrary
	{
	public:

		void Add(Ref<Shader>& shader);

		Ref<Shader> Load(const std::string& filePath);
		Ref<Shader> Load(const std::string& name, const std::string& filePath);
		

		Ref<Shader> Get(const std::string& name);

	private:

		std::unordered_map<const std::string, Ref<Shader>> m_Shaders;
	};

}

