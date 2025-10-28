#pragma once

#include <glad.h>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <glm/glm.hpp>

#include <graphics/GLObject.h>
#include <core/Common.h>
#include <core/Utils.h>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>

namespace stereorizer::graphics
{
	struct ShaderProgramSource {
		std::string VertexSource;
		std::string FragmentSource;
	};

	class Shader : public GLObject
	{
	private:
		std::unordered_map<std::string, int32_t> uniformLocations;
		std::unordered_map<std::string, int> textureIndices;
		int32_t lastMaterial = -1;
		std::string _filePath;
		std::filesystem::file_time_type _lastWriteTime;

	public:
		uint32_t addedTextureCount = 0;
		const uint32_t textureCount;
		Shader(const std::string& file);

		bool ReloadIfChanged();
		GLuint CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
		GLuint CompileShader(const std::string& source, GLenum type);

		int32_t getUniformLocation(const std::string& name);
		//adds to the texture indices if texture doesn't exist
		int32_t getTextureIndex(const char* name);
		void setLastMaterial(int32_t material);
		int32_t getLastMaterial();
		template <typename T>
		void setUniform(const int32_t location, const T& val);
		template <typename T>
		void setUniform(const std::string& name, const T& val)
		{
			int32_t location = getUniformLocation(name);
			if (location != -1)
				setUniform(location, val);
		}
		ShaderProgramSource ParseShader(const std::string& filepath);
		std::filesystem::file_time_type GetLastWriteTime();
		GLOBJ_OVERRIDE(Shader)
	};
}



