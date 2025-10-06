#include "Renderer.h"

void Renderer::Draw(Model& model)
{
	model.Draw();
}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}
