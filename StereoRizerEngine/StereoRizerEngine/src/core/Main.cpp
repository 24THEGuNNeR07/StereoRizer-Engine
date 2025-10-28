#include "core/Window.h"
#include "graphics/Shader.h"
#include "graphics/AssimpModel.h"
#include "graphics/Mesh.h"

#include <iostream>
#include <memory>

int main()
{
    stereorizer::core::Window window(800, 600, "StereoRizer Engine");

	std::shared_ptr<stereorizer::graphics::Mesh> mesh = std::make_shared<stereorizer::graphics::Mesh>("../models/Suzanne.obj");
	std::shared_ptr<stereorizer::graphics::Shader> shader = std::make_shared<stereorizer::graphics::Shader>("resources/shaders/Phong.shader");

    auto model = std::make_shared<stereorizer::graphics::AssimpModel>(mesh, shader);

    /*model->Translate(glm::vec3(0.0f, 0.0f, -3.0f));
    model->Rotate(45.0f, glm::vec3(0.0, 1.0f, 0.0));*/

    window.AddModel(model);
    window.Run();

    return 0;
}

