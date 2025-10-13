#include "Renderer.h"

void Renderer::Draw(Model& model)
{
	_camera.SetShaderUnifroms(model.GetShader());
	model.Draw();
}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::SetCamera(glm::mat4 view, glm::mat4 proj)
{
	_camera.SetViewMatrix(view);
	_camera.SetProjectionMatrix(proj);
}
