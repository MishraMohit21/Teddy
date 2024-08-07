#include "tdpch.h"
#include "Shader.h"

#include <glad/glad.h>

namespace Teddy
{
	Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc)
	{

		// 
		// Create a empty shader (vertex shader) then,
		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		// Compile the vertex shader
		// Error Handling
	
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		const GLchar* source = vertexSrc.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		glCompileShader(vertexShader);

		GLint is_Compiled = false;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &is_Compiled);

		if (is_Compiled == GL_FALSE)
		{

			int maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(vertexShader);

			TD_CORE_ERROR("{0}", infoLog.data());
			TD_CORE_ASSERT(false, "Vertex shader compilation failure!");
			return;
		}

		// Now same process for fragment shader

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		source = fragmentSrc.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &is_Compiled);
		if (is_Compiled == GL_FALSE)
		{
			int maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(fragmentShader);

			TD_CORE_ERROR("{0}", infoLog.data());
			TD_CORE_ASSERT(false, "Fragment shader compilation failure!");
			return;
		}

		m_RendererID = glCreateProgram();
		GLuint program = m_RendererID;

		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			TD_CORE_ERROR("{0}", infoLog.data());
			TD_CORE_ASSERT(false, "Shader link failure!");
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);

	}
	
	Shader::~Shader()
	{
		glDeleteProgram(m_RendererID);
	}
	
	void Shader::Bind() const
	{
		glUseProgram(m_RendererID);
	}
	
	void Shader::UnBind() const
	{
		glUseProgram(0);
	}
}