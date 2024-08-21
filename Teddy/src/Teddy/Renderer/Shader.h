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

		virtual const std::string GetName() = 0;

		static Ref<Shader> Create(const std::string& path);
		static Ref<Shader> Create(const std ::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	};
	 
	class ShaderLibrary
	{
	public:
		void Add(const std::string& name, const Ref<Shader>& shader);
		void Add(const Ref<Shader>& shader);
		Ref<Shader> Load(const std::string& filepath);
		Ref<Shader> Load(const std::string& name, const std::string& filepath);

		Ref<Shader> Get(const std::string& name);

		bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};

}

