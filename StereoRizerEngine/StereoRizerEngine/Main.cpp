#include "Window.h"
#include "Shader.h"

#include <iostream>

int main()
{
    Window window(800, 600, "StereoRizer Engine");

	Mesh mesh("../models/Cube.obj");
    Shader shader("Flat.shader");

    window.Run(mesh, shader);

    return 0;
}

