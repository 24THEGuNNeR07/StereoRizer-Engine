#include "Window.h"
#include "Shader.h"

#include <iostream>

int main()
{
    Window window(800, 600, "StereoRizer Engine");

    Mesh mesh("../models/Suzanne.obj");
    Shader shader("Phong.shader");
    Model model(std::move(mesh), std::move(shader));
	model.Translate(glm::vec3(0.0f, 0.0f, -10.0f));

    window.UploadModel(model);
    window.Run();

    return 0;
}

