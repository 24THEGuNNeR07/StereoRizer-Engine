
#include "graphics/Renderer.h"
#include "graphics/Camera.h"
#include "graphics/Model.h"

Renderer::Renderer()
	: _camera(std::make_shared<Camera>(45.0f, 4.0f / 3.0f, 0.1f, 100.0f)) {}

Renderer::~Renderer() = default;

std::shared_ptr<Camera> Renderer::GetCamera() {
	if (!_camera)
		_camera = std::make_unique<Camera>(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	return _camera;
}

void Renderer::Clear() {
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw(std::shared_ptr<Model> model) {
	if (_camera)
		_camera->UploadToShader(model->GetShader());
	model->Draw();
}