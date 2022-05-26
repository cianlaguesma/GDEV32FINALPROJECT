// Quick note: GLAD needs to be included first before GLFW.
// Otherwise, GLAD will complain about gl.h being already included.
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ctime>
#include <cstdlib>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

/**
 * @brief Function for handling the event when the size of the framebuffer changed.
 * @param[in] window Reference to the window
 * @param[in] width New width
 * @param[in] height New height
 */

 /// <summary>
 /// Creates a shader program based on the provided file paths for the vertex and fragment shaders.
 /// </summary>
 /// <param name="vertexShaderFilePath">Vertex shader file path</param>
 /// <param name="fragmentShaderFilePath">Fragment shader file path</param>
 /// <returns>OpenGL handle to the created shader program</returns>
GLuint CreateShaderProgram(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);

/// <summary>
/// Creates a shader based on the provided shader type and the path to the file containing the shader source.
/// </summary>
/// <param name="shaderType">Shader type</param>
/// <param name="shaderFilePath">Path to the file containing the shader source</param>
/// <returns>OpenGL handle to the created shader</returns>
GLuint CreateShaderFromFile(const GLuint& shaderType, const std::string& shaderFilePath);

/// <summary>
/// Creates a shader based on the provided shader type and the string containing the shader source.
/// </summary>
/// <param name="shaderType">Shader type</param>
/// <param name="shaderSource">Shader source string</param>
/// <returns>OpenGL handle to the created shader</returns>
GLuint CreateShaderFromSource(const GLuint& shaderType, const std::string& shaderSource);

/// <summary>
/// Function for handling the event when the size of the framebuffer changed.
/// </summary>
/// <param name="window">Reference to the window</param>
/// <param name="width">New width</param>
/// <param name="height">New height</param>

void FramebufferSizeChangedCallback(GLFWwindow* window, int width, int height)
{
    // Whenever the size of the framebuffer changed (due to window resizing, etc.),
    // update the dimensions of the region to the new size
    glViewport(0, 0, width, height);
}

//camera
glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, 8.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, -1.0f, -2.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraSpeed = 0.005f;

//mouse
float lastX = 400.f, lastY = 300.f;
float pitch = 0.0f;
float yaw = -90.0f;
bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

 

struct Vertex
{
    GLfloat x, y, z;
    GLubyte r, g, b;
    GLfloat u, v;
    GLfloat nx, ny, nz;
};
struct Light
{
    glm::vec3 lightPos;
    glm::vec3 lightColor;
    glm::vec3 diffuseColor;
    glm::vec3 ambientColor;
    glm::vec3 specular;
    glm::vec3 lightDirection;
    glm::vec3 materialAmbient;
    glm::vec3 materialDiffuse;
    glm::vec3 materialSpecular;
};
/**
 * @brief Main function
 * @return An integer indicating whether the program ended successfully or not.
 * A value of 0 indicates the program ended succesfully, while a non-zero value indicates
 * something wrong happened during execution.
 */
int main()
{
    // Initialize GLFW
    int glfwInitStatus = glfwInit();
    if (glfwInitStatus == GLFW_FALSE)
    {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return 1;
    }

    // Tell GLFW that we prefer to use OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Tell GLFW that we prefer to use the modern OpenGL
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //multisample buff
    glfwWindowHint(GLFW_SAMPLES, 4);
    // Tell GLFW to create a window
    int windowWidth = 800;
    int windowHeight = 600;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Final Project - GDEV32", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return 1;
    }

    // Tell GLFW to use the OpenGL context that was assigned to the window that we just created
    glfwMakeContextCurrent(window);

    // Register the callback function that handles when the framebuffer size has changed
    glfwSetFramebufferSizeCallback(window, FramebufferSizeChangedCallback);

    // Tell GLAD to load the OpenGL function pointers
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        return 1;
    }

    // Tell OpenGL the dimensions of the region where stuff will be drawn.
    // For now, tell OpenGL to use the whole screen
    glViewport(0, 0, windowWidth, windowHeight);

    // Tell OpenGL that whenever we clear the screen, we clear it with
    // the specified color, which in this case is blue
    // (red = 0.0, green = 0.0, blue = 1.0)
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    Vertex vertices[250];

  
    // plane reused bottom face
    vertices[0] = { -0.5f, -0.5f, 0.5f,       54,54,54,      0.f,0.f,        0.0f, 1.0f, 0.0f };
    vertices[1] = { 0.5f, -0.5f, 0.5f,        54,54,54,      1.f,0.f,        0.0f, 1.0f, 0.0f };
    vertices[2] = { 0.5f, -0.5f, -0.5f,       54,54,54,      1.f,1.f,        0.0f, 1.0f, 0.0f };

    vertices[3] = { -0.5f, -0.5f, 0.5f,       54,54,54,      0.f,0.f,        0.0f, 1.0f, 0.0f };
    vertices[4] = { -0.5f, -0.5f, -0.5f,      54,54,54,      0.f,1.f,        0.0f, 1.0f, 0.0f };
    vertices[5] = { 0.5f, -0.5f, -0.5f,       54,54,54,      1.f,1.f,        0.0f, 1.0f, 0.0f };

    //BOX

    //front face
    vertices[6] = { -0.5f, 0.5f, 0.5f,		0,0,0,      0.f,1.f,		0.0f, 0.0f, 1.0f };
    vertices[7] = { -0.5f, -0.5f, 0.5f,		0,0,0,      0.f,0.f,		0.0f, 0.0f, 1.0f };
    vertices[8] = { 0.5f, -0.5f, 0.5f,		0,0,0,      1.f,0.f,		0.0f, 0.0f, 1.0f };

    vertices[9] = { -0.5f, 0.5f, 0.5f,		0,0,0,      0.f,1.f,		0.0f, 0.0f, 1.0f };
    vertices[10] = { 0.5f, 0.5f, 0.5f,		0,0,0,      1.f,1.f,		0.0f, 0.0f, 1.0f };
    vertices[11] = { 0.5f, -0.5f, 0.5f,		0,0,0,      1.f,0.f,		0.0f, 0.0f, 1.0f };


    // right face
    vertices[12] = { 0.5f, 0.5f, 0.5f,		0,0,0,      0.f,1.f,		1.0f, 0.0f, 0.0f };
    vertices[13] = { 0.5f, -0.5f, 0.5f,		0,0,0,      0.f,0.f,		1.0f, 0.0f, 0.0f };
    vertices[14] = { 0.5f, -0.5f, -0.5f,	0,0,0,      1.f,0.f,		1.0f, 0.0f, 0.0f };

    vertices[15] = { 0.5f, 0.5f, 0.5f,		0,0,0,      0.f,1.f,		1.0f, 0.0f, 0.0f };
    vertices[16] = { 0.5f, 0.5f, -0.5f,		0,0,0,      1.f,1.f,		1.0f, 0.0f, 0.0f };
    vertices[17] = { 0.5f, -0.5f, -0.5f,	0,0,0,      1.f,0.f,		1.0f, 0.0f, 0.0f };

    // back face
    vertices[18] = { -0.5f, 0.5f, -0.5f,		0,0,0,      0.f,1.f,		0.0f, 0.0f, -1.0f };
    vertices[19] = { -0.5f, -0.5f, -0.5f,		0,0,0,      0.f,0.f,		0.0f, 0.0f, -1.0f };
    vertices[20] = { 0.5f, -0.5f, -0.5f,		0,0,0,      1.f,0.f,		0.0f, 0.0f, -1.0f };

    vertices[21] = { -0.5f, 0.5f, -0.5f,	0,0,0,      0.f,1.f,		0.0f, 0.0f, -1.0f };
    vertices[22] = { 0.5f, 0.5f, -0.5f,		0,0,0,      1.f,1.f,		0.0f, 0.0f, -1.0f };
    vertices[23] = { 0.5f, -0.5f, -0.5f,	0,0,0,      1.f,0.f,		0.0f, 0.0f, -1.0f };

    // left face
    vertices[24] = { -0.5f, 0.5f, 0.5f,		0,0,0,      0.f,1.f,		-1.0f, 0.0f, 0.0f };
    vertices[25] = { -0.5f, -0.5f, 0.5f,	0,0,0,      0.f,0.f,		-1.0f, 0.0f, 0.0f };
    vertices[26] = { -0.5f, -0.5f, -0.5f,	0,0,0,      1.f,0.f,		-1.0f, 0.0f, 0.0f };

    vertices[27] = { -0.5f, 0.5f, 0.5f,		0,0,0,      0.f,1.f,		-1.0f, 0.0f, 0.0f };
    vertices[28] = { -0.5f, 0.5f, -0.5f,	0,0,0,      1.f,1.f,		-1.0f, 0.0f, 0.0f };
    vertices[29] = { -0.5f, -0.5f, -0.5f,	0,0,0,      1.f,0.f,		-1.0f, 0.0f, 0.0f };

    // top face
    vertices[30] = { -0.5f, 0.5f, 0.5f,		0,0,0,      0.f,0.f,		0.0f, 1.0f, 0.0f };
    vertices[31] = { 0.5f, 0.5f, 0.5f,		0,0,0,      1.f,0.f,		0.0f, 1.0f, 0.0f };
    vertices[32] = { 0.5f, 0.5f, -0.5f,		0,0,0,      1.f,1.f,		0.0f, 1.0f, 0.0f };

    vertices[33] = { -0.5f, 0.5f, 0.5f,		0,0,0,      0.f,0.f,		0.0f, 1.0f, 0.0f };
    vertices[34] = { -0.5f, 0.5f, -0.5f,	0,0,0,      0.f,1.f,		0.0f, 1.0f, 0.0f };
    vertices[35] = { 0.5f, 0.5f, -0.5f,		0,0,0,      1.f,1.f,		0.0f, 1.0f, 0.0f };

    // bottom face
    vertices[36] = { -0.5f, -0.5f, 0.5f,	0,0,0,      0.f,0.f,		0.0f, -1.0f, 0.0f };
    vertices[37] = { 0.5f, -0.5f, 0.5f,		0,0,0,      1.f,0.f,		0.0f, -1.0f, 0.0f };
    vertices[38] = { 0.5f, -0.5f, -0.5f,	0,0,0,      1.f,1.f,		0.0f, -1.0f, 0.0f };

    vertices[39] = { -0.5f, -0.5f, 0.5f,	0,0,0,      0.f,0.f,		0.0f, -1.0f, 0.0f };
    vertices[40] = { -0.5f, -0.5f, -0.5f,	0,0,0,      0.f,1.f,		0.0f, -1.0f, 0.0f };
    vertices[41] = { 0.5f, -0.5f, -0.5f,	0,0,0,      1.f,1.f,		0.0f, -1.0f, 0.0f };

    //LAMP
    // TOP OF LAMP
