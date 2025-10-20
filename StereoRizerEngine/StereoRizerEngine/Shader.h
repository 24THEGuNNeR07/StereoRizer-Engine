#pragma once
#include <GL/glew.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader {
public:
	explicit Shader(const std::string& filepath);
	~Shader();

	Shader(Shader&& other) noexcept;
	Shader& operator=(Shader&& other) noexcept;

	void Bind() const;
	void Unbind() const;
	bool ReloadIfChanged();
	GLuint GetID() const noexcept { return _rendererID; }
	std::string GetLastCompileLog() const { return _lastLog; }

private:
	GLuint _rendererID = 0;
	std::string _filePath;
	std::string _lastLog;
	fs::file_time_type _lastWriteTime;

	ShaderProgramSource ParseShader(const std::string& filepath);
	fs::file_time_type GetLastWriteTime();

	GLuint CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	GLuint CompileShader(const std::string& source, GLenum type);
};