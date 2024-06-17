#include <glad/glad.h> // OpenGL function loader
#include <GLFW/glfw3.h>

#include <iostream>

const char *vertexShaderSource = "#version 460 core\n"
                                 "layout (location = 0) in vec3 pos;\n"
                                 "layout (location = 1) in vec3 color;\n"
                                 "out vec3 ourColor;\n"
                                 "void main() {\n"
                                     "gl_Position = vec4(pos, 1.0);\n"
                                     "ourColor = color;\n"
                                 "}\0";

const char *fragmentShaderSource = "#version 460 core\n"
                                   "out vec4 fragColor;\n"
                                   "in vec3 ourColor;\n"
                                   "void main() {\n"
                                       "fragColor = vec4(ourColor, 1.0);\n"
                                   "}\0";

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

int main() {
    glfwInit();

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window object
    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Make this the main context on current thread

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Set viewport dimensions
    glViewport(0, 0, 800, 600); // In pixels
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Adjust dimensions on window resize

    // Compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); // Create empty shader object
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);   // Attach shader source
    glCompileShader(vertexShader);

    // Check for compilation error
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); // Query shader for information
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Vertex shader compilation failed: " << infoLog << std::endl;
    }

    // Compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check for compilation error
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Fragment shader compilation failed: " << infoLog << std::endl;
    }

    // Create shader program object and link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for link error
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Shader linking failed: " << infoLog << std::endl;
    }

    // Shader objects no longer needed
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Position + color
    float vertices[] = {
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
         0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f  // top
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    // Create and bind vertex array for storing vertex attribute configurations
    unsigned int vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // Create, bind, and populate vertex buffer
    unsigned int vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // One write, many reads

    // Create, bind, and populate element buffer (for storing indices)
    //unsigned int elementBufferObject;
    //glGenBuffers(1, &elementBufferObject);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Configure and enable position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // Enable attribute at location 0

    // Configure and enable color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Clear color buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Activate shader program
        glUseProgram(shaderProgram);

        //// Update uniform color
        //float timeValue = glfwGetTime();
        //float greenValue = sin(timeValue) / 2.0f + 0.5f; // Vary from 0 to 1
        //int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        //glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

        // Draw rectangle from index buffer
        glBindVertexArray(vertexArrayObject); // Load VBO, EBO, and attributes
        //glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &vertexBufferObject);
    glDeleteVertexArrays(1, &vertexArrayObject);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    // Close window on escape
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}