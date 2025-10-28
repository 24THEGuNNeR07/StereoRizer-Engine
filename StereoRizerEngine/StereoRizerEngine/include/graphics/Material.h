#pragma once
#include <string>
#include <unordered_map>
#include <memory>

#include <graphics/Shader.h>

namespace stereorizer::graphics
{
	// Forward declaration
	class IShaderUniform;
	template<typename T>
	class ShaderUniform;

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

		//bool isLastMaterial();

		/*template<typename T>
		void setUniform(const char* name, const T& value);*/
	private:
		template<typename T>
		std::unique_ptr<IShaderUniform>& createUniform(const char* name, const T& value);
	};
}

// Include ShaderUniform.h after Material class definition to avoid circular dependency
//#include <graphics/ShaderUniform.h>
//
//namespace stereorizer::graphics
//{
//	// Template implementation (must be in header for templates)
//	template<typename T>
//	inline void Material::setUniform(const char* name, const T& value)
//	{
//		auto it = uniforms.find(name);
//		if (it == uniforms.end())
//		{
//			createUniform<T>(name, value).get()->updateUniform(true);
//			return;
//		}
//		((ShaderUniform<T>*)it->second.get())->set(value);
//	}
//}