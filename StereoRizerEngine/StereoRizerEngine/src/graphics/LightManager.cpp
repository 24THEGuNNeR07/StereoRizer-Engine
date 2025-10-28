#include <graphics/LightManager.h>

using namespace stereorizer::graphics;

ILightType::ILightType(const std::string &typeName, const uint32_t maxLightCount, std::vector<Shader *> *const litShaders) : typeName(typeName), maxLightCount(maxLightCount), litShaders(litShaders)
{
}

template<typename Type>
LightType<Type> *LightType<Type>::singleton;

template<typename Type>
LightType<Type>::LightType(const std::string &typeName, const uint32_t maxLightCount, std::vector<Shader *> *const litShaders) : ILightType(typeName, maxLightCount, litShaders)
{
	LightType<Type>::singleton = this;
	lights.reserve(maxLightCount);
	for(uint32_t i = 0; i < maxLightCount; i++)
	{
		lights.push_back(new Type(i, typeName, litShaders));
		((Light *)lights[i])->free();
	}
}

template<typename Type>
void LightType<Type>::updateTransforms()
{
	for(auto &light : lights)
		((Light *)light)->updateTransforms();
}


template<typename Type>
Type *LightType<Type>::reserveLight()
{
	int32_t suitable = -1;
	Type *selected = nullptr;
	for(uint32_t i = 0; i < maxLightCount; i++)
	{
		if(!((Light *)lights[i])->isActive())
		{
			suitable = i;
			selected = lights[i];
			break;
		}
	}

	if(suitable == -1)
	{
		std::cout << "No available light slot for light type: " << typeName << std::endl;
	}
	return selected;
}

template<typename Type>
void LightType<Type>::free()
{
	for(auto &light : lights)
	{
		delete light;
	}
	lights.clear();
}

void LightManager::updateLights()
{
	for(auto &it : lightTypes)
	{
		it.second->updateTransforms();
	}
}

LightManager::~LightManager()
{
	for(auto &it : lightTypes)
	{
		it.second->free();
		delete it.second;
	}
}

template LightType<PointLight>;
template LightType<DirectionalLight>;
template LightType<SpotLight>;