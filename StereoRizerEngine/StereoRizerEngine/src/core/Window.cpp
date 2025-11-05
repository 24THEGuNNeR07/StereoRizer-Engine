#include "core/Window.h"
#include "graphics/Shader.h"
#include "graphics/Model.h"
#include "core/Common.h"
#include "graphics/Renderer.h"
#include "xr/OpenXRSupport.h"
#include <glm/glm.hpp>
#include <cmath>

using namespace stereorizer::core;
using namespace stereorizer::graphics;

Window::Window(int width, int height, const char* title)
{
	_width = width;
	_height = height;
	_title = title;

	// initialize unique_ptr with nullptr and custom deleter
	_window = std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>>(nullptr, [](GLFWwindow* w){ if (w) glfwDestroyWindow(w); });

	InitResources();
	_leftRenderer = std::make_unique<Renderer>();
	_rightRenderer = std::make_unique<Renderer>();

	// Setup depth texture for left renderer (always enabled)
	int textureWidth = _width / 2;
	int textureHeight = _height;
	_leftRenderer->SetupDepthTexture(textureWidth, textureHeight);

	// Position the stereo camera pair using IPD (left/right offset around origin)
	// Calculate middle look-at point and set both cameras to look at it
	{
		auto leftCam = _leftRenderer->GetCamera();
		auto rightCam = _rightRenderer->GetCamera();
		if (leftCam && rightCam) {
			float half = _ipd / 2.0f;
			glm::vec3 center = leftCam->GetPosition();
			
			glm::vec3 lookAtDistance = leftCam->GetFront() * 10.0f;
			glm::vec3 middleLookAt = center + lookAtDistance;
			
			glm::vec3 leftPos = center + glm::vec3(-half, 0.0f, 0.0f);
			glm::vec3 rightPos = center + glm::vec3(half, 0.0f, 0.0f);
			
			leftCam->SetPosition(leftPos);
			rightCam->SetPosition(rightPos);
			
			glm::vec3 leftDir = glm::normalize(middleLookAt - leftPos);
			glm::vec3 rightDir = glm::normalize(middleLookAt - rightPos);
			
			float leftYaw = glm::degrees(atan2(leftDir.z, leftDir.x));
			float leftPitch = glm::degrees(asin(leftDir.y));
			float rightYaw = glm::degrees(atan2(rightDir.z, rightDir.x));
			float rightPitch = glm::degrees(asin(rightDir.y));
			
			leftCam->SetYaw(leftYaw);
			leftCam->SetPitch(leftPitch);
			rightCam->SetYaw(rightYaw);
			rightCam->SetPitch(rightPitch);
		}
	}
}

Window::~Window()
{
	Destroy();
}

void Window::Destroy()
{
	if (_window)
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		_window.reset();
		glfwTerminate();
	}
}

void Window::PollEvents()
{
	glfwPollEvents();
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(_window.get());
}

void Window::AddModel(std::shared_ptr<Model> model)
{
	if (!model) return;
	if (std::find(_models.begin(), _models.end(), model) == _models.end())
		_models.push_back(model);
}

void Window::RemoveModel(std::shared_ptr<Model> model)
{
	if (!model) return;
	auto it = std::find(_models.begin(), _models.end(), model);
	if (it != _models.end())
		_models.erase(it);
}

