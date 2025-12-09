#include "core/Window.h"
#include "graphics/Shader.h"
#include "graphics/Model.h"
#include "graphics/Mesh.h"
#include "graphics/Light.h"

#include <iostream>
#include <memory>
#include <glm/glm.hpp>

int main()
{
    stereorizer::core::Window window(600, 400, "StereoRizer Engine");

	std::shared_ptr<stereorizer::graphics::Mesh> mesh = std::make_shared<stereorizer::graphics::Mesh>("../models/Suzanne.obj");
	std::shared_ptr<stereorizer::graphics::Shader> shader = std::make_shared<stereorizer::graphics::Shader>("resources/shaders/PhongDiffuseOnly.shader");

    auto model = std::make_shared<stereorizer::graphics::Model>(mesh, shader);

    model->Translate(glm::vec3(0.0f, 0.0f, -3.0f));
    model->Rotate(45.0f, glm::vec3(0.0, 1.0f, 0.0));
	model->SetColor(glm::vec3(0.8f, 0.5f, 0.3f));

    // Create a directional light source (like the sun)
    auto light = std::make_shared<stereorizer::graphics::Light>(stereorizer::graphics::LightType::Directional);
    light->SetDirection(glm::vec3(-0.5f, -1.0f, -0.8f));
    light->SetColor(glm::vec3(1.0f, 0.95f, 0.8f));
    light->SetIntensity(1.2f);
    
    window.SetLight(light);
    
    window.AddModel(model);
    window.Run();

    return 0;
}

