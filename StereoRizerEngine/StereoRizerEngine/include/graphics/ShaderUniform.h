#pragma once
#include <string>
#include <memory>

#include <core/Utils.h>
#include <graphics/Shader.h>
#include <graphics/Texture.h>

namespace stereorizer::graphics
{
	class Material;

	class IShaderUniform
	{
	public:
		Material& material;
		const std::string name;
		IShaderUniform(Material& material, const std::string& name);

		virtual std::unique_ptr<IShaderUniform> clone(Material& material, const std::unique_ptr<IShaderUniform>& other) = 0;
		virtual void updateUniform(bool wasLast) = 0;
	};

	template<typename T>
	class DefShaderUniform : public IShaderUniform
	{
	protected:
		T value;
	public:
		DefShaderUniform(Material& material, const std::string& name, const T& value) : IShaderUniform(material, name), value(value)
		{
		}
		void set(const T& newValue)
		{
			this->value = newValue;
			updateUniform(true);
		}
		const T& get() const
		{
			return value;
		}
		virtual void updateUniform(bool newVal) override = 0;
	};


	class TextureShaderUniform : public DefShaderUniform<Texture2D*>
	{
	private:
		int32_t texLoc;
	public:
		TextureShaderUniform(Material& material, const std::string& name, Texture2D* const& value);
		std::unique_ptr<IShaderUniform> clone(Material& material, const std::unique_ptr<IShaderUniform>& other) override;
		void updateUniform(bool wasLast) override;
	};

	template<typename T>
	class ShaderUniform : public DefShaderUniform<T>
	{
	public:
		ShaderUniform(Material& material, const std::string& name, const T& value);
		std::unique_ptr<IShaderUniform> clone(Material& material, const std::unique_ptr<IShaderUniform>& other) override;
		void updateUniform(bool newVal) override;
	};
}

#define templateUniforms()