#include "graphics/Renderer.h"
#include "graphics/Camera.h"
#include "graphics/Model.h"
#include "core/Common.h"

using namespace stereorizer::graphics;

Renderer::Renderer()
	: _camera(std::make_shared<Camera>(45.0f, 4.0f / 3.0f, 0.1f, 10.0f)) {
	// Framebuffer and textures are created on-demand in BeginDepthTextureRender
	// This avoids OpenGL calls during construction when context might not be ready
}

Renderer::~Renderer() {
	if (_framebuffer != 0) {
		glDeleteFramebuffers(1, &_framebuffer);
	}
	if (_colorTexture != 0) {
		glDeleteTextures(1, &_colorTexture);
	}
	if (_depthTexture != 0) {
		glDeleteTextures(1, &_depthTexture);
	}
	CleanupFullScreenQuad();
}

std::shared_ptr<Camera> Renderer::GetCamera() {
	if (!_camera)
		_camera = std::make_shared<Camera>(45.0f, 4.0f / 3.0f, 0.1f, 10.0f);
	return _camera;
}

void Renderer::Clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Draw(std::shared_ptr<Model> model) {
	if (_camera)
		_camera->UploadToShader(model->GetShader());
	if (_light)
		_light->UploadToShader(model->GetShader()->GetID(), "light");
	model->Draw();
}

void stereorizer::graphics::Renderer::SetLight(std::shared_ptr<Light> light)
{
	_light = light;
}

void Renderer::SetupDepthTexture(int width, int height, bool isRightViewport) {
	// Just store the parameters - don't do ANY OpenGL work here
	// The actual setup will happen on first use in BeginDepthTextureRender
	_textureWidth = width;
	_textureHeight = height;
	_isRightViewport = isRightViewport;
	
	// Clean up existing resources only
	if (_framebuffer != 0) {
		glDeleteFramebuffers(1, &_framebuffer);
		_framebuffer = 0;
	}
	if (_colorTexture != 0) {
		glDeleteTextures(1, &_colorTexture);
		_colorTexture = 0;
	}
	if (_depthTexture != 0) {
		glDeleteTextures(1, &_depthTexture);
		_depthTexture = 0;
	}
	
	LOG_INFO("Depth texture setup deferred - will be created on first use");
	LOG_INFO("Target size: " + std::to_string(width) + "x" + std::to_string(height));
}