#pragma region PYRAMID
//PYRMID
    vertices[42] = { -0.5f, -0.5f, -0.5f,	255, 255, 255,		0.0f, 0.0f,		0.0f, 0.1f, -0.8f };	//BACK
    vertices[43] = { 0.5f, -0.5f, -0.5f,		255, 255, 255,		1.0f, 0.0f,		0.0f, 0.1f, -0.8f };
    vertices[44] = { 0.0f, 0.5f, 0.0f,		255, 255, 255,		0.5f, 1.0f,		0.0f, 0.1f, -0.8f };

    vertices[45] = { -0.5f, -0.5f, 0.5f,		255, 255, 255,		0.0f, 0.0f,		0.0f, 0.1f, 0.8f };	//FRONT
    vertices[46] = { 0.5f, -0.5f, 0.5f,		255, 255, 255,		1.0f, 0.0f,		0.0f, 0.1f, 0.8f };
    vertices[47] = { 0.0f, 0.5f, 0.0f,		255, 255, 255,		0.5f, 1.0f,		0.0f, 0.1f, 0.8f };

    vertices[48] = { -0.5f, -0.5f, -0.5f,		255, 255, 255,		0.0f, 0.0f,		-0.8f, 0.1f, 0.f };	//LEFT
    vertices[49] = { -0.5f, -0.5f, 0.5f,		255, 255, 255,		1.0f, 0.0f,		-0.8f, 0.1f, 0.f };
    vertices[50] = { 0.0f, 0.5f, 0.0f,		255, 255, 255,		0.5f, 1.0f,		-0.8f, 0.1f, 0.f };

    vertices[51] = { 0.5f, -0.5f, -0.5f,		255, 255, 255,		1.0f, 0.0f,		0.8f, 0.1f, 0.f };	//RIGHT
    vertices[52] = { 0.5f, -0.5f, 0.5f,		255, 255, 255,		0.0f, 0.0f,		0.8f, 0.1f, 0.f };
    vertices[53] = { 0.0f, 0.5f, 0.0f,		255, 255, 255,		0.5f, 1.0f,		0.8f, 0.1f, 0.f };

    vertices[54] = { -0.5f, -0.5f, 0.5f,		255, 125, 255,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f };	//BELOW TOP LEFT
    vertices[55] = { 0.5f, -0.5f, 0.5f,		255, 255, 255,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f };
    vertices[56] = { -0.5f, -0.5f, -0.5f,		255, 255, 255,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f };

    vertices[57] = { -0.5f, -0.5f, -0.5f,		255, 255, 255,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f };	//BELOW BOTTOM RIGHT
    vertices[58] = { 0.5f, -0.5f, 0.5f,		255, 255, 255,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f };
    vertices[59] = { 0.5f, -0.5f, -0.5f,		255, 255, 255,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f };
