#include <graphics/Material.h>
#include <graphics/ShaderUniform.h>

using namespace stereorizer::graphics;

uint32_t Material::materialCount = 0;

Material::Material(Shader &shader, const char *name) : materialID(materialCount++), shader(shader), name(name)
{
	//shader.setLastMaterial(materialID);
}

Material::Material(const Material &other, const char *name) : materialID(materialCount++), shader(other.shader), name(name)
{
	//shader.setLastMaterial(materialID);
	for(auto &it : other.uniforms)
	{
		uniforms.emplace(it.first, it.second->clone(*this, it.second));
	}
}

void Material::use()
{
	//shader.ReloadIfChanged();
	//shader.useIfNecessary();
	shader.Bind();
	//bool isLast = isLastMaterial();
	/*for(auto &it : uniforms)
	{
		it.second->updateUniform(false);
	}*/
	//shader.setLastMaterial(materialID);
}

//bool Material::isLastMaterial()
//{
	//return shader.getLastMaterial() == materialID;
//}

template<>
std::unique_ptr<IShaderUniform> &Material::createUniform<Texture2D *>(const char *name, Texture2D *const &value)
{
	return uniforms.emplace(name, std::make_unique<TextureShaderUniform>(*this, name, value)).first->second;
}

template<typename T>
std::unique_ptr<IShaderUniform> &Material::createUniform(const char *name, const T &value)
{
	return uniforms.emplace(name, std::make_unique<ShaderUniform<T>>(*this, name, value)).first->second;
}

#define tempCreateUniform(type) template std::unique_ptr<IShaderUniform> &Material::createUniform<type>(const char *name, const type &value)

tempCreateUniform(bool);
tempCreateUniform(int32_t);
tempCreateUniform(float);
tempCreateUniform(glm::vec1);
tempCreateUniform(glm::vec2);
tempCreateUniform(glm::vec3);
tempCreateUniform(glm::vec4);
tempCreateUniform(glm::mat4);