bool Window::UpdateXRViews()
{
	if (!_xrInitialized)
		return false;

	_xrSupport.PollEvents();
	if (_xrInitialized)
		_xrInitialized = _xrSupport.WaitFrame();
	else
		return false;

	if (_xrInitialized)
		_xrInitialized = _xrSupport.BeginFrame();
	else
		return false;

	if (_xrInitialized)
		_xrInitialized = _xrSupport.LocateViews();
	else
		return false;

	glm::mat4 leftView = _xrSupport.ConvertXrPoseToMat4(0);
	glm::mat4 leftProj = _xrSupport.ConvertXrFovToProj(0, 0.1f, 100.0f);
	_leftRenderer->GetCamera()->SetViewMatrix(leftView);
	_leftRenderer->GetCamera()->SetProjectionMatrix(leftProj);

	glm::mat4 rightView = _xrSupport.ConvertXrPoseToMat4(1);
	glm::mat4 rightProj = _xrSupport.ConvertXrFovToProj(1, 0.1f, 100.0f);
	_rightRenderer->GetCamera()->SetViewMatrix(rightView);
	_rightRenderer->GetCamera()->SetProjectionMatrix(rightProj);

	return true;
}

void Window::RenderModelsLeft()
{
	if (!_leftRenderer) return;
	
	// Always render to depth texture framebuffer first
	if (_leftRenderer->IsDepthTextureEnabled()) {
		_leftRenderer->BeginDepthTextureRender();
		for (auto& m : _models) {
			if (m)
				_leftRenderer->Draw(m);
		}
		_leftRenderer->EndDepthTextureRender();
	}
	
	// Render to the current viewport (left half of screen)
	// Display either normal color rendering or depth visualization based on mode
	if (_leftViewDisplayMode == LeftViewDisplayMode::Depth && _leftRenderer->IsDepthTextureEnabled()) {
		RenderDepthVisualization();
	} else {
		// Normal color rendering
		for (auto& m : _models) {
			if (m)
				_leftRenderer->Draw(m);
		}
	}
}

void Window::RenderModelsRight()
{
	if (!_rightRenderer) return;
	for (auto& m : _models)
	{
		if (m)
			_rightRenderer->Draw(m);
	}
}

void Window::Run()
{
	if (_xrInitialized)
		_xrSupport.InitCopyFrameBuffer(_width, _height);

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(_window.get()))
	{
		currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		PollEvents();

		glfwMakeContextCurrent(_window.get());

		int newWidth, newHeight;
		glfwGetFramebufferSize(_window.get(), &newWidth, &newHeight);
		
		// Update depth texture if window size changed
		if (newWidth != _width || newHeight != _height) {
			_width = newWidth;
			_height = newHeight;
			if (_leftRenderer) {
				int textureWidth = _width / 2; // Left view takes half the screen width
				int textureHeight = _height;
				_leftRenderer->SetupDepthTexture(textureWidth, textureHeight);
			}
		}

		if (_xrInitialized)
			_xrSupport.SetFrameSize(_width, _height);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, _width, _height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (_xrInitialized)
			_xrInitialized = UpdateXRViews();
		else {
			processInput(_window.get());
			handleMouseInput();
		}

		glViewport(0, 0, _width / 2, _height);
		RenderModelsLeft();

		glViewport(_width / 2, 0, _width / 2, _height);
		RenderModelsRight();

		glFlush();

		if (!_xrInitialized)
		{
			glViewport(0, 0, _width, _height);
			RenderImGui();
		}

		if (_xrInitialized)
			_xrInitialized = _xrSupport.CopyFrameBuffer();
        
		SwapBuffers();
	}

	if (_xrInitialized)
		_xrSupport.EndLoop();
}

int Window::GetWidth() const
{
	return _width;
}

int Window::GetHeight() const
{
	return _height;
}

void Window::InitResources()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_window = std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)>(nullptr, [](GLFWwindow* w){ if (w) glfwDestroyWindow(w); });
	GLFWwindow* raw = glfwCreateWindow(_width, _height, _title, NULL, NULL);
	_window.reset(raw);
	glfwMakeContextCurrent(_window.get());
	glewInit();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	glfwSetWindowUserPointer(_window.get(), this);
	
	ImGui_ImplGlfw_InitForOpenGL(_window.get(), true);
	ImGui_ImplOpenGL3_Init("#version 450");

	unsigned char* tex_pixels = nullptr;
	int tex_w, tex_h;
	io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_w, &tex_h);

	LOG_INFO(std::string("GL Renderer: ") + reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

	_xrInitialized = _xrSupport.Init(m_apiType);
	if (_xrInitialized)
	{
		auto [recommendedWidth, recommendedHeight] = _xrSupport.GetRecommendedTargetSize();
		_width = static_cast<int>(recommendedWidth) * 2;
		_height = static_cast<int>(recommendedHeight);
		glfwSetWindowSize(_window.get(), _width, _height);
	}
}