#pragma endregion

    //Top Reuse the box
    // BED
    // LOWER FRAME
    vertices[60] = { -0.75f, 0.0f, 1.0f,		231,105,105,    0.f,0.f,		0.0f, 1.0f, 0.0f };
    vertices[61] = { -0.75f, 0.0f, 0.f,		    231,105,105,    0.f,1.f,		0.0f, 1.0f, 0.0f };
    vertices[62] = { 0.75f, 0.0f, 0.f,			231,105,105,    1.f,1.f,		0.0f, 1.0f, 0.0f };

    vertices[63] = { -0.75f, 0.0f, 1.0f,		231,105,105,    0.f,0.f,		0.0f, 1.0f, 0.0f };
    vertices[64] = { 0.75f, 0.0f, 0.f,			231,105,105,    1.f,1.f,		0.0f, 1.0f, 0.0f };
    vertices[65] = { 0.75f, 0.0f, 1.0f,			231,105,105,    1.f,0.f,		0.0f, 1.0f, 0.0f };

    // UPPER FRAME
    vertices[66] = { -0.75f, 0.15f, 1.0f,		231,235,105,    0.f,0.f,		0.0f, 1.0f, 0.0f };
    vertices[67] = { -0.75f, 0.15f, 0.f,		231,235,105,    0.f,1.f,		0.0f, 1.0f, 0.0f };
    vertices[68] = { 0.75f, 0.15f, 0.f,			231,235,105,    1.f,1.f,		0.0f, 1.0f, 0.0f };

    vertices[69] = { -0.75f, 0.15f, 1.0f,		231,235,105,    0.f,0.f,		0.0f, 1.0f, 0.0f };
    vertices[70] = { 0.75f, 0.15f, 0.f,			231,235,105,    1.f,1.f,		0.0f, 1.0f, 0.0f };
    vertices[71] = { 0.75f, 0.15f, 1.0f,		231,235,105,    1.f,0.f,		0.0f, 1.0f, 0.0f };
    
    // CONNECTOR LEFT
    vertices[72] = { -0.75f, 0.0f, 1.0f,		231,235,155,    0.f,0.f,		0.0f, 1.0f, 0.0f };
    vertices[73] = { -0.75f, 0.0f, 0.f,		    231,235,155,	1.f,0.f,   	0.0f, 1.0f, 0.0f };
    vertices[74] = { -0.75f, 0.15f, 0.f,		231,235,155,	1.f,1.f,	0.0f, 1.0f, 0.0f };

    vertices[75] = { -0.75f, 0.15f, 1.0f,		231,235,155,	0.f,1.f,	0.0f, 1.0f, 0.0f };
    vertices[76] = { -0.75f, 0.15f, 0.f,		231,235,155,	1.f,1.f,	0.0f, 1.0f, 0.0f };
    vertices[77] = { -0.75f, 0.f, 1.0f,			231,235,155,	0.f,0.f,	0.0f, 1.0f, 0.0f };

    // CONNECTOR RIGHT
    vertices[78] = { 0.75f, 0.0f, 1.0f,		    231,235,155,	0.f,0.f,	0.0f, 1.0f, 0.0f };
    vertices[79] = { 0.75f, 0.0f, 0.f,		    231,235,155,	1.f,0.f,	0.0f, 1.0f, 0.0f };
    vertices[80] = { 0.75f, 0.15f, 0.f,			231,235,155,	1.f,1.f,	0.0f, 1.0f, 0.0f };

    vertices[81] = { 0.75f, 0.15f, 1.0f,		231,235,155,	0.f,1.f,	0.0f, 1.0f, 0.0f };
    vertices[82] = { 0.75f, 0.15f, 0.f,			231,235,155,	1.f,1.f,	0.0f, 1.0f, 0.0f };
    vertices[83] = { 0.75f, 0.f, 1.0f,			231,235,155,	0.f,0.f,	0.0f, 1.0f, 0.0f };


    // CONNECTOR BOTTOM
    vertices[84] = { -0.75f, 0.0f, 0.0f,		231,235,155,	0.f,0.f,	0.0f, 1.0f, 0.0f };
    vertices[85] = { -0.75f, 0.15f, 0.f,		231,235,155,	1.f,0.f,	0.0f, 1.0f, 0.0f };
    vertices[86] = { 0.75f, 0.15f, 0.f,			231,235,155,	1.f,1.f,	0.0f, 1.0f, 0.0f };

    vertices[87] = { 0.75f, 0.f, 0.0f,		    231,235,155,	0.f,1.f,	0.0f, 1.0f, 0.0f };
    vertices[88] = { 0.75f, 0.15f, 0.f,			231,235,155,	1.f,1.f,	0.0f, 1.0f, 0.0f };
    vertices[89] = { -0.75f, 0.f, 0.0f,			231,235,155,	0.f,0.f,	0.0f, 1.0f, 0.0f };

    
    // CONNECTOR TOP
    vertices[90] = { -0.75f, 0.0f, 1.0f,		231,235,155,	0.f,0.f,	0.0f, 1.0f, 0.0f };
    vertices[91] = { -0.75f, 0.15f, 1.f,		231,235,155,	1.f,0.f,	0.0f, 1.0f, 0.0f };
    vertices[92] = { 0.75f, 0.15f, 1.f,			231,235,155,	1.f,1.f,	0.0f, 1.0f, 0.0f };

    vertices[93] = { 0.75f, 0.f, 1.0f,		    231,235,155,	0.f,1.f,	0.0f, 1.0f, 0.0f };
    vertices[94] = { 0.75f, 0.15f, 1.f,			231,235,155,	1.f,1.f,	0.0f, 1.0f, 0.0f };
    vertices[95] = { -0.75f, 0.f, 1.0f,			231,235,155,	0.f,0.f,	0.0f, 1.0f, 0.0f };


    // BED FRAME
     // CONNECTOR LEFT
    vertices[96] = { -0.80f, -0.1f, 1.05f,		231,123,45,     0.f,0.f, 	0.0f, 1.0f, 0.0f };
    vertices[97] = { -0.80f, -0.1f, -0.05f,		231,123,45,     1.f,0.f,		0.0f, 1.0f, 0.0f };
    vertices[98] = { -0.80f, 0.12f, -0.05f,		231,123,45,     1.f,1.f,		0.0f, 1.0f, 0.0f };

    vertices[99] = { -0.80f, 0.12f, 1.05f,		231,123,45,     0.f,1.f,		0.0f, 1.0f, 0.0f };
    vertices[100] = { -0.80f, 0.12f, -0.05f,	231,123,45,     1.f,1.f,		0.0f, 1.0f, 0.0f };
    vertices[101] = { -0.80f, -0.1f, 1.05f,		231,123,45,     0.f,0.f,		0.0f, 1.0f, 0.0f };

    // CONNECTOR RIGHT
    vertices[102] = { 0.80f, -0.1f, 1.05f,		231,123,123,    0.f,0.f,		0.0f, 1.0f, 0.0f };
    vertices[103] = { 0.80f, -0.1f, -0.05f,		231,123,123,    1.f,0.f,		0.0f, 1.0f, 0.0f };
    vertices[104] = { 0.80f, 0.12f, -0.05f,		231,123,123,    1.f,1.f,		0.0f, 1.0f, 0.0f };

    vertices[105] = { 0.80f, 0.12f, 1.05f,		231,123,123,    0.f,1.f,		0.0f, 1.0f, 0.0f };
    vertices[106] = { 0.80f, 0.12f, -0.05f,		231,123,123,	1.f,1.f,	0.0f, 1.0f, 0.0f };
    vertices[107] = { 0.80f, -0.1f, 1.05f,		231,123,123,	0.f,0.f,	0.0f, 1.0f, 0.0f };

    // CONNECTOR BOTTOM
    vertices[108] = { -0.80f, -0.1f, -0.05f,	231,235,155,	0.f,0.f,	0.0f, 1.0f, 0.0f };
    vertices[109] = { -0.80f, 0.12f, -0.05f,	231,235,155,	1.f,0.f,	0.0f, 1.0f, 0.0f };
    vertices[110] = { 0.80f, 0.12f, -0.05f,		231,235,155,	1.f,1.f,	0.0f, 1.0f, 0.0f };

    vertices[111] = { 0.80f, -0.1f, -0.05f,		231,235,155,	0.f,1.f,	0.0f, 1.0f, 0.0f };
    vertices[112] = { 0.80f, 0.12f, -0.05f,		231,235,155,	1.f,1.f,	0.0f, 1.0f, 0.0f };
    vertices[113] = { -0.80f, -0.1f, -0.05f,	231,235,155,	0.f,0.f,	0.0f, 1.0f, 0.0f };


    // CONNECTOR TOP
    vertices[114] = { -0.80f, -0.1f, 1.05f,		231,235,155,	0.f,0.f,	0.0f, 1.0f, 0.0f };
    vertices[115] = { -0.80f, 0.12f, 1.05f,		231,235,155,	1.f,0.f,	0.0f, 1.0f, 0.0f };
    vertices[116] = { 0.80f, 0.12f, 1.05f,		231,235,155,	1.f,1.f,	0.0f, 1.0f, 0.0f };

    vertices[117] = { 0.80f, -0.1f, 1.05f,		231,235,155,	0.f,1.f,	0.0f, 1.0f, 0.0f };
    vertices[118] = { 0.80f, 0.12f, 1.05f,		231,235,155,	1.f,1.f,	0.0f, 1.0f, 0.0f };
    vertices[119] = { -0.80f, -0.1f, 1.05f,		231,235,155,	0.f,0.f,	0.0f, 1.0f, 0.0f };

    // BOTTOM FRAME FACE

    vertices[120] = { -0.80f, -0.1f, 1.05f,		231,105,105,	0.f,0.f,	0.0f, 1.0f, 0.0f };
    vertices[121] = { -0.80f, -0.1f, -0.05f,	231,105,105,	1.0f,0.f,	0.0f, 1.0f, 0.0f };
    vertices[122] = { 0.80f, -0.1f, -0.05f,		231,105,105,	1.f,1.f,	0.0f, 1.0f, 0.0f };

    vertices[123] = { -0.80f, -0.1f, 1.05f,		231,105,105,	0.f,1.f,	0.0f, 1.0f, 0.0f };
    vertices[124] = { 0.80f, -0.1f, -0.05f,		231,105,105,	1.f,1.f,	0.0f, 1.0f, 0.0f };
    vertices[125] = { 0.80f, -0.1f, 1.05f,		231,105,105,	0.f,0.f,	0.0f, 1.0f, 0.0f };

    // Back Frame

    vertices[126] = { -0.80f, 0.12f, 1.05f,		231,123,45,     0.f,0.f,		0.0f, 1.0f, 0.0f };
    vertices[127] = { -0.80f, 0.12f, -0.05f,	231,123,45,     1.0f,0.f,		0.0f, 1.0f, 0.0f };
    vertices[128] = { -0.80f, 0.5f, -0.05f,		231,123,45,     1.f,1.f,		0.0f, 1.0f, 0.0f };

    vertices[129] = { -0.80f, 0.12f, 1.05f,		231,123,45,     0.f,1.f,		0.0f, 1.0f, 0.0f };
    vertices[130] = { -0.80f, 0.12f, -0.05f,	231,123,45,     1.f,1.f,		0.0f, 1.0f, 0.0f };
    vertices[131] = { -0.80f, 0.5f, 1.05f,		231,123,45,     0.f,0.f,		0.0f, 1.0f, 0.0f };


    //PYRMID
    vertices[132] = { -0.5f, -0.5f, -0.5f,	255, 255, 255,		0.0f, 0.0f,		0.0f, 0.1f, -0.8f };	//BACK
    vertices[133] = { 0.5f, -0.5f, -0.5f,		255, 255, 255,		1.0f, 0.0f,		0.0f, 0.1f, -0.8f };
    vertices[134] = { 0.0f, 0.5f, 0.0f,		255, 255, 255,		0.5f, 1.0f,		0.0f, 0.1f, -0.8f };

    vertices[135] = { -0.5f, -0.5f, 0.5f,		255, 255, 255,		0.0f, 0.0f,		0.0f, 0.1f, 0.8f };	//FRONT
    vertices[136] = { 0.5f, -0.5f, 0.5f,		255, 255, 255,		1.0f, 0.0f,		0.0f, 0.1f, 0.8f };
    vertices[137] = { 0.0f, 0.5f, 0.0f,		255, 255, 255,		0.5f, 1.0f,		0.0f, 0.1f, 0.8f };

    vertices[138] = { -0.5f, -0.5f, -0.5f,		255, 255, 255,		0.0f, 0.0f,		-0.8f, 0.1f, 0.f };	//LEFT
    vertices[139] = { -0.5f, -0.5f, 0.5f,		255, 255, 255,		1.0f, 0.0f,		-0.8f, 0.1f, 0.f };
    vertices[140] = { 0.0f, 0.5f, 0.0f,		255, 255, 255,		0.5f, 1.0f,		-0.8f, 0.1f, 0.f };

    vertices[141] = { 0.5f, -0.5f, -0.5f,		255, 255, 255,		1.0f, 0.0f,		0.8f, 0.1f, 0.f };	//RIGHT
    vertices[142] = { 0.5f, -0.5f, 0.5f,		255, 255, 255,		0.0f, 0.0f,		0.8f, 0.1f, 0.f };
    vertices[143] = { 0.0f, 0.5f, 0.0f,		255, 255, 255,		0.5f, 1.0f,		0.8f, 0.1f, 0.f };

    vertices[144] = { -0.5f, -0.5f, 0.5f,		255, 125, 255,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f };	//BELOW TOP LEFT
    vertices[145] = { 0.5f, -0.5f, 0.5f,		255, 255, 255,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f };
    vertices[146] = { -0.5f, -0.5f, -0.5f,		255, 255, 255,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f };

    vertices[147] = { -0.5f, -0.5f, -0.5f,		255, 255, 255,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f };	//BELOW BOTTOM RIGHT
    vertices[148] = { 0.5f, -0.5f, 0.5f,		255, 255, 255,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f };
    vertices[149] = { 0.5f, -0.5f, -0.5f,		255, 255, 255,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f };


    // face movement
     //front face
    vertices[150] = { -0.5f, 0.5f, 0.5f,		0,0,0,      0.f,1.f,		0.0f, 0.0f, 1.0f };
    vertices[151] = { -0.5f, -0.5f, 0.5f,		0,0,0,      0.f,0.f,		0.0f, 0.0f, 1.0f };
    vertices[152] = { 0.5f, -0.5f, 0.5f,		0,0,0,      1.f,0.f,		0.0f, 0.0f, 1.0f };

    vertices[153] = { -0.5f, 0.5f, 0.5f,		0,0,0,      0.f,1.f,		0.0f, 0.0f, 1.0f };
    vertices[154] = { 0.5f, 0.5f, 0.5f,		0,0,0,      1.f,1.f,		0.0f, 0.0f, 1.0f };
    vertices[155] = { 0.5f, -0.5f, 0.5f,		0,0,0,      1.f,0.f,		0.0f, 0.0f, 1.0f };


    // right face
    vertices[156] = { 0.5f, 0.5f, 0.5f,		0,0,0,      0.f,1.f,		1.0f, 0.0f, 0.0f };
    vertices[157] = { 0.5f, -0.5f, 0.5f,		0,0,0,      0.f,0.f,		1.0f, 0.0f, 0.0f };
    vertices[158] = { 0.5f, -0.5f, -0.5f,	0,0,0,      1.f,0.f,		1.0f, 0.0f, 0.0f };

    vertices[159] = { 0.5f, 0.5f, 0.5f,		0,0,0,      0.f,1.f,		1.0f, 0.0f, 0.0f };
    vertices[160] = { 0.5f, 0.5f, -0.5f,		0,0,0,      1.f,1.f,		1.0f, 0.0f, 0.0f };
    vertices[161] = { 0.5f, -0.5f, -0.5f,	0,0,0,      1.f,0.f,		1.0f, 0.0f, 0.0f };

    // back face
    vertices[162] = { -0.5f, 0.5f, -0.5f,		0,0,0,      0.f,1.f,		0.0f, 0.0f, -1.0f };
    vertices[163] = { -0.5f, -0.5f, -0.5f,		0,0,0,      0.f,0.f,		0.0f, 0.0f, -1.0f };
    vertices[164] = { 0.5f, -0.5f, -0.5f,		0,0,0,      1.f,0.f,		0.0f, 0.0f, -1.0f };

    vertices[165] = { -0.5f, 0.5f, -0.5f,	0,0,0,      0.f,1.f,		0.0f, 0.0f, -1.0f };
    vertices[166] = { 0.5f, 0.5f, -0.5f,		0,0,0,      1.f,1.f,		0.0f, 0.0f, -1.0f };
    vertices[167] = { 0.5f, -0.5f, -0.5f,	0,0,0,      1.f,0.f,		0.0f, 0.0f, -1.0f };

    // left face
    vertices[168] = { -0.5f, 0.5f, 0.5f,		0,0,0,      0.f,1.f,		-1.0f, 0.0f, 0.0f };
    vertices[169] = { -0.5f, -0.5f, 0.5f,	0,0,0,      0.f,0.f,		-1.0f, 0.0f, 0.0f };
    vertices[170] = { -0.5f, -0.5f, -0.5f,	0,0,0,      1.f,0.f,		-1.0f, 0.0f, 0.0f };

    vertices[171] = { -0.5f, 0.5f, 0.5f,		0,0,0,      0.f,1.f,		-1.0f, 0.0f, 0.0f };
    vertices[172] = { -0.5f, 0.5f, -0.5f,	0,0,0,      1.f,1.f,		-1.0f, 0.0f, 0.0f };
    vertices[173] = { -0.5f, -0.5f, -0.5f,	0,0,0,      1.f,0.f,		-1.0f, 0.0f, 0.0f };

    // top face
    vertices[174] = { -0.5f, 0.5f, 0.5f,		0,0,0,      0.f,0.f,		0.0f, 1.0f, 0.0f };
    vertices[175] = { 0.5f, 0.5f, 0.5f,		0,0,0,      1.f,0.f,		0.0f, 1.0f, 0.0f };
    vertices[176] = { 0.5f, 0.5f, -0.5f,		0,0,0,      1.f,1.f,		0.0f, 1.0f, 0.0f };

    vertices[177] = { -0.5f, 0.5f, 0.5f,		0,0,0,      0.f,0.f,		0.0f, 1.0f, 0.0f };
    vertices[178] = { -0.5f, 0.5f, -0.5f,	0,0,0,      0.f,1.f,		0.0f, 1.0f, 0.0f };
    vertices[179] = { 0.5f, 0.5f, -0.5f,		0,0,0,      1.f,1.f,		0.0f, 1.0f, 0.0f };

    // bottom face
    vertices[180] = { -0.5f, -0.5f, 0.5f,	0,0,0,      0.f,0.f,		0.0f, -1.0f, 0.0f };
    vertices[181] = { 0.5f, -0.5f, 0.5f,		0,0,0,      1.f,0.f,		0.0f, -1.0f, 0.0f };
    vertices[182] = { 0.5f, -0.5f, -0.5f,	0,0,0,      1.f,1.f,		0.0f, -1.0f, 0.0f };

    vertices[183] = { -0.5f, -0.5f, 0.5f,	0,0,0,      0.f,0.f,		0.0f, -1.0f, 0.0f };
    vertices[184] = { -0.5f, -0.5f, -0.5f,	0,0,0,      0.f,1.f,		0.0f, -1.0f, 0.0f };
    vertices[185] = { 0.5f, -0.5f, -0.5f,	0,0,0,      1.f,1.f,		0.0f, -1.0f, 0.0f };

    // SKYBOX
    
    vertices[186] = { -1.0f,  1.0f, -1.0f,	0,0,0,      0.f,0.f,		0.0f, -1.0f, 0.0f };
    vertices[187] = { -1.0f, -1.0f, -1.0f,		0,0,0,      1.f,0.f,		0.0f, -1.0f, 0.0f };
    vertices[188] = { 1.0f, -1.0f, -1.0f,	0,0,0,      1.f,1.f,		0.0f, -1.0f, 0.0f };

    vertices[189] = { 1.0f, -1.0f, -1.0f,	0,0,0,      0.f,0.f,		0.0f, -1.0f, 0.0f };
    vertices[190] = { 1.0f,  1.0f, -1.0f,	0,0,0,      1.f,1.f,		0.0f, -1.0f, 0.0f };
    vertices[191] = { -1.0f,  1.0f, -1.0f,	0,0,0,      0.f,0.f,		0.0f, -1.0f, 0.0f };

    vertices[192] = { -1.0f, -1.0f,  1.0f,		0,0,0,      1.f,0.f,		0.0f, -1.0f, 0.0f };
    vertices[193] = { -1.0f, -1.0f, -1.0f,	0,0,0,      1.f,1.f,		0.0f, -1.0f, 0.0f };
    vertices[194] = { -1.0f,  1.0f, -1.0f,	0,0,0,      0.f,0.f,		0.0f, -1.0f, 0.0f };

    vertices[195] = { -1.0f,  1.0f, -1.0f,	0,0,0,      0.f,1.f,		0.0f, -1.0f, 0.0f };
    vertices[196] = { -1.0f,  1.0f,  1.0f,	0,0,0,      1.f,1.f,		0.0f, -1.0f, 0.0f };
    vertices[197] = { -1.0f, -1.0f,  1.0f,	0,0,0,      0.f,0.f,		0.0f, -1.0f, 0.0f };

    vertices[198] = { 1.0f, -1.0f, -1.0f,		0,0,0,      1.f,0.f,		0.0f, -1.0f, 0.0f };
    vertices[199] = { 1.0f, -1.0f,  1.0f,	0,0,0,      1.f,1.f,		0.0f, -1.0f, 0.0f };
    vertices[200] = { 1.0f,  1.0f,  1.0f,	0,0,0,      0.f,0.f,		0.0f, -1.0f, 0.0f };

    vertices[201] = { 1.0f,  1.0f,  1.0f,	0,0,0,      0.f,1.f,		0.0f, -1.0f, 0.0f };
    vertices[202] = { 1.0f,  1.0f, -1.0f,	0,0,0,      1.f,1.f,		0.0f, -1.0f, 0.0f };
    vertices[203] = { 1.0f, -1.0f, -1.0f,	0,0,0,      0.f,0.f,		0.0f, -1.0f, 0.0f };

    vertices[204] = { -1.0f, -1.0f,  1.0f,		0,0,0,      1.f,0.f,		0.0f, -1.0f, 0.0f };
    vertices[205] = { -1.0f,  1.0f,  1.0f,	0,0,0,      1.f,1.f,		0.0f, -1.0f, 0.0f };
    vertices[206] = { 1.0f,  1.0f,  1.0f,	0,0,0,      0.f,0.f,		0.0f, -1.0f, 0.0f };

    vertices[207] = { 1.0f,  1.0f,  1.0f,	0,0,0,      0.f,1.f,		0.0f, -1.0f, 0.0f };
    vertices[208] = { 1.0f, -1.0f,  1.0f,	0,0,0,      1.f,1.f,		0.0f, -1.0f, 0.0f };
    vertices[209] = { -1.0f, -1.0f,  1.0f,	0,0,0,      0.f,0.f,		0.0f, -1.0f, 0.0f };

    vertices[210] = { -1.0f,  1.0f, -1.0f,		0,0,0,      1.f,0.f,		0.0f, -1.0f, 0.0f };
    vertices[211] = { 1.0f,  1.0f, -1.0f,	0,0,0,      1.f,1.f,		0.0f, -1.0f, 0.0f };
    vertices[212] = { 1.0f,  1.0f,  1.0f,	0,0,0,      0.f,0.f,		0.0f, -1.0f, 0.0f };

    vertices[213] = { 1.0f,  1.0f,  1.0f,	0,0,0,      0.f,1.f,		0.0f, -1.0f, 0.0f };
    vertices[214] = { -1.0f,  1.0f,  1.0f,	0,0,0,      1.f,1.f,		0.0f, -1.0f, 0.0f };
    vertices[215] = { -1.0f,  1.0f, -1.0f,	0,0,0,      0.f,0.f,		0.0f, -1.0f, 0.0f };

    vertices[216] = { -1.0f, -1.0f, -1.0f,		0,0,0,      1.f,0.f,		0.0f, -1.0f, 0.0f };
    vertices[217] = { -1.0f, -1.0f,  1.0f,	0,0,0,      1.f,1.f,		0.0f, -1.0f, 0.0f };
    vertices[218] = { 1.0f, -1.0f, -1.0f,	0,0,0,      0.f,0.f,		0.0f, -1.0f, 0.0f };

    vertices[219] = { 1.0f, -1.0f, -1.0f,	0,0,0,      0.f,1.f,		0.0f, -1.0f, 0.0f };
    vertices[220] = { -1.0f, -1.0f,  1.0f,	0,0,0,      1.f,1.f,		0.0f, -1.0f, 0.0f };
    vertices[221] = { 1.0f, -1.0f,  1.0f,	0,0,0,      1.f,1.f,		0.0f, -1.0f, 0.0f };
    
    // Create a vertex buffer object (VBO), and upload our vertices data to the VBO
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Create a vertex array object that contains data on how to map vertex attributes
    // (e.g., position, color) to vertex shader properties.
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Vertex attribute 0 - Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));

    // Vertex attribute 1 - Color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(offsetof(Vertex, r)));

    // Vertex attribute 2 - UV coordinate
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, u)));

    //Vertex Attribute 3 - Normal
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, nx)));

    glBindVertexArray(0);

    // Create a shader program

    GLuint program = CreateShaderProgram("main.vsh", "main.fsh");

    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint lightShader = CreateShaderProgram("light.vsh", "light.fsh");

    GLuint depthVAO;
    glGenVertexArrays(1, &depthVAO);
    glBindVertexArray(depthVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glBindVertexArray(0);
    GLuint depthShader = CreateShaderProgram("depth.vsh", "depth.fsh");


    GLuint skyboxVAO;
    glGenVertexArrays(1, &skyboxVAO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint skyboxShader = CreateShaderProgram("skybox.vsh", "skybox.fsh");

    GLuint reflectVAO;
    glGenVertexArrays(1, &reflectVAO);
    glBindVertexArray(reflectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Vertex attribute 0 - Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));


    //Vertex Attribute 3 - Normal
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, nx)));

    glEnableVertexAttribArray(0);

    GLuint reflectShader = CreateShaderProgram("cubeReflect.vsh", "cubeReflect.fsh");

    

    // Tell OpenGL the dimensions of the region where stuff will be drawn.
    // For now, tell OpenGL to use the whole screen

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    float angle = 0.0;
    //mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);


    glViewport(0, 0, windowWidth, windowHeight);



