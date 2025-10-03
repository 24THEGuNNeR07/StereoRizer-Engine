#include "Shader.h"

Shader::Shader(const std::string& filepath)
{
	_filePath = filepath;
	_lastWriteTime = GetLastWriteTime();

	ShaderProgramSource source = ParseShader(filepath);
	_rendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	glDeleteProgram(_rendererID);
}

void Shader::Bind() const
{
	glUseProgram(_rendererID);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::ReloadIfChanged()
{
	fs::file_time_type currentWriteTime = GetLastWriteTime();
	if (currentWriteTime != _lastWriteTime) {
		_lastWriteTime = currentWriteTime;
		std::cout << "Reloading shader...\n";

		ShaderProgramSource newSource = ParseShader(_filePath);
		unsigned int newShader = CreateShader(newSource.VertexSource, newSource.FragmentSource);
		if (newShader != 0) {
			glDeleteProgram(_rendererID);
			_rendererID = newShader;
			glUseProgram(_rendererID);
		}
	}
}

unsigned int Shader::CompileShader(const std::string& source, unsigned int type)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)_malloca(length * sizeof(char));
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;
	}

	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
	unsigned int fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType {
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (std::getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else {
			ss[(int)type] << line << "\n";
		}
	}

	return { ss[0].str(), ss[1].str() };
}

fs::file_time_type Shader::GetLastWriteTime()
{
	try {
		return fs::last_write_time(_filePath);
	}
	catch (fs::filesystem_error& e) {
		std::cerr << "Error getting last write time for shader file: " << e.what() << std::endl;
		return fs::file_time_type::min();
	}
}
