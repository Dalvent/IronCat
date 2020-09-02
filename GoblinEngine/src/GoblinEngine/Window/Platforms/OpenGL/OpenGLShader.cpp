#include "pch.h"
#include "OpenGLShader.h"

#include <chrono> 
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#define TEST_COUNT 10000
namespace GoblinEngine
{
	OpenGLShader::OpenGLShader(const TextFile& glslFile)
	{
		auto shaderCode = PreProcess(glslFile.GetData());
		Compile(shaderCode[GL_VERTEX_SHADER], shaderCode[GL_FRAGMENT_SHADER]);
	}
	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(_id);
	}

	void OpenGLShader::Bind()
	{
		glUseProgram(_id);
	}
	void OpenGLShader::Unbind()
	{
		glUseProgram(0);
	}
	void OpenGLShader::SetUniformInt(const std::string& name, const int value)
	{
		GLint location = glGetUniformLocation(_id, name.c_str());
		glUniform1i(location, value);
	}
	void OpenGLShader::SetUniformVec3(const std::string& name, const glm::vec3& value)
	{
		GLint location = glGetUniformLocation(_id, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}
	void OpenGLShader::SetUniformVec4(const std::string& name, const glm::vec4& value)
	{
		GLint location = glGetUniformLocation(_id, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}
	void OpenGLShader::SetUniformMat3(const std::string& name, const glm::mat3& value)
	{
		GLint location = glGetUniformLocation(_id, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
	void OpenGLShader::SetUniformMat4(const std::string& name, const glm::mat4& value)
	{
		GLint location = glGetUniformLocation(_id, name.c_str()); https://www.youtube.com/watch?v=238A-bHaB20
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderCode(2);
		// Beginn and end of the viewing part of the shader code
		
		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		for (size_t i = 0; i < TEST_COUNT; i++)
		{
			size_t pos = source.find(typeToken, 0);
			while (pos != std::string::npos)
			{
				size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
				GE_LOG_CORE_ASSERT(eol != std::string::npos, "Syntax error");
				size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
				std::string type = source.substr(begin, eol - begin);
				GE_LOG_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

				size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
				GE_LOG_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
				pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

				shaderCode[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
			}

		}
		return shaderCode;
	}
	GLenum OpenGLShader::ShaderTypeFromString(std::string type)
	{
		if (type == "vertex") return GL_VERTEX_SHADER;
		if (type == "fragment") return GL_FRAGMENT_SHADER;

		GE_LOG_CORE_ASSERT(false, "Unown shader type, can't from {0} get OpenGL shader type!", type);
		return GLenum();
	}
	void OpenGLShader::Compile(const std::string& vertexCode, const std::string& fragmentCode)
	{
		int vertexShader = glCreateShader(GL_VERTEX_SHADER);

		const GLchar* source = (const GLchar*)vertexCode.c_str();
		glShaderSource(vertexShader, 1, &source, 0);
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLenght = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLenght);

			auto infoLog = std::vector<GLchar>(maxLenght);
			glGetShaderInfoLog(vertexShader, maxLenght, &maxLenght, &infoLog[0]);

			glDeleteShader(vertexShader);

			GE_LOG_CORE_ERROR("{0}", infoLog.data());
			GE_LOG_CORE_ERROR("Vertex shader compilation failure!");
			return;
		}

		int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		source = (const GLchar*)fragmentCode.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLenght = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLenght);

			auto infoLog = std::vector<GLchar>(maxLenght);
			glGetShaderInfoLog(fragmentShader, maxLenght, &maxLenght, &infoLog[0]);

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			GE_LOG_CORE_ERROR("{0}", infoLog.data());
			GE_LOG_CORE_ERROR("Fragment shader compilation failure!");
			return;
		}

		_id = glCreateProgram();

		glAttachShader(_id, vertexShader);
		glAttachShader(_id, fragmentShader);

		glLinkProgram(_id);

		GLint isLinked = 0;
		glGetProgramiv(_id, GL_LINK_STATUS, (int*)&isLinked);

		if (isLinked == GL_FALSE)
		{
			GLint maxLenght = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLenght);

			auto infoLog = std::vector<GLchar>(maxLenght);
			glGetShaderInfoLog(fragmentShader, maxLenght, &maxLenght, &infoLog[0]);

			glDeleteProgram(_id);

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			GE_LOG_CORE_ERROR("{0}", infoLog.data());
			GE_LOG_CORE_ERROR("Shader link failure!");
			return;
		}
		glDetachShader(_id, vertexShader);
		glDetachShader(_id, fragmentShader);
	}
}