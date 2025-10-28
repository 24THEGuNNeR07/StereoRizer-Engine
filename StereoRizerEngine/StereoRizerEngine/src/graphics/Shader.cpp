#pragma once
#include <graphics/Shader.h>

using namespace stereorizer::graphics;
using namespace stereorizer::core;

Shader::Shader(const char *vtxPath, const char *fragPath)
	: textureCount(textureCount)
{
	// 1. read code from file
	std::string vtxShaderCodeStr = FileUtils::readFile(vtxPath);
	std::string fragShaderCodeStr = FileUtils::readFile(fragPath);

	const char *vtxShaderCode = vtxShaderCodeStr.c_str();
	const char *fragShaderCode = fragShaderCodeStr.c_str();

	// 2. compile shaders

	uint32_t vtx, frag;
	int32_t success;
	char infoLog[512];

	vtx = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vtx, 1, &vtxShaderCode, NULL);
	glCompileShader(vtx);

	glGetShaderiv(vtx, GL_COMPILE_STATUS, &success);

	if(!success)
	{
		glGetShaderInfoLog(vtx, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &fragShaderCode, NULL);
	glCompileShader(frag);

	glGetShaderiv(frag, GL_COMPILE_STATUS, &success);

	if(!success)
	{
		glGetShaderInfoLog(frag, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	id = glCreateProgram();
	glAttachShader(id, vtx);
	glAttachShader(id, frag);
	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &success);

	if(!success)
	{
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vtx);
	glDeleteShader(frag);
	use();
}

int32_t Shader::getUniformLocation(const std::string &name)
{
	if(uniformLocations.find(name) != uniformLocations.end())
		return uniformLocations[name];
	useIfNecessary();
	int32_t loc = glGetUniformLocation(id, name.c_str());
	if(loc == -1)
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
	uniformLocations[name] = loc;
	return loc;
}

int32_t Shader::getTextureIndex(const char *name)
{
	int32_t texIndex;
	auto it = textureIndices.find(name);
	if(it == textureIndices.end())
	{
		texIndex = (int32_t)textureIndices.size();
		setUniform<int32_t>(name, texIndex);
		if(getUniformLocation(name) == -1)
		{
			return -1;
		}
		textureIndices.insert({name, texIndex});
		return texIndex;
	}
	else
	{
		return it->second;
	}
}

void Shader::setLastMaterial(int32_t material)
{
	lastMaterial = material;
}

int32_t Shader::getLastMaterial()
{
	return lastMaterial;
}

template<>
void Shader::setUniform(const int32_t location, const bool &value)
{
	useIfNecessary();
	glUniform1i(location, (int)value);
}template<>
void Shader::setUniform(const int32_t location, const int &value)
{
	useIfNecessary();
	glUniform1i(location, value);
}
template<>
void Shader::setUniform(const int32_t location, const float &val)
{
	useIfNecessary();
	glUniform1f(location, val);
}
template<>
void Shader::setUniform(const int32_t location, const glm::vec1 &val)
{
	useIfNecessary();
	glUniform1f(location, val.x);
}
template<>
void Shader::setUniform(const int32_t location, const glm::vec2 &val)
{
	useIfNecessary();
	glUniform2f(location, val.x, val.y);
}
template<>
void Shader::setUniform(const int32_t location, const glm::vec3 &val)
{
	useIfNecessary();
	glUniform3f(location, val.x, val.y, val.z);
}
template<>
void Shader::setUniform(const int32_t location, const glm::vec4 &val)
{
	useIfNecessary();
	glUniform4f(location, val.x, val.y, val.z, val.w);
}
template<>
void Shader::setUniform(const int32_t location, const glm::mat4 &val)
{
	useIfNecessary();
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(val));
}

GLOBJ_DEFAULTS(Shader, glUseProgram, glDeleteProgram);

GLOBJ_LAST_USED(Shader)