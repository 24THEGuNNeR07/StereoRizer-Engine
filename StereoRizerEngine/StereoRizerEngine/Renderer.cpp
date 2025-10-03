#include "Renderer.h"

void Renderer::Draw(Mesh& mesh, Shader& shader)
{
	shader.Bind();
	mesh.Draw();
}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}