void stereorizer::core::Window::processInput(GLFWwindow* window)
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureKeyboard) {
		return;
	}

	std::vector<stereorizer::graphics::Renderer*> renderers = {
		_leftRenderer.get(),
		_rightRenderer.get()
	};

	auto moveCameras = [&](const glm::vec3& offset) {
		for (auto* renderer : renderers) {
			auto camera = renderer->GetCamera();
			camera->SetPosition(camera->GetPosition() + offset);
		}
		};

	const float cameraSpeed = 2.5f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		moveCameras(cameraSpeed * _leftRenderer->GetCamera()->GetFront());
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		moveCameras(-cameraSpeed * _leftRenderer->GetCamera()->GetFront());

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		moveCameras(-cameraSpeed * _leftRenderer->GetCamera()->GetCameraRight());
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		moveCameras(cameraSpeed * _leftRenderer->GetCamera()->GetCameraRight());

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		moveCameras(glm::vec3(0.0f, cameraSpeed, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		moveCameras(glm::vec3(0.0f, -cameraSpeed, 0.0f));
}

void stereorizer::core::Window::OnMouseMove(double xpos, double ypos)
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {
		return;
	}

	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos;
	lastX = (float)xpos;
	lastY = (float)ypos;

	if (glfwGetMouseButton(_window.get(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		const float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		std::vector<stereorizer::graphics::Camera*> cameras = {
			_leftRenderer->GetCamera().get(),
			_rightRenderer->GetCamera().get()
		};

		for (auto* cam : cameras) {
			cam->SetYaw(cam->GetYaw() + xoffset);
			cam->SetPitch(cam->GetPitch() + yoffset);
		}
	}
}

void stereorizer::core::Window::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	Window* app = static_cast<Window*>(glfwGetWindowUserPointer(window));
	app->OnMouseMove(xpos, ypos);
}

float Window::GetIPD() const {
	return _ipd;
}

void Window::SetIPD(float ipd) {
	_ipd = ipd;
	// Reposition cameras around the current center position and recalculate orientations
	if (_leftRenderer && _rightRenderer) {
		auto leftCam = _leftRenderer->GetCamera();
		auto rightCam = _rightRenderer->GetCamera();
		if (leftCam && rightCam) {
			glm::vec3 center = (leftCam->GetPosition() + rightCam->GetPosition()) * 0.5f;
			glm::vec3 averageDir = glm::normalize((leftCam->GetFront() + rightCam->GetFront()) * 0.5f);
			glm::vec3 middleLookAt = center + averageDir * 10.0f;
			
			float half = _ipd / 2.0f;
			glm::vec3 leftPos = center + glm::vec3(-half, 0.0f, 0.0f);
			glm::vec3 rightPos = center + glm::vec3(half, 0.0f, 0.0f);
			
			leftCam->SetPosition(leftPos);
			rightCam->SetPosition(rightPos);
			
			glm::vec3 leftDir = glm::normalize(middleLookAt - leftPos);
			glm::vec3 rightDir = glm::normalize(middleLookAt - rightPos);
			
			float leftYaw = glm::degrees(atan2(leftDir.z, leftDir.x));
			float leftPitch = glm::degrees(asin(leftDir.y));
			float rightYaw = glm::degrees(atan2(rightDir.z, rightDir.x));
			float rightPitch = glm::degrees(asin(rightDir.y));
			
			leftCam->SetYaw(leftYaw);
			leftCam->SetPitch(leftPitch);
			rightCam->SetYaw(rightYaw);
			rightCam->SetPitch(rightPitch);
		}
	}
}

void stereorizer::core::Window::RenderImGui() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(350, 200), ImGuiCond_FirstUseEver);
	
	ImGui::Begin("Stereo Settings");
	
	float currentIPD = GetIPD();
	if (ImGui::SliderFloat("IPD (meters)", &currentIPD, 0.05f, 0.2f, "%.3f")) {
		SetIPD(currentIPD);
	}
	
	ImGui::Text("IPD: %.1f mm", currentIPD * 1000.0f);
	
	ImGui::Separator();
	
	// Left view display mode selection
	ImGui::Text("Left View Display Mode:");
	bool showColor = (GetLeftViewDisplayMode() == LeftViewDisplayMode::Color);
	bool showDepth = (GetLeftViewDisplayMode() == LeftViewDisplayMode::Depth);
	
	if (ImGui::RadioButton("Color", showColor)) {
		SetLeftViewDisplayMode(LeftViewDisplayMode::Color);
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Depth", showDepth)) {
		SetLeftViewDisplayMode(LeftViewDisplayMode::Depth);
	}
	
	// Depth texture info
	if (_leftRenderer && _leftRenderer->IsDepthTextureEnabled()) {
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "Depth texture active");
		ImGui::Text("Depth Texture ID: %u", GetLeftViewDepthTexture());
		ImGui::Text("Color Texture ID: %u", GetLeftViewColorTexture());
	} else {
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Depth texture inactive");
	}
	
	ImGui::Separator();
	ImGui::Text("Inter-Pupillary Distance");
	ImGui::TextWrapped("Adjust the distance between the left and right eye cameras for comfortable stereo viewing.");
	
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void stereorizer::core::Window::handleMouseInput() {
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {
		return;
	}

	double xpos, ypos;
	glfwGetCursorPos(_window.get(), &xpos, &ypos);

	if (firstMouse) {
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos;
	lastX = (float)xpos;
	lastY = (float)ypos;

	if (glfwGetMouseButton(_window.get(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		const float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		std::vector<stereorizer::graphics::Camera*> cameras = {
			_leftRenderer->GetCamera().get(),
			_rightRenderer->GetCamera().get()
		};

		for (auto* cam : cameras) {
			cam->SetYaw(cam->GetYaw() + xoffset);
			cam->SetPitch(cam->GetPitch() + yoffset);
		}
	}
}

void Window::SetLeftViewDisplayMode(LeftViewDisplayMode mode) {
	_leftViewDisplayMode = mode;
}

LeftViewDisplayMode Window::GetLeftViewDisplayMode() const {
	return _leftViewDisplayMode;
}

GLuint Window::GetLeftViewDepthTexture() const {
	if (_leftRenderer && _leftRenderer->IsDepthTextureEnabled()) {
		return _leftRenderer->GetDepthTexture();
	}
	return 0;
}

GLuint Window::GetLeftViewColorTexture() const {
	if (_leftRenderer && _leftRenderer->IsDepthTextureEnabled()) {
		return _leftRenderer->GetColorTexture();
	}
	return 0;
}

void Window::RenderDepthVisualization() {
	if (_leftRenderer && _leftRenderer->IsDepthTextureEnabled()) {
		// Get camera near and far planes for proper depth linearization
		auto camera = _leftRenderer->GetCamera();
		float nearPlane = camera ? camera->GetNearPlane() : 0.1f;
		float farPlane = camera ? camera->GetFarPlane() : 100.0f;
		
		// Render the depth texture as a full-screen visualization
		_leftRenderer->RenderDepthVisualization(nearPlane, farPlane);
	} else {
		// Fallback to normal rendering if depth texture is not available
		for (auto& m : _models) {
			if (m)
				_leftRenderer->Draw(m);
		}
	}
}