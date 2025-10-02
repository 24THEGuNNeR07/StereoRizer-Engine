#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <chrono>

#include <Renderer.h>

namespace fs = std::filesystem;

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	fs::file_time_type m_LastWriteTime;

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

