
#include "Renderer.h"

Renderer::Renderer()
	: _camera(45.0f, 4.0f / 3.0f, 0.1f, 100.0f) {}

Renderer::~Renderer() = default;

void Renderer::SetCamera(const Camera& camera) {
	_camera = camera;
}

Camera& Renderer::GetCamera() {
	return _camera;
}

void Renderer::Clear() {
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw(const Model& model) {
	_camera.UploadToShader(model.GetShader());
	model.Draw();
}