#pragma region SECONDTEXTURE


    //THIS IS FOR THE SECOND TEXTURE
    GLuint tex1;
    glGenTextures(1, &tex1);
    stbi_set_flip_vertically_on_load(true);
    int imageWidth, imageHeight, numChannels;

    unsigned char* imageData = stbi_load("cabinetTex.jpg", &imageWidth, &imageHeight, &numChannels, 0);


    if (imageData != nullptr)
    {
        glBindTexture(GL_TEXTURE_2D, tex1);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);


        stbi_image_free(imageData);
        imageData = nullptr;
    }
    else
    {
        std::cerr << "Failed to load image" << std::endl;
    }


    stbi_set_flip_vertically_on_load(true);


#pragma endregion

#pragma region THIRDTEXTURE


    //THIS IS FOR THE THIRD TEXTURE
    GLuint tex2;
    glGenTextures(1, &tex2);
    stbi_set_flip_vertically_on_load(true);

    imageData = stbi_load("woodTex.jpg", &imageWidth, &imageHeight, &numChannels, 0);

    if (imageData != nullptr)
    {
        glBindTexture(GL_TEXTURE_2D, tex2);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);


        stbi_image_free(imageData);
        imageData = nullptr;
    }
    else
    {
        std::cerr << "Failed to load image" << std::endl;
    }


    stbi_set_flip_vertically_on_load(true);


