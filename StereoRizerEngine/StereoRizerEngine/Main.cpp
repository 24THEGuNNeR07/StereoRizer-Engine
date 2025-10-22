#include "Window.h"
#include "Shader.h"

#include <iostream>
#include <memory>

int main()
{
    Window window(800, 600, "StereoRizer Engine");

	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>("../models/Suzanne.obj");
	std::shared_ptr<Shader> shader = std::make_shared<Shader>("Phong.shader");

    auto model = std::make_shared<Model>(mesh, shader);

    model->Translate(glm::vec3(0.0f, 0.0f, -3.0f));
    model->Rotate(45.0f, glm::vec3(0.0, 1.0f, 0.0));

    window.AddModel(model);
    window.Run();

    return 0;
}

