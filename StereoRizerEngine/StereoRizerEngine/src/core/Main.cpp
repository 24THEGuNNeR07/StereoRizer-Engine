#include <core/Window.h>
#include <graphics/AssimpModel.h>

int main()
{
    stereorizer::core::Window window(800, 600, "StereoRizer Engine");

	stereorizer::graphics::Shader* shader = new stereorizer::graphics::Shader("./resources/shaders/Phong.shader");
    stereorizer::graphics::Material* litMat = new stereorizer::graphics::Material(*shader, "testMat");

    stereorizer::graphics::AssimpModel* model = new stereorizer::graphics::AssimpModel("../models/Suzanne.obj", *litMat);

	model->meshes[0]->setPosition({ 0.0f, 0.0f, -5.0f });

    window.AddModel(*model);
    window.Run();

    return 0;
}

