#pragma once

#include <glad.h>
#include <iostream>
#include <unordered_map>
#include <glm/glm.hpp>

#include <graphics/GLObject.h>
#include <core/Utils.h>
#include <glm/gtc/type_ptr.hpp>

namespace stereorizer::graphics
{
	class Shader : public GLObject
	{
	private:
		std::unordered_map<std::string, int32_t> uniformLocations;
		std::unordered_map<std::string, int> textureIndices;
		int32_t lastMaterial = -1;
	public:
		uint32_t addedTextureCount = 0;
		const uint32_t textureCount;
		Shader(const char* vtxPath, const char* fragPath); // TODO combine vtx frag shaders to a single file, read textureCount from there

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
		GLOBJ_OVERRIDE(Shader)
	};
}