#pragma endregion
#pragma region FOURTHTEXTURE


    //THIS IS FOR THE FOURTH TEXTURE
    GLuint tex3;
    glGenTextures(1, &tex3);
    stbi_set_flip_vertically_on_load(true);

    imageData = stbi_load("bedTop.jpg", &imageWidth, &imageHeight, &numChannels, 0);

    if (imageData != nullptr)
    {
        glBindTexture(GL_TEXTURE_2D, tex3);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);


        stbi_image_free(imageData);
        imageData = nullptr;
    }
    else
    {
        std::cerr << "Failed to load image" << std::endl;
    }


    stbi_set_flip_vertically_on_load(true);


#pragma endregion


#pragma region SEVENTHTEX


    GLuint tex6;
    glGenTextures(1, &tex6);
    stbi_set_flip_vertically_on_load(true);

    imageData = stbi_load("tiles.jpg", &imageWidth, &imageHeight, &numChannels, 0);

    if (imageData != nullptr)
    {
        glBindTexture(GL_TEXTURE_2D, tex6);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);


        stbi_image_free(imageData);
        imageData = nullptr;
    }
    else
    {
        std::cerr << "Failed to load image" << std::endl;
    }


    stbi_set_flip_vertically_on_load(true);


#pragma endregion
#pragma region EIGHTTEX


    GLuint tex7;
    glGenTextures(1, &tex7);
    stbi_set_flip_vertically_on_load(true);

    imageData = stbi_load("sims.jpg", &imageWidth, &imageHeight, &numChannels, 0);

    if (imageData != nullptr)
    {
        glBindTexture(GL_TEXTURE_2D, tex7);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);


        stbi_image_free(imageData);
        imageData = nullptr;
    }
    else
    {
        std::cerr << "Failed to load image" << std::endl;
    }


    stbi_set_flip_vertically_on_load(true);


