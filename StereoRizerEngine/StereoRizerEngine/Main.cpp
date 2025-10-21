#include "Window.h"
#include "Shader.h"

#include <iostream>

int main()
{
    Window window(800, 600, "StereoRizer Engine");

    Model model(Mesh("../models/Suzanne.obj"), Shader("Phong.shader"));

	model.Translate(glm::vec3(0.0f, 0.0f, -3.0f));
	model.Rotate(45.0f, glm::vec3(0.0, 1.0f, 0.0f));

    window.Run(model);

    return 0;
}

