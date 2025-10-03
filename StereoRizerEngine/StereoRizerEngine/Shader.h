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

class Shader
{
private:
	unsigned int _rendererID;
	std::string _filePath;
	fs::file_time_type _lastWriteTime;

public:
	Shader(const std::string& filepath);
	~Shader();
	void Bind() const;
	void Unbind() const;
	void ReloadIfChanged();
	//void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

private:
	//int GetUniformLocation(const std::string& name);
	unsigned int CompileShader(const std::string& source, unsigned int type);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	ShaderProgramSource ParseShader(const std::string& filepath);
	fs::file_time_type GetLastWriteTime();
};