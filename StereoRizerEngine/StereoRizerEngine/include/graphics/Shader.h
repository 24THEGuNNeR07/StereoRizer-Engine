#pragma once
#include <GL/glew.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <vector>
#include <unordered_map>

#include "core/Common.h"

namespace fs = std::filesystem;

namespace stereorizer::graphics
{
	struct ShaderProgramSource {
		std::string VertexSource;
		std::string FragmentSource;
	};

	class Shader {
	public:
		Shader(const std::string& filepath);
		Shader(const std::string& filepath, const std::unordered_map<std::string, std::string>& defines);
		~Shader();

		Shader(Shader&& other) noexcept;
		Shader& operator=(Shader&& other) noexcept;

		void Bind() const;
		void Unbind() const;
		bool ReloadIfChanged();
		GLuint GetID() const noexcept { return _rendererID; }

		// Define management methods
		void EnableDefine(const std::string& name, const std::string& value = "");
		void DisableDefine(const std::string& name);
		bool HasDefine(const std::string& name) const;
		void ClearAllDefines();
		bool RecompileWithDefines();

	private:
		GLuint _rendererID = 0;
		std::string _filePath;
		fs::file_time_type _lastWriteTime;
		std::unordered_map<std::string, std::string> _defines;

		ShaderProgramSource ParseShader(const std::string& filepath);
		fs::file_time_type GetLastWriteTime();
		std::string InjectDefines(const std::string& source);

		GLuint CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
		GLuint CompileShader(const std::string& source, GLenum type);
	};
}