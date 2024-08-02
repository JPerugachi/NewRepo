#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <learnopengl/stb_image.h>

// Callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// Settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1000;

// Camera
Camera camera(glm::vec3(0.0f, 1.8f, 3.0f)); // Altura de 1.8f para simular la altura de una persona
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Constants
const float CAMERA_HEIGHT = 1.8f; // Altura fija de la cámara para simular una persona
const glm::vec2 ROOM_MIN = glm::vec2(-12.0f, -12.0f); // Coordenadas mínimas de la sala (paredes exteriores)
const glm::vec2 ROOM_MAX = glm::vec2(12.0f, 12.0f);   // Coordenadas máximas de la sala (paredes exteriores)

// Define las cajas de colisión para cada modelo
struct AABB {
    glm::vec3 center;  // Centro de la caja
    glm::vec3 halfSize; // Mitad del tamaño (dimensiones / 2)
};

AABB model1AABB = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 1.0f, 0.5f) };
AABB model2AABB = { glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.7f, 0.7f, 0.7f) };
AABB model3AABB = { glm::vec3(2.0f, 0.0f, -1.5f), glm::vec3(0.04f, 0.04f, 0.04f) };
AABB model4AABB = { glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
AABB model5AABB = { glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f) };

// Función de detección de colisiones
bool CheckCollision(AABB box, glm::vec3 position) {
    glm::vec3 min = box.center - box.halfSize;
    glm::vec3 max = box.center + box.halfSize;

    // Comprobar si la posición está dentro de los límites de la caja
    return (position.x > min.x && position.x < max.x &&
        position.z > min.z && position.z < max.z);
}

int main()
{
    // GLFW: Initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Proyecto - Juego Backroom", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLAD: Load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Flip loaded textures on the y-axis before loading model
    stbi_set_flip_vertically_on_load(true);

    // Configure global OpenGL state
    glEnable(GL_DEPTH_TEST);

    // Build and compile shaders
    Shader ourShader("shaders/shader_model_loading.vs", "shaders/shader_model_loading.fs");

    // Load models
    Model ourModel("D:/Descargas/Backrooms-OpenGL-main/Backrooms-OpenGL-main/backroom_scene/model.obj");
    Model otherModel("D:/Descargas/mO5/model.obj"); // Ruta al nuevo modelo
    Model thirdModel("D:/Descargas/globo_entidad/model2.obj"); // Ruta al tercer modelo
    Model fourthModel("D:/Descargas/mosuntroGrande/model.obj"); // Ruta al cuarto modelo
    Model fiveModel("D:/Descargas/jerry/model.obj"); // Ruta al cuarto modelo

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Lógica de tiempo por cuadro
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        processInput(window);

        // Posición anterior de la cámara
        glm::vec3 prevPosition = camera.Position;

        // Movimiento de la cámara (sin movimiento automático hacia adelante)
        camera.Position.y = CAMERA_HEIGHT;
        camera.Position.x = glm::clamp(camera.Position.x, ROOM_MIN.x, ROOM_MAX.x);
        camera.Position.z = glm::clamp(camera.Position.z, ROOM_MIN.y, ROOM_MAX.y);

        // Verificar colisiones y revertir la posición si es necesario
        if (CheckCollision(model1AABB, camera.Position) ||
            CheckCollision(model2AABB, camera.Position) ||
            CheckCollision(model3AABB, camera.Position) ||
            CheckCollision(model4AABB, camera.Position) ||
            CheckCollision(model5AABB, camera.Position)) {
            camera.Position = prevPosition; // Revertir a la posición anterior si hay colisión
        }

        // Renderizar
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activar el shader
        ourShader.use();

        // Transformaciones de vista/proyección
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // Dibujar los modelos
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, glm::vec3(sin(glfwGetTime()) * 1.3f, 2.0f, 0.0f));
        model2 = glm::scale(model2, glm::vec3(0.7f, 0.7f, 0.7f));
        ourShader.setMat4("model", model2);
        otherModel.Draw(ourShader);

        glm::mat4 model3 = glm::mat4(1.0f);
        model3 = glm::translate(model3, glm::vec3(2.0f, 0.0f, -1.5f));
        model3 = glm::scale(model3, glm::vec3(0.04f, 0.04f, 0.04f));
        ourShader.setMat4("model", model3);
        thirdModel.Draw(ourShader);

        glm::mat4 model4 = glm::mat4(1.0f);
        model4 = glm::translate(model4, glm::vec3(1.0f, 0.0f, 1.0f));
        model4 = glm::scale(model4, glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setMat4("model", model4);
        fourthModel.Draw(ourShader);

        glm::mat4 model5 = glm::mat4(1.0f);
        model5 = glm::translate(model5, glm::vec3(0.0f, 2.0f, 0.0f));
        model5 = glm::scale(model5, glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setMat4("model", model5);
        fiveModel.Draw(ourShader);

        // GLFW: intercambiar buffers y poll de eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Terminar GLFW
    glfwTerminate();
    return 0;
}

// Procesa todos los eventos de entrada: consulta si se han presionado/released teclas este cuadro y reacciona en consecuencia
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// Procesa el tamaño del framebuffer para ajustar el viewport
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Procesa el movimiento del mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // al revés porque y-coordinates van de abajo hacia arriba

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// Procesa el scroll del mouse
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
