#include <graphics/ShaderUniform.h>
#include <graphics/Material.h>

using namespace stereorizer::graphics;

IShaderUniform::IShaderUniform(Material &material, const std::string &name) : material(material), name(name)
{
}

template<typename T>
ShaderUniform<T>::ShaderUniform(Material &material, const std::string &name, const T &value) : DefShaderUniform<T>(material, name, value)
{
}
template<typename T>
std::unique_ptr<IShaderUniform>  ShaderUniform<T>::clone(Material &material, const std::unique_ptr<IShaderUniform> &other)
{
	return std::make_unique<ShaderUniform<T>>(material, other->name, ((ShaderUniform<T>*)other.get())->get());
}

template<typename T>
void ShaderUniform<T>::updateUniform(bool newVal)
{
	/*if((!newVal || (newVal && this->material.isLastMaterial())))
		this->material.shader.setUniform<T>("material." + this->name, this->value);*/
}

TextureShaderUniform::TextureShaderUniform(Material &material, const std::string &name, Texture2D *const &value) : DefShaderUniform<Texture2D *>(material, name, value)
{
	texLoc = material.shader.getTextureIndex(("material." + name).c_str());
}

std::unique_ptr<IShaderUniform> TextureShaderUniform::clone(Material &material, const std::unique_ptr<IShaderUniform> &other)
{
	return std::make_unique<TextureShaderUniform>(material, other->name, ((TextureShaderUniform *)other.get())->get());
}

void TextureShaderUniform::updateUniform(bool newVal)
{
	/*if((!newVal || (newVal && material.isLastMaterial())) && texLoc >= 0)
	{
		material.shader.use();
		glActiveTexture(GL_TEXTURE0 + texLoc);
		get()->use();
	}*/
}


template ShaderUniform<bool>;
template ShaderUniform<int32_t>;
template ShaderUniform<float>;
template ShaderUniform<glm::vec1>;
template ShaderUniform<glm::vec2>;
template ShaderUniform<glm::vec3>;
template ShaderUniform<glm::vec4>;
template ShaderUniform<glm::mat4>;