#pragma endregion

    GLuint skyboxTex;
    glGenTextures(1, &skyboxTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);


    std::vector<std::string> cubeMapFaces { 
            "space-skybox-right.jpg",
            "space-skybox-left.jpg",
            "space-skybox-top.jpg",
            "space-skybox-bottom.jpg",
            "space-skybox-front.jpg",

            "space-skybox-back.jpg",

    };
    stbi_set_flip_vertically_on_load(false);
    for (int i = 0; i < cubeMapFaces.size(); i++) {
        imageData = stbi_load(cubeMapFaces[i].c_str(), &imageWidth, &imageHeight, &numChannels, 0);
        if (imageData != nullptr) {
            std::cout << "Loading... " << cubeMapFaces[i] << std::endl;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
            stbi_image_free(imageData);
        }
        else
        {
            std::cerr << "Failed to load image "  << cubeMapFaces[i] << std::endl;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);


    // Framebuffer
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);

    // Framebuffer Texture Attachment
    GLuint framebufferTex;
    glGenTextures(1, &framebufferTex);
    glBindTexture(GL_TEXTURE_2D, framebufferTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebufferTex, 0);
    glDrawBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Error! Framebuffer not complete!" << std::endl;
    }
    
    


    glm::vec3 movingFacePosition = glm::vec3(1.5f, -1.f, -2.f);
    float movingFaceSpeed = 0.005f;
    glm::vec3 x = glm::vec3(1.0f,0,0);
    glm::vec3 z = glm::vec3(0,0,-1.0f);
    float xRot = 0;

    double timeout = 0;
    bool night = true;
    // Render loop

    // seed random generator
    srand(static_cast <unsigned> (time(0)));

    // can change only 
    bool canChange = true;
    float lightColorX = 1.0f;
    float lightColorY = 1.0f;
    float lightColorZ = 1.0f;

    while (!glfwWindowShouldClose(window))
    {
        // Clear the colors in our off-screen framebuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Use Shader Program
        glUseProgram(program);

        // Use the vertex array object that we created
        glBindVertexArray(vao);



        int upstate = glfwGetKey(window, GLFW_KEY_W);
        int downstate = glfwGetKey(window, GLFW_KEY_S);
        int rightstate = glfwGetKey(window, GLFW_KEY_D);
        int leftstate = glfwGetKey(window, GLFW_KEY_A);
        if (upstate == GLFW_PRESS) {
            cameraPos += cameraFront * cameraSpeed;
        }
        else if (downstate == GLFW_PRESS) {
            cameraPos -= cameraFront * cameraSpeed;
        }
        if (rightstate == GLFW_PRESS) {
            cameraPos += glm::normalize(glm::cross(cameraFront,cameraUp)) * cameraSpeed;
        }
        else if (leftstate == GLFW_PRESS) {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }

        int upArrowState = glfwGetKey(window, GLFW_KEY_UP);
        int downArrowState = glfwGetKey(window, GLFW_KEY_DOWN);
        int rightArrowState = glfwGetKey(window, GLFW_KEY_RIGHT);
        int leftArrowState = glfwGetKey(window, GLFW_KEY_LEFT);


        if (upArrowState == GLFW_PRESS) {
            movingFacePosition += z * movingFaceSpeed;
        }
        else if (downArrowState == GLFW_PRESS) {
            movingFacePosition -= z * movingFaceSpeed;
        }
        if (rightArrowState == GLFW_PRESS) {
            movingFacePosition += x * movingFaceSpeed;
        }
        else if (leftArrowState == GLFW_PRESS) {
            movingFacePosition -= x * movingFaceSpeed;
        }
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

#pragma region firstpass
        glUseProgram(depthShader);
        glBindVertexArray(depthVAO);
        glViewport(0, 0, 1024, 1024);

        glm::mat4 orthoProj = glm::ortho(-5.f, 5.0f, -5.0f, 5.0f, 0.1f, 11.f);
        glm::mat4 dirLightViewMat = glm::lookAt(glm::vec3(0.0f, 5.0f, 1.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
        glm::mat4 lightProj = orthoProj * dirLightViewMat;
        GLint orthoProjUnifLocation = glGetUniformLocation(depthShader, "orthoProjection");
        glUniformMatrix4fv(orthoProjUnifLocation, 1, GL_FALSE, glm::value_ptr(orthoProj));
        GLint dirLightViewMatUnifLocation = glGetUniformLocation(depthShader, "dirLightViewMatrix");
        glUniformMatrix4fv(dirLightViewMatUnifLocation, 1, GL_FALSE, glm::value_ptr(dirLightViewMat));
        GLint modelUniformLocation = glGetUniformLocation(depthShader, "model");


        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::mat4 planeTransform = glm::mat4(1.0f);
        glm::mat4 cabinetTransform = glm::mat4(1.0f);

        glm::mat4 midLampTransform = glm::mat4(1.0f);
        glm::mat4 botLampTransform = glm::mat4(1.0f);
        glm::vec3 color;

        glm::mat4 bedTransform = glm::mat4(1.0f);
        glm::mat4 belowBed = glm::mat4(1.0f);

        glm::mat4 sims = glm::mat4(1.0f);
        glm::mat4 simsBelow = glm::mat4(1.0f);
        glm::mat4 movingFace = glm::mat4(1.0f);



        
        cabinetTransform = glm::translate(cabinetTransform, glm::vec3(3.f, -4.f, -4.f));
        cabinetTransform = glm::scale(cabinetTransform, glm::vec3(2.f, 2.f, 2.f));
        glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(cabinetTransform));
        glDrawArrays(GL_TRIANGLES, 6, 36);


        midLampTransform = glm::translate(midLampTransform, glm::vec3(3.f, -2.5f, -4.f));
        midLampTransform = glm::scale(midLampTransform, glm::vec3(0.05f, 1.f, 0.05f));
        glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(midLampTransform));
        glDrawArrays(GL_TRIANGLES, 6, 36);


        botLampTransform = glm::translate(botLampTransform, glm::vec3(3.f, -3.f, -4.f));
        botLampTransform = glm::scale(botLampTransform, glm::vec3(0.5f, 0.3f, 0.5f));
        glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(botLampTransform));
        glDrawArrays(GL_TRIANGLES, 6, 36);


        bedTransform = glm::translate(bedTransform, glm::vec3(-2.4f, -4.6f, -4.f));
        bedTransform = glm::scale(bedTransform, glm::vec3(3.f, 3.7f, 3.f));

        glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(bedTransform));
        //bedtop
        glDrawArrays(GL_TRIANGLES, 60, 36);
        //bedbelow
        belowBed = glm::translate(belowBed, glm::vec3(-2.4f, -4.5f, -4.f));
        belowBed = glm::scale(belowBed, glm::vec3(3.f, 3.2f, 3.f));
        glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(belowBed));
        glDrawArrays(GL_TRIANGLES, 96, 36);

        xRot += 0.5f;
        movingFace = glm::translate(movingFace, movingFacePosition);
        movingFace = glm::scale(movingFace, glm::vec3(1.f, 1.f, 1.f));
        movingFace = glm::rotate(movingFace, glm::radians(xRot), glm::vec3(0.f, 1.0f, 0.f));

        glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(movingFace));
        glDrawArrays(GL_TRIANGLES, 150, 36);

        sims = glm::translate(sims, movingFacePosition + glm::vec3(0.f, 2.f, 0.f));
        sims = glm::scale(sims, glm::vec3(0.5f,0.5f,0.5f));
        sims = glm::rotate(sims, glm::radians(xRot), glm::vec3(0.f, 1.f, 0.f));
        /*glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(sims));
        glDrawArrays(GL_TRIANGLES, 132, 18);*/

        simsBelow = glm::translate(simsBelow, movingFacePosition + glm::vec3(0.f, 1.5f, 0.f));
        simsBelow = glm::scale(simsBelow, glm::vec3(0.5f, 0.5f, 0.5f));
        simsBelow = glm::rotate(simsBelow, glm::radians(180.f), glm::vec3(1.f, 0.f, 0.f));
        simsBelow = glm::rotate(simsBelow, glm::radians(xRot), glm::vec3(0.f, -1.f, 0.f));
        /*glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(simsBelow));
        glDrawArrays(GL_TRIANGLES, 132, 18);*/

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, windowWidth, windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#pragma endregion

