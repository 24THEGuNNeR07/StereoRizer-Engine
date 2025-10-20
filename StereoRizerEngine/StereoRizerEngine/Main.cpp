#include "Window.h"
#include "Shader.h"

#include <iostream>

int main()
{
    Window window(800, 600, "StereoRizer Engine");

	Model model("../models/Suzanne.obj", "Phong.shader");

    window.Run(model);

    return 0;
}

