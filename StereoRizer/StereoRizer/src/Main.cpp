#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include <chrono>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Renderer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"

namespace fs = std::filesystem;

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath) 
{
	std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (std::getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
        else {
			ss[(int)type] << line << "\n";
        }
	}

	return { ss[0].str(), ss[1].str() };
}

fs::file_time_type GetLastWriteTime(const std::string& path) {
    if (fs::exists(path)) 
        return fs::last_write_time(path);
    else
        return fs::file_time_type::min();
}

static unsigned int CompileShader(const std::string& source, unsigned int type) 
{
    unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

    int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE) 
    {
        int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)_malloca(length * sizeof(char));
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;
    }

	return id;
}

static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) 
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
	unsigned int fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()) {
        std::cout << "Error initializing GLFW" << std::endl;
        return -1;
    }
        
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << "GLEW Error: " << glewGetErrorString(err) << std::endl;
        return -1;
    }

    float positions[6] = {
        -0.5f, -0.5f,
        0.0f, 0.5f,
        0.5f, -0.5f
    };

	unsigned int indices[3] = {
		0, 1, 2
	};

	unsigned int vao;
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));

    VertexArray va;
	VertexBuffer vb(positions, 6 * sizeof(float));

    VertexBufferLayout layout;
	layout.Push<float>(2); // 2 floats per vertex (x, y)
	va.AddBuffer(vb, layout);

	IndexBuffer ib(indices, 3);

    std::string shaderPath = "res/shaders/Basic.shader";
	ShaderProgramSource source = ParseShader(shaderPath);
	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	glUseProgram(shader);

    auto lastWriteTime = GetLastWriteTime(shaderPath);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        auto currentWriteTime = GetLastWriteTime(shaderPath);

        if (currentWriteTime != lastWriteTime) {
            lastWriteTime = currentWriteTime;
            std::cout << "Reloading shader...\n";

            ShaderProgramSource newSource = ParseShader(shaderPath);
            unsigned int newShader = CreateShader(newSource.VertexSource, newSource.FragmentSource);
            if (newShader != 0) { // Check compilation success
                glDeleteProgram(shader);
                shader = newShader;
                glUseProgram(shader);
            }
        }
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(vao);
		ib.Bind();

		glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

	glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}