#pragma region secondpass
        //Use Shader Program
        glUseProgram(program);

        // Use the vertex array object that we created
        glBindVertexArray(vao);

        GLint texUniformLocation = glGetUniformLocation(program, "tex");
        glUniform1i(texUniformLocation, 0);

        GLint bumpLoc = glGetUniformLocation(program, "bump");
        glUniform1i(bumpLoc, 1);

        GLint matUniformLocation = glGetUniformLocation(program, "transformationMatrix");

        GLint shadowMapTexUnifLocation = glGetUniformLocation(framebufferTex, "shadowMap");
        glUniform1i(shadowMapTexUnifLocation, 1);

        GLint viewUniformLocation = glGetUniformLocation(program, "view");
        glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, glm::value_ptr(view));

        GLint projectionUniformLocation = glGetUniformLocation(program, "projection");
        glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, glm::value_ptr(projection));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, framebufferTex);


        planeTransform = glm::rotate(planeTransform, glm::radians(0.0f), glm::vec3(0.f, 1.0f, 0.0f));
        planeTransform = glm::scale(planeTransform, glm::vec3(10.0f, 10.0f, 10.0f));

        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, tex6);
        glUniformMatrix4fv(matUniformLocation, 1, GL_FALSE, glm::value_ptr(planeTransform));

        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindTexture(GL_TEXTURE_2D, tex1);

        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D, tex1);

        glUniformMatrix4fv(matUniformLocation, 1, GL_FALSE, glm::value_ptr(cabinetTransform));
        glDrawArrays(GL_TRIANGLES, 6, 36);


        glBindTexture(GL_TEXTURE_2D, tex2);
        glUniformMatrix4fv(matUniformLocation, 1, GL_FALSE, glm::value_ptr(midLampTransform));
        glDrawArrays(GL_TRIANGLES, 6, 36);

        glBindTexture(GL_TEXTURE_2D, tex2);

        glUniformMatrix4fv(matUniformLocation, 1, GL_FALSE, glm::value_ptr(botLampTransform));
        glDrawArrays(GL_TRIANGLES, 6, 36);


        glBindTexture(GL_TEXTURE_2D, tex3);

        glUniformMatrix4fv(matUniformLocation, 1, GL_FALSE, glm::value_ptr(bedTransform));
        //bedtop
        glDrawArrays(GL_TRIANGLES, 60, 36);
        glBindTexture(GL_TEXTURE_2D, tex2);
        //bedbelow

    
        glUniformMatrix4fv(matUniformLocation, 1, GL_FALSE, glm::value_ptr(belowBed));
        glDrawArrays(GL_TRIANGLES, 96, 36);


        glBindTexture(GL_TEXTURE_2D, tex7);
        glUniformMatrix4fv(matUniformLocation, 1, GL_FALSE, glm::value_ptr(sims));

        glDrawArrays(GL_TRIANGLES, 132, 18);
        glUniformMatrix4fv(matUniformLocation, 1, GL_FALSE, glm::value_ptr(simsBelow));
        glDrawArrays(GL_TRIANGLES, 132, 18);

       

        //LIGHTS
        float constant = 1.0f;
        float linear = 0.14f;
        float quadratic = 0.07f;
        float materialShininess = 32.f;
#pragma endregion
#pragma region LIGHT_1

        //light
        // directionalLight
        Light light = {  };
        //glm::vec3 lightColor;

        int spaceBarState = glfwGetKey(window, GLFW_KEY_SPACE);
        timeout += 0.1;
        if (spaceBarState == GLFW_PRESS && timeout > 40) {
            timeout = 0;
            if (night == false) {
                night = true;
            }
            else {
                night = false;

            }
        }
        if (night == true) {
            light.lightColor = glm::vec3(0.2f, 1.0f, 1.0f);
            light.diffuseColor = light.lightColor * glm::vec3(0.5f);
            light.ambientColor = light.diffuseColor * glm::vec3(0.5f);
            canChange = true;
        }
        else {
            if (canChange) {
              
                lightColorX = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                lightColorY = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                lightColorZ = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            }
            canChange = false;
         
          
            light.lightColor = glm::vec3(lightColorX, lightColorY, lightColorZ);
            light.diffuseColor = light.lightColor * glm::vec3(1.f);
            light.ambientColor = light.diffuseColor * glm::vec3(1.f);
        }
        light.specular = glm::vec3(1.f, 1.f, 1.f);
        light.lightDirection = glm::vec3(0.f,-1.0f, 0.75f);
        light.materialAmbient = glm::vec3(1.f, 0.5f, 0.31f);

        light.materialDiffuse = glm::vec3(1.f, 0.5f, 0.31f);
        light.materialSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
        GLint lightUniformDirection = glGetUniformLocation(program, "light.direction");


        glUniform3fv(lightUniformDirection, 1, glm::value_ptr(light.lightDirection));
        GLint lightUniformAmbient = glGetUniformLocation(program, "light.ambient");
        glUniform3fv(lightUniformAmbient, 1, glm::value_ptr(light.ambientColor));
        GLint lightUniformDiffuse = glGetUniformLocation(program, "light.diffuse");
        glUniform3fv(lightUniformDiffuse, 1, glm::value_ptr(light.diffuseColor));
        GLint lightUniformSpecular = glGetUniformLocation(program, "light.specular");
        glUniform3fv(lightUniformSpecular, 1, glm::value_ptr(light.specular));


        GLint materialUniformAmbient = glGetUniformLocation(program, "material.ambient");
        glUniform3fv(materialUniformAmbient, 1, glm::value_ptr(light.materialAmbient));
        GLint materialUniformDiffuse = glGetUniformLocation(program, "material.diffuse");
        glUniform3fv(materialUniformDiffuse, 1, glm::value_ptr(light.materialDiffuse));
        GLint materialUniformSpecular = glGetUniformLocation(program, "material.specular");
        glUniform3fv(materialUniformSpecular, 1, glm::value_ptr(light.materialSpecular));
        GLint materialUniformShininess = glGetUniformLocation(program, "material.shininess");
        glUniform1f(materialUniformShininess, materialShininess);

