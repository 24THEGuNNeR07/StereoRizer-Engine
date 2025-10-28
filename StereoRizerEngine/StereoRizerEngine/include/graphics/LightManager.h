#pragma once
#include <graphics/Light.h>

#include <iostream>
#include <string>
#include <unordered_map>

namespace stereorizer::graphics
{
	class ILightType
	{
	public:
		std::vector<Shader*>* const litShaders;
		const std::string typeName;
		uint32_t maxLightCount;
		ILightType(const std::string& typeName, const uint32_t maxLightCount, std::vector<Shader*>* const litShaders);
		virtual void updateTransforms() = 0;
		virtual void free() = 0;
	};

	template<typename Type>
	class LightType : public ILightType
	{
	public:
		static LightType<Type>* singleton;
		std::vector<Type*> lights;
		LightType(const std::string& typeName, const uint32_t maxLightCount, std::vector<Shader*>* const litShaders);
		Type* reserveLight();
		void updateTransforms() override;
		void free() override;
	};

	class LightManager
	{
	private:
		std::unordered_map<std::string, ILightType*> lightTypes;
	public:
		std::vector<Shader*> litShaders;
		template<typename Type>
		LightType<Type>* createLightType(const std::string& lightName, uint32_t maxLightCount)
		{
			if (LightType<Type>::singleton != nullptr)
			{
				std::cout << "Light type " + lightName + " already exists, returning singleton" << std::endl;
				return LightType<Type>::singleton;
			}
			if (maxLightCount == 0)
			{
				std::cout << "Max Light Count can't be 0, initialize lightType " << lightName << " before adding light." << std::endl;
				return nullptr;
			}
			LightType<Type>* lightType = new LightType<Type>(lightName, maxLightCount, &litShaders);
			lightTypes[lightName] = lightType;
			return lightType;
		}
		template<typename Type>
		LightType<Type>* getLightType(const std::string& lightName)
		{
			if (LightType<Type>::singleton != nullptr)
			{
				return LightType<Type>::singleton;
			}

			std::cout << "Create light type " + lightName + " first." << std::endl;
			return nullptr;

		}
		/*
			Must call light.setup() before using the light
		*/
		template<typename Type>
		Type* reserveLight(const std::string& lightType)
		{
			LightType<Type>* type = getLightType<Type>(lightType);
			return type->reserveLight();
		}
		void updateLights();
		~LightManager();
	};
}

