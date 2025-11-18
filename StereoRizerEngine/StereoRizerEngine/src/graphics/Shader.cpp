#include "graphics/Shader.h"
#include "core/Common.h"

using namespace stereorizer::graphics;

Shader::Shader(const std::string& filepath)
{
	_filePath = filepath;
	_lastWriteTime = GetLastWriteTime();

	ShaderProgramSource source = ParseShader(filepath);
	_rendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::Shader(const std::string& filepath, const std::unordered_map<std::string, std::string>& defines)
{
	_filePath = filepath;
	_defines = defines;
	_lastWriteTime = GetLastWriteTime();

	ShaderProgramSource source = ParseShader(filepath);
	// Inject defines into both vertex and fragment shaders
	source.VertexSource = InjectDefines(source.VertexSource);
	source.FragmentSource = InjectDefines(source.FragmentSource);
	_rendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	glDeleteProgram(_rendererID);
}

Shader::Shader(Shader&& other) noexcept
{
	_rendererID = other._rendererID;
	_filePath = std::move(other._filePath);
	_lastWriteTime = other._lastWriteTime;
	_defines = std::move(other._defines);

	other._rendererID = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
	if (this != &other) {
		glDeleteProgram(_rendererID);
		_rendererID = other._rendererID;
		_filePath = std::move(other._filePath);
		_lastWriteTime = other._lastWriteTime;
		_defines = std::move(other._defines);

		other._rendererID = 0;
	}
	return *this;
}

void Shader::Bind() const
{
	glUseProgram(_rendererID);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

bool Shader::ReloadIfChanged()
{
	fs::file_time_type currentWriteTime = GetLastWriteTime();
	if (currentWriteTime != _lastWriteTime) {
		_lastWriteTime = currentWriteTime;
	LOG_INFO("Reloading shader...");

		ShaderProgramSource newSource = ParseShader(_filePath);
		// Inject defines if they exist
		if (!_defines.empty()) {
			newSource.VertexSource = InjectDefines(newSource.VertexSource);
			newSource.FragmentSource = InjectDefines(newSource.FragmentSource);
		}
		unsigned int newShader = CreateShader(newSource.VertexSource, newSource.FragmentSource);
		if (newShader != 0) {
			glDeleteProgram(_rendererID);
			_rendererID = newShader;
			glUseProgram(_rendererID);
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

GLuint Shader::CompileShader(const std::string& source, GLenum type)
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
	std::vector<char> message(length);
	glGetShaderInfoLog(id, length, nullptr, message.data());
	LOG_ERROR(std::string("Failed to compile ") + (type == GL_VERTEX_SHADER ? "vertex" : "fragment") + " shader!");
	LOG_ERROR(std::string(message.data()));
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
		LOG_ERROR(std::string("Error getting last write time for shader file: ") + e.what());
		return fs::file_time_type::min();
	}
}

std::string Shader::InjectDefines(const std::string& source)
{
	if (_defines.empty()) {
		return source;
	}

	std::stringstream ss;
	std::istringstream sourceStream(source);
	std::string line;
	bool versionFound = false;

	// Read through the source line by line
	while (std::getline(sourceStream, line)) {
		// Add the line first
		ss << line << "\n";

		// If this is the version directive, inject defines after it
		if (!versionFound && line.find("#version") != std::string::npos) {
			versionFound = true;
			// Inject all defines after the version line
			for (const auto& define : _defines) {
				ss << "#define " << define.first;
				if (!define.second.empty()) {
					ss << " " << define.second;
				}
				ss << "\n";
			}
		}
	}

	// If no version directive was found, prepend defines at the beginning
	if (!versionFound) {
		std::string result;
		for (const auto& define : _defines) {
			result += "#define " + define.first;
			if (!define.second.empty()) {
				result += " " + define.second;
			}
			result += "\n";
		}
		result += source;
		return result;
	}

	return ss.str();
}

void Shader::EnableDefine(const std::string& name, const std::string& value)
{
	_defines[name] = value;
}

void Shader::DisableDefine(const std::string& name)
{
	auto it = _defines.find(name);
	if (it != _defines.end()) {
		_defines.erase(it);
	}
}

bool Shader::HasDefine(const std::string& name) const
{
	return _defines.find(name) != _defines.end();
}

void Shader::ClearAllDefines()
{
	_defines.clear();
}

bool Shader::RecompileWithDefines()
{
	ShaderProgramSource source = ParseShader(_filePath);
	// Apply defines if they exist
	if (!_defines.empty()) {
		source.VertexSource = InjectDefines(source.VertexSource);
		source.FragmentSource = InjectDefines(source.FragmentSource);
	}

	unsigned int newShader = CreateShader(source.VertexSource, source.FragmentSource);
	if (newShader != 0) {
		glDeleteProgram(_rendererID);
		_rendererID = newShader;
		glUseProgram(_rendererID);
		return true;
	}
	else {
		LOG_ERROR("Failed to recompile shader with defines");
		return false;
	}
}