#pragma endregion

#pragma region LIGHT_2
        Light light2 = {  };
        //Point
        // 2nd light
        light2.lightPos = glm::vec3(3.f, -2.f, -4.f);
        light2.lightColor.x = sin(glfwGetTime() * 2.0f);
        light2.lightColor.y = sin(glfwGetTime() * 0.7f);
        light2.lightColor.z = sin(glfwGetTime() * 1.3f);
        light2.diffuseColor = light2.lightColor * glm::vec3(0.5f);
        light2.ambientColor = light2.diffuseColor * glm::vec3(0.2f);
        light2.specular = glm::vec3(1.f, 1.f, 1.f);
        light2.lightDirection = glm::vec3(1.2f, 0.0f, 3.0f);
        light2.materialAmbient = glm::vec3(1.f, 0.5f, 0.31f);
        light2.materialDiffuse = glm::vec3(1.f, 0.5f, 0.31f);
        light2.materialSpecular = glm::vec3(0.5f, 0.5f, 0.5f);



        GLint light2UniformLocation = glGetUniformLocation(program, "plight.position");

        glUniform3fv(light2UniformLocation, 1, glm::value_ptr(light2.lightPos));
        GLint light2UniformDirection = glGetUniformLocation(program, "plight.direction");

        glUniform3fv(light2UniformDirection, 1, glm::value_ptr(light2.lightDirection));
        GLint light2UniformAmbient = glGetUniformLocation(program, "plight.ambient");
        glUniform3fv(light2UniformAmbient, 1, glm::value_ptr(light2.ambientColor));
        GLint light2UniformDiffuse = glGetUniformLocation(program, "plight.diffuse");
        glUniform3fv(light2UniformDiffuse, 1, glm::value_ptr(light2.diffuseColor));
        GLint light2UniformSpecular = glGetUniformLocation(program, "plight.specular");
        glUniform3fv(light2UniformSpecular, 1, glm::value_ptr(light2.specular));



        glUniform3fv(materialUniformAmbient, 1, glm::value_ptr(light2.materialAmbient));
        glUniform3fv(materialUniformDiffuse, 1, glm::value_ptr(light2.materialDiffuse));
        glUniform3fv(materialUniformSpecular, 1, glm::value_ptr(light2.materialSpecular));
        glUniform1f(materialUniformShininess, materialShininess);

        GLint pointConstantUniform = glGetUniformLocation(program, "plight.constant");
        glUniform1f(pointConstantUniform, constant);
        GLint pointLinearUniform = glGetUniformLocation(program, "plight.linear");
        glUniform1f(pointLinearUniform, linear);
        GLint pointQuadraticUniform = glGetUniformLocation(program, "plight.quadratic");
        glUniform1f(pointQuadraticUniform, quadratic);

#pragma endregion

        glBindVertexArray(lightVAO);
        glUseProgram(lightShader);
        GLint matLightUniformLocation = glGetUniformLocation(lightShader, "transformationMatrix");

        GLint viewLightUniformLocation = glGetUniformLocation(lightShader, "view");
        glUniformMatrix4fv(viewLightUniformLocation, 1, GL_FALSE, glm::value_ptr(view));

        GLint projectionLightUniformLocation = glGetUniformLocation(lightShader, "projection");
        glUniformMatrix4fv(projectionLightUniformLocation, 1, GL_FALSE, glm::value_ptr(projection));


        GLint light2ColorLocation = glGetUniformLocation(lightShader, "lightColor");
        glUniform3fv(light2ColorLocation, 1, glm::value_ptr(light2.lightColor));


        glm::mat4 topLampTransform = glm::mat4(1.0f);
        topLampTransform = glm::scale(topLampTransform, glm::vec3(0.8f, 0.8f, 0.8f));
        topLampTransform = glm::translate(topLampTransform, glm::vec3(3.75f, -2.5f, -5.f));
        glUniformMatrix4fv(matLightUniformLocation, 1, GL_FALSE, glm::value_ptr(topLampTransform));

        glDrawArrays(GL_TRIANGLES, 132, 18);

#pragma region reflection
        //REFLECTION
        glUseProgram(reflectShader);
        GLint cameraUniformLocation = glGetUniformLocation(reflectShader, "cameraPos");
        glUniform3fv(cameraUniformLocation, 1, glm::value_ptr(cameraPos));
        GLint modelRefLocation = glGetUniformLocation(reflectShader, "model");

        GLint cubeViewUniformLocation = glGetUniformLocation(reflectShader, "view");
        glUniformMatrix4fv(cubeViewUniformLocation, 1, GL_FALSE, glm::value_ptr(view));

        GLint cubeProjectionUniformLocation = glGetUniformLocation(reflectShader, "projection");
        glUniformMatrix4fv(cubeProjectionUniformLocation, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(reflectVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
        glUniformMatrix4fv(modelRefLocation, 1, GL_FALSE, glm::value_ptr(movingFace));

        glDrawArrays(GL_TRIANGLES, 150, 6);
        glDrawArrays(GL_TRIANGLES, 156, 6);
        glDrawArrays(GL_TRIANGLES, 162, 6);
        glDrawArrays(GL_TRIANGLES, 168, 6);
        glDrawArrays(GL_TRIANGLES, 174, 6);
        glDrawArrays(GL_TRIANGLES, 180, 6);
#pragma endregion

        // SKYBOX
        glDepthFunc(GL_LEQUAL); // disables depth so always at background
        glUseProgram(skyboxShader);
        view = glm::mat4(glm::mat3(view));
        GLint viewSkyUniformLocation = glGetUniformLocation(skyboxShader, "view");
        glUniformMatrix4fv(viewSkyUniformLocation, 1, GL_FALSE, glm::value_ptr(view));

        GLint projectionSkyUniformLocation = glGetUniformLocation(skyboxShader, "projection");
        glUniformMatrix4fv(projectionSkyUniformLocation, 1, GL_FALSE, glm::value_ptr(projection));
        
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
        glDrawArrays(GL_TRIANGLES, 186, 36);



        glBindVertexArray(0);
        glDepthFunc(GL_LESS);  // set depth function back to default




        // Tell GLFW to swap the screen buffer with the offscreen buffer
        glfwSwapBuffers(window);

        // Tell GLFW to process window events (e.g., input events, window closed events, etc.)
        glfwPollEvents();
    }

    // Clean

    glDeleteProgram(program);

    glDeleteBuffers(1, &vbo);

    glDeleteVertexArrays(1, &vao);

    glfwTerminate();

    return 0;
}

GLuint CreateShaderProgram(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
{
    GLuint vertexShader = CreateShaderFromFile(GL_VERTEX_SHADER, vertexShaderFilePath);
    GLuint fragmentShader = CreateShaderFromFile(GL_FRAGMENT_SHADER, fragmentShaderFilePath);
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    GLint linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        char infoLog[512];
        GLsizei infoLogLen = sizeof(infoLog);
        glGetProgramInfoLog(program, infoLogLen, &infoLogLen, infoLog);
        std::cerr << "program link error: " << infoLog << std::endl;
    }

    return program;
}

GLuint CreateShaderFromFile(const GLuint& shaderType, const std::string& shaderFilePath)
{
    std::ifstream shaderFile(shaderFilePath);
    if (shaderFile.fail())
    {
        std::cerr << "Unable to open shader file: " << shaderFilePath << std::endl;
        return 0;
    }

    std::string shaderSource;
    std::string temp;
    while (std::getline(shaderFile, temp))
    {
        shaderSource += temp + "\n";
    }
    shaderFile.close();

    return CreateShaderFromSource(shaderType, shaderSource);
}

GLuint CreateShaderFromSource(const GLuint& shaderType, const std::string& shaderSource)
{
    GLuint shader = glCreateShader(shaderType);

    const char* shaderSourceCStr = shaderSource.c_str();
    GLint shaderSourceLen = static_cast<GLint>(shaderSource.length());
    glShaderSource(shader, 1, &shaderSourceCStr, &shaderSourceLen);
    glCompileShader(shader);

    GLint compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE)
    {
        char infoLog[512];
        GLsizei infoLogLen = sizeof(infoLog);
        glGetShaderInfoLog(shader, infoLogLen, &infoLogLen, infoLog);
        std::cerr << "shader compilation error: " << infoLog << std::endl;
    }

    return shader;
}
