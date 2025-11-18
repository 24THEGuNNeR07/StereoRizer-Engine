#pragma once

#include <memory>
#include <vector>
#include <GL/glew.h>

#include "Shader.h"
#include "Mesh.h"
#include "Model.h"
#include "Camera.h"
#include "Light.h"

namespace stereorizer::graphics
{
	class Renderer {
	public:
		Renderer();
		~Renderer();

		std::shared_ptr<Camera> GetCamera();

		void Clear();
		void Draw(std::shared_ptr<Model> model);
		
		// Light management
		void SetLight(std::shared_ptr<Light> light);
		std::shared_ptr<Light> GetLight() const { return _light; }

		// Depth texture support
		void SetupDepthTexture(int width, int height, bool isRightViewport = false);
		void BeginDepthTextureRender();
		void EndDepthTextureRender();
		void RenderDepthTexture(const std::vector<std::shared_ptr<Model>>& models);
		void RenderDepthVisualization(float nearPlane = 0.1f, float farPlane = 100.0f);
		void RenderReprojection(const std::vector<std::shared_ptr<Model>>& models, std::unique_ptr<Renderer>& leftRenderer);
		GLuint GetDepthTexture() const { return _depthTexture; }
		GLuint GetColorTexture() const { return _colorTexture; }
		bool IsDepthTextureEnabled() const { return _depthTexture != 0; }

	private:
		std::shared_ptr<Camera> _camera;
		std::shared_ptr<Light> _light;
		
		// OpenGL state management
		struct OpenGLState {
			GLint framebuffer;
			GLint texture2D;
			GLint activeTexture;
		};
		
		// Depth texture rendering
		GLuint _framebuffer = 0;
		GLuint _colorTexture = 0;
		GLuint _depthTexture = 0;
		int _textureWidth = 0;
		int _textureHeight = 0;
		bool _isRightViewport = false;

		bool texturesReadyForReprojection = false;
		
		// Full-screen quad for depth visualization
		GLuint _quadVAO = 0;
		GLuint _quadVBO = 0;
		std::shared_ptr<Shader> _depthShader = nullptr;
		std::shared_ptr<Shader> _reprojectionShader = nullptr;
		
		void SetupFullScreenQuad();
		void CleanupFullScreenQuad();
		
		// Texture and framebuffer creation methods
		void CreateColorTexture();
		void CreateDepthTexture();
		void CreateFramebuffer();
		
		// OpenGL state management
		OpenGLState SaveOpenGLState();
		void RestoreOpenGLState(const OpenGLState& state);
	};
}