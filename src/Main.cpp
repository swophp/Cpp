#include <cstdint>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

struct SVertexPositionColor
{
    glm::vec3 Position;
    glm::vec4 Color;
};

#include <fstream>
#include <sstream>
#include <expected>
#include <iostream>

std::string ReadTextFromFile(const char *filename)
{
    std::ifstream in;
    in.open(filename, std::ifstream::in | std::ifstream::binary);
    std::stringstream sstr;
    sstr << in.rdbuf();
    in.close();
    return sstr.str();
}

auto LinkProgram(
    const uint32_t program,
    std::string& errorLog) -> bool {

    int32_t linkStatus = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE)
    {
        int32_t length = 512;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        errorLog.resize(length + 1, '\0');
        glGetProgramInfoLog(program, length, nullptr, errorLog.data());
        return false;
    }

    return true;
}

auto CreateProgram(
    const uint32_t shaderType,
    const std::string_view shaderSource) -> std::expected<uint32_t, std::string> {

    const auto shaderSourcePtr = shaderSource.data();
    auto program = glCreateShaderProgramv(shaderType, 1, &shaderSourcePtr);
   
    glProgramParameteri(program, GL_PROGRAM_SEPARABLE, GL_TRUE);

    std::string errorLog;
    if (!LinkProgram(program, errorLog)) {
        return std::unexpected(errorLog);
    }
    return program;
}

auto OnKey(GLFWwindow* window, int key, int scancode, int action, int mods) -> void
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

auto main() -> int32_t
{
    if (glfwInit() == GLFW_FALSE)
    {
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    auto window = glfwCreateWindow(1280, 720, "Hallo Anton", nullptr, nullptr);
    if (window == nullptr)
    {
        glfwTerminate();
        return -2;
    }

    glfwSetKeyCallback(window, OnKey);

    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress))
    {
        return -3;
    }

    uint32_t defaultInputLayout = 0;
    glCreateVertexArrays(1, &defaultInputLayout);

    glVertexArrayAttribFormat(defaultInputLayout, 0, 3, GL_FLOAT, false, offsetof(SVertexPositionColor, Position));
    glVertexArrayAttribBinding(defaultInputLayout, 0, 0);
    glEnableVertexArrayAttrib(defaultInputLayout, 0);

    glVertexArrayAttribFormat(defaultInputLayout, 1, 4, GL_FLOAT, false, offsetof(SVertexPositionColor, Color));
    glVertexArrayAttribBinding(defaultInputLayout, 1, 0);
    glEnableVertexArrayAttrib(defaultInputLayout, 1);

    std::vector<SVertexPositionColor> vertices = 
    {
        {{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{-0.5, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
    };

    std::vector<uint32_t> indices = 
    {
        0, 1, 2
    };

    uint32_t vertexBuffer = 0;
    glCreateBuffers(1, &vertexBuffer);
    glNamedBufferData(vertexBuffer, sizeof(SVertexPositionColor) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    uint32_t indexBuffer = 0;
    glCreateBuffers(1, &indexBuffer);
    glNamedBufferData(indexBuffer, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

    auto vertexShaderSource = ReadTextFromFile("../data/shaders/Simple.vs.glsl");
    auto fragmentShaderSource = ReadTextFromFile("../data/shaders/Simple.fs.glsl");

    auto vertexShaderResult = CreateProgram(GL_VERTEX_SHADER, vertexShaderSource);
    if (!vertexShaderResult)
    {
        std::cout << vertexShaderResult.error() << "\n";
        return -10;
    }

    auto fragmentShaderResult = CreateProgram(GL_FRAGMENT_SHADER, fragmentShaderSource);
    if (!fragmentShaderResult)
    {
        std::cout << fragmentShaderResult.error() << "\n";
        return -10; 
    }

    uint32_t simpleProgramPipeline = 0;
    glCreateProgramPipelines(1, &simpleProgramPipeline);
    glUseProgramStages(simpleProgramPipeline, GL_VERTEX_SHADER_BIT, *vertexShaderResult);
    glUseProgramStages(simpleProgramPipeline, GL_FRAGMENT_SHADER_BIT, *fragmentShaderResult);

    glEnable(GL_FRAMEBUFFER_SRGB);
    glClearColor(0.01f, 0.02f, 0.03f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(defaultInputLayout);
        glBindProgramPipeline(simpleProgramPipeline);
        glVertexArrayVertexBuffer(defaultInputLayout, 0, vertexBuffer, 0, sizeof(SVertexPositionColor));
        glVertexArrayElementBuffer(defaultInputLayout, indexBuffer);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}