void Renderer::CreateColorTexture() {
	glGenTextures(1, &_colorTexture);
	glBindTexture(GL_TEXTURE_2D, _colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _textureWidth, _textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Renderer::CreateDepthTexture() {
	glGenTextures(1, &_depthTexture);
	glBindTexture(GL_TEXTURE_2D, _depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, _textureWidth, _textureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
}

void Renderer::CreateFramebuffer() {
	glGenFramebuffers(1, &_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
	
	// Create and attach textures
	CreateColorTexture();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorTexture, 0);
	
	CreateDepthTexture();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthTexture, 0);
	
	// Ensure we have both color and depth attachments
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	
	// Check framebuffer completeness
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		LOG_ERROR("Framebuffer not complete for depth texture rendering! Status: " + std::to_string(status));
		// Clean up failed resources
		if (_framebuffer != 0) {
			glDeleteFramebuffers(1, &_framebuffer);
			_framebuffer = 0;
		}
		if (_colorTexture != 0) {
			glDeleteTextures(1, &_colorTexture);
			_colorTexture = 0;
		}
		if (_depthTexture != 0) {
			glDeleteTextures(1, &_depthTexture);
			_depthTexture = 0;
		}
		throw std::runtime_error("Failed to create framebuffer");
	} else {
		LOG_INFO("Depth texture framebuffer created successfully");
		LOG_INFO("Color texture: " + std::to_string(_colorTexture) + ", Depth texture: " + std::to_string(_depthTexture));
		LOG_INFO("Framebuffer size: " + std::to_string(_textureWidth) + "x" + std::to_string(_textureHeight));
	}
}

void Renderer::BeginTextureRender() {
	// Check if depth texture setup was called (deferred creation pattern)
	if (_textureWidth == 0 || _textureHeight == 0) return;
	
	// Create framebuffer and textures on first use if not already created
	if (_framebuffer == 0) {
		// Save current OpenGL state
		OpenGLState savedState = SaveOpenGLState();
		
		try {
			CreateFramebuffer();
		} catch (const std::runtime_error& e) {
			// Restore state and return on failure
			RestoreOpenGLState(savedState);
			return;
		}
		
		// Restore state after creation
		RestoreOpenGLState(savedState);
	}
	
	// Now use the framebuffer for rendering
	glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
	// Set viewport based on whether this is right viewport (shifted) or left viewport
	glViewport(0, 0, _textureWidth, _textureHeight);
	
	// Ensure depth testing and depth writes are enabled
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::EndTextureRender() {
	if (_depthTexture == 0) return;
	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		LOG_ERROR("Framebuffer not complete when ending render! Status: " + std::to_string(status));
	}

	glFlush();
	glFinish();
	
	glViewport(_isRightViewport ? _textureWidth : 0, 0, _textureWidth, _textureHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderToTextures(const std::vector<std::shared_ptr<Model>>& models) {

	BeginTextureRender();
	for (const auto& model : models) {
		if (model) {
			Draw(model);
		}
	}
	EndTextureRender();
}

void Renderer::SetupFullScreenQuad() {
	// Save current OpenGL state
	GLint previousVAO, previousVBO, previousArrayBuffer;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousVAO);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previousArrayBuffer);
	
	// Full-screen quad vertices (position + texture coordinates)
	float quadVertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		
		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenVertexArrays(1, &_quadVAO);
	glGenBuffers(1, &_quadVBO);
	glBindVertexArray(_quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, _quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	
	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	
	// Texture coordinate attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	
	// Restore previous OpenGL state
	glBindVertexArray(previousVAO);
	glBindBuffer(GL_ARRAY_BUFFER, previousArrayBuffer);
	
	// Load visualization shaders
	try {
		_depthShader = std::make_shared<Shader>("resources/shaders/DepthVisualization.shader");
	} catch (const std::exception& e) {
		LOG_ERROR(std::string("Failed to load depth visualization shader: ") + e.what());
		_depthShader = nullptr;
	}
	
	try {
		_colorShader = std::make_shared<Shader>("resources/shaders/ColorVisualization.shader");
	} catch (const std::exception& e) {
		LOG_ERROR(std::string("Failed to load color visualization shader: ") + e.what());
		_colorShader = nullptr;
	}
}

void Renderer::CleanupFullScreenQuad() {
	if (_quadVAO != 0) {
		glDeleteVertexArrays(1, &_quadVAO);
		_quadVAO = 0;
	}
	if (_quadVBO != 0) {
		glDeleteBuffers(1, &_quadVBO);
		_quadVBO = 0;
	}
	_depthShader = nullptr;
	_colorShader = nullptr;
}

Renderer::OpenGLState Renderer::SaveOpenGLState() {
	OpenGLState state;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &state.framebuffer);
	glGetIntegerv(GL_ACTIVE_TEXTURE, &state.activeTexture);
	glActiveTexture(GL_TEXTURE0);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &state.texture2D);
	return state;
}

void Renderer::RestoreOpenGLState(const OpenGLState& state) {
	if (state.framebuffer != 0)
		glBindFramebuffer(GL_FRAMEBUFFER, state.framebuffer);
	if (state.texture2D != 0)
		glBindTexture(GL_TEXTURE_2D, state.texture2D);
	if (state.activeTexture != 0)
		glActiveTexture(state.activeTexture);
}

void Renderer::RenderDepthVisualization(float nearPlane, float farPlane) {
	if (_depthTexture == 0) {
		LOG_ERROR("Cannot render depth visualization: depth texture not available");
		return;
	}
	
	// Setup full-screen quad if not already done
	if (_quadVAO == 0) {
		SetupFullScreenQuad();
	}
	
	if (!_depthShader || _quadVAO == 0) {
		LOG_ERROR("Depth visualization resources not available");
		return;
	}
	
	// Save current OpenGL state
	GLint previousVAO, previousTexture, previousActiveTexture, previousProgram, previousArrayBuffer;
	GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousVAO);
	glGetIntegerv(GL_CURRENT_PROGRAM, &previousProgram);
	glGetIntegerv(GL_ACTIVE_TEXTURE, &previousActiveTexture);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previousArrayBuffer);
	glActiveTexture(GL_TEXTURE0);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);
	
	// Disable depth testing for full-screen quad
	glDisable(GL_DEPTH_TEST);
	
	// Bind depth shader
	_depthShader->ReloadIfChanged();
	_depthShader->Bind();
	
	// Set uniforms
	glUniform1f(glGetUniformLocation(_depthShader->GetID(), "nearPlane"), nearPlane);
	glUniform1f(glGetUniformLocation(_depthShader->GetID(), "farPlane"), farPlane);
	
	// Bind depth texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _depthTexture);
	glUniform1i(glGetUniformLocation(_depthShader->GetID(), "depthTexture"), 0);
	
	// Render full-screen quad
	glBindVertexArray(_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	// Restore previous OpenGL state completely
	glUseProgram(previousProgram);
	glBindVertexArray(previousVAO);
	glBindBuffer(GL_ARRAY_BUFFER, previousArrayBuffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, previousTexture);
	glActiveTexture(previousActiveTexture);
	
	// Restore depth testing state
	if (depthTestEnabled) {
		glEnable(GL_DEPTH_TEST);
	}
}

void Renderer::RenderColorVisualization() {
	if (_colorTexture == 0) {
		LOG_ERROR("Cannot render color visualization: color texture not available");
		return;
	}
	
	// Setup full-screen quad if not already done
	if (_quadVAO == 0) {
		SetupFullScreenQuad();
	}
	
	if (!_colorShader || _quadVAO == 0) {
		LOG_ERROR("Color visualization resources not available");
		return;
	}
	
	// Save current OpenGL state
	GLint previousVAO, previousTexture, previousActiveTexture, previousProgram, previousArrayBuffer;
	GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousVAO);
	glGetIntegerv(GL_CURRENT_PROGRAM, &previousProgram);
	glGetIntegerv(GL_ACTIVE_TEXTURE, &previousActiveTexture);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previousArrayBuffer);
	glActiveTexture(GL_TEXTURE0);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);
	
	// Disable depth testing for full-screen quad
	glDisable(GL_DEPTH_TEST);
	
	// Bind color shader
	_colorShader->ReloadIfChanged();
	_colorShader->Bind();
	
	// Bind color texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _colorTexture);
	glUniform1i(glGetUniformLocation(_colorShader->GetID(), "colorTexture"), 0);
	
	// Render full-screen quad
	glBindVertexArray(_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	// Restore previous OpenGL state completely
	glUseProgram(previousProgram);
	glBindVertexArray(previousVAO);
	glBindBuffer(GL_ARRAY_BUFFER, previousArrayBuffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, previousTexture);
	glActiveTexture(previousActiveTexture);
	
	// Restore depth testing state
	if (depthTestEnabled) {
		glEnable(GL_DEPTH_TEST);
	}
}

void Renderer::RenderReprojection(const std::vector<std::shared_ptr<Model>>& models, std::unique_ptr<Renderer>& leftRenderer) {
	if (!leftRenderer->IsDepthTextureEnabled() || leftRenderer->GetDepthTexture() == 0 || leftRenderer->GetColorTexture() == 0) {
		LOG_ERROR("Cannot render reprojection: left renderer textures not available");
		return;
	}

	// Load reprojection shader
	if (_reprojectionShader == nullptr) {
		try {
			_reprojectionShader = std::make_shared<Shader>("resources/shaders/Reprojection.shader");
		}
		catch (const std::exception& e) {
			LOG_ERROR(std::string("Failed to load reprojection shader: ") + e.what());
			return;
		}
	}

	// Bind reprojection shader
	_reprojectionShader->ReloadIfChanged();
	_reprojectionShader->Bind();

	// Upload camera matrices
	auto leftCamera = leftRenderer->GetCamera();


	if (!texturesReadyForReprojection) {
		// Validate texture handles before binding
		GLuint depthTexture = leftRenderer->GetDepthTexture();
		GLuint colorTexture = leftRenderer->GetColorTexture();

		if (depthTexture == 0 || colorTexture == 0) {
			LOG_ERROR("Invalid texture handles - Depth: " + std::to_string(depthTexture) + ", Color: " + std::to_string(colorTexture));
			return;
		}

		// Bind left renderer textures
		// Left depth texture (texture unit 0)
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glUniform1i(glGetUniformLocation(_reprojectionShader->GetID(), "leftDepthTexture"), 0);

		// Left color texture (texture unit 1)
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, colorTexture);
		glUniform1i(glGetUniformLocation(_reprojectionShader->GetID(), "leftColorTexture"), 1);

		// Check if textures are valid OpenGL objects
		if (!glIsTexture(depthTexture) || !glIsTexture(colorTexture)) {
			LOG_ERROR("Invalid OpenGL texture objects - Depth valid: " + std::to_string(glIsTexture(depthTexture)) +
				", Color valid: " + std::to_string(glIsTexture(colorTexture)));
		}
	}

	// Upload right camera matrices (used by vertex shader)
	if (_camera) {
		_camera->UploadToShader(_reprojectionShader);
	}

	if (leftCamera)
		leftCamera->UploadToReprojectionShader(_reprojectionShader);

	glFlush();
	glFinish(); // Ensure all texture bindings are complete

	for (auto& model : models) {
		if (model) {
			// Upload model matrix to the reprojection shader
			GLint modelLoc = glGetUniformLocation(_reprojectionShader->GetID(), "modelMatrix");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model->GetTransformMatrix()));

			// Draw the model's mesh directly (bypassing Model::Draw() to avoid shader binding)
			model->GetMesh()->Draw();
		}
	}
}