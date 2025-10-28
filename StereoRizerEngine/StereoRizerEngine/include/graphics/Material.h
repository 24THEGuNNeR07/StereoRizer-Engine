#pragma once
#include <string>
#include <unordered_map>
#include <graphics/Shader.h>
#include <graphics/ShaderUniform.h>

#include <memory>

namespace stereorizer::graphics
{
	struct MaterialSettings
	{

	};

	class Material
	{
	private:
		std::unordered_map<std::string, std::unique_ptr<IShaderUniform>> uniforms;
	public:
		static uint32_t materialCount;
		const uint32_t materialID;
		Shader& shader;
		const std::string name;
		Material(Shader& shader, const char* name);
		Material(const Material& other, const char* name);
		Material(const Material& other) = delete;
		void use();

		bool isLastMaterial();

		template<typename T>
		void setUniform(const char* name, const T& value)
		{
			auto it = uniforms.find(name);
			if (it == uniforms.end())
			{
				createUniform<T>(name, value)->updateUniform(true);
				return;
			}
			((ShaderUniform<T>*)it->second.get())->set(value);
		}
	private:
		template<typename T>
		std::unique_ptr<IShaderUniform>& createUniform(const char* name, const T& value);
	};
}