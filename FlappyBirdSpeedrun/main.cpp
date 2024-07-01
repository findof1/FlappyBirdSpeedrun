#include <glad.h>
#include <glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <random>

const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main()
{
    FragColor = vec4(1.0, 0.5, 0.2, 1.0);
}
)";

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

bool CheckBoundingBoxCollision(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 size1, glm::vec3 size2) {
    glm::vec3 min1 = pos1 - size1 / 2.0f;
    glm::vec3 max1 = pos1 + size1 / 2.0f;
    glm::vec3 min2 = pos2 - size2 / 2.0f;
    glm::vec3 max2 = pos2 + size2 / 2.0f;

    bool overlapX = (min1.x <= max2.x && max1.x >= min2.x);
    bool overlapY = (min1.y <= max2.y && max1.y >= min2.y);
    bool overlapZ = (min1.z <= max2.z && max1.z >= min2.z);

    return overlapX && overlapY && overlapZ;
}

int main() {

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Flappy bird speedrun", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 1920, 1080);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 
         0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f, 
        -0.5f,  0.5f, 0.0f  
    };

    unsigned int indices[] = {
    0, 1, 2, 
    2, 3, 0  
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);



    
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), 
        glm::vec3(0.0f, 0.0f, 0.0f), 
        glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);

    glm::mat4 playerModel = glm::mat4(1.0f);
    playerModel = glm::scale(playerModel, glm::vec3(0.25f));
    playerModel = glm::translate(playerModel, glm::vec3(-5.0f, 0.0f, 0.0f));

    glm::mat4 pipeModel1 = glm::mat4(1.0f);
    pipeModel1 = glm::scale(pipeModel1, glm::vec3(0.3f, 10.0f, 1.0f));
    pipeModel1 = glm::translate(pipeModel1, glm::vec3(10.0f, -0.55f, 0.0f));

    glm::mat4 pipeModel2 = glm::mat4(1.0f);
    pipeModel2 = glm::scale(pipeModel2, glm::vec3(0.3f, 10.0f, 1.0f));
    pipeModel2 = glm::translate(pipeModel2, glm::vec3(10.0f, 0.55f, 0.0f));

    glm::mat4 pipeModel3 = glm::mat4(1.0f);
    pipeModel3 = glm::scale(pipeModel3, glm::vec3(0.3f, 10.0f, 1.0f));
    pipeModel3 = glm::translate(pipeModel3, glm::vec3(18.0f, -0.56f, 0.0f));

    glm::mat4 pipeModel4 = glm::mat4(1.0f);
    pipeModel4 = glm::scale(pipeModel4, glm::vec3(0.3f, 10.0f, 1.0f));
    pipeModel4 = glm::translate(pipeModel4, glm::vec3(18.0f, 0.54f, 0.0f));

    glm::mat4 pipeModel5 = glm::mat4(1.0f);
    pipeModel5 = glm::scale(pipeModel5, glm::vec3(0.3f, 10.0f, 1.0f));
    pipeModel5 = glm::translate(pipeModel5, glm::vec3(26.0f, -0.55f, 0.0f));

    glm::mat4 pipeModel6 = glm::mat4(1.0f);
    pipeModel6 = glm::scale(pipeModel6, glm::vec3(0.3f, 10.0f, 1.0f));
    pipeModel6 = glm::translate(pipeModel6, glm::vec3(26.0f, 0.55f, 0.0f));

    double lastFrameTime = glfwGetTime();
    float deltaTime = 0.0f;
    float yVel = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        double currentFrameTime = glfwGetTime();
        deltaTime = static_cast<float>(currentFrameTime - lastFrameTime);
        lastFrameTime = currentFrameTime;
        

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            yVel = 10.0f;
        else
            yVel -= 0.1f;


        playerModel = glm::translate(playerModel, glm::vec3(0.0f, yVel * deltaTime, 0.0f));
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        

        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

        

        
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(playerModel));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pipeModel1));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pipeModel2));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pipeModel3));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pipeModel4));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pipeModel5));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pipeModel6));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        pipeModel1 = glm::translate(pipeModel1, glm::vec3(-5.5f * deltaTime, 0.0f, 0.0f));
        pipeModel2 = glm::translate(pipeModel2, glm::vec3(-5.5f * deltaTime, 0.0f, 0.0f));
        pipeModel3 = glm::translate(pipeModel3, glm::vec3(-5.5f * deltaTime, 0.0f, 0.0f));
        pipeModel4 = glm::translate(pipeModel4, glm::vec3(-5.5f * deltaTime, 0.0f, 0.0f));
        pipeModel5 = glm::translate(pipeModel5, glm::vec3(-5.5f * deltaTime, 0.0f, 0.0f));
        pipeModel6 = glm::translate(pipeModel6, glm::vec3(-5.5f * deltaTime, 0.0f, 0.0f));

        

        if (CheckBoundingBoxCollision(glm::vec3(playerModel[3]), glm::vec3(pipeModel1[3]), glm::vec3(glm::vec3(playerModel[0]).x, glm::vec3(playerModel[1]).y, glm::vec3(playerModel[2]).z) , glm::vec3(glm::vec3(pipeModel1[0]).x, glm::vec3(pipeModel1[1]).y, glm::vec3(pipeModel1[2]).z)))
            exit(EXIT_SUCCESS);
        if (CheckBoundingBoxCollision(glm::vec3(playerModel[3]), glm::vec3(pipeModel2[3]), glm::vec3(glm::vec3(playerModel[0]).x, glm::vec3(playerModel[1]).y, glm::vec3(playerModel[2]).z), glm::vec3(glm::vec3(pipeModel2[0]).x, glm::vec3(pipeModel2[1]).y, glm::vec3(pipeModel2[2]).z)))
            exit(EXIT_SUCCESS);
        if (CheckBoundingBoxCollision(glm::vec3(playerModel[3]), glm::vec3(pipeModel3[3]), glm::vec3(glm::vec3(playerModel[0]).x, glm::vec3(playerModel[1]).y, glm::vec3(playerModel[2]).z), glm::vec3(glm::vec3(pipeModel3[0]).x, glm::vec3(pipeModel3[1]).y, glm::vec3(pipeModel3[2]).z)))
            exit(EXIT_SUCCESS);
        if (CheckBoundingBoxCollision(glm::vec3(playerModel[3]), glm::vec3(pipeModel4[3]), glm::vec3(glm::vec3(playerModel[0]).x, glm::vec3(playerModel[1]).y, glm::vec3(playerModel[2]).z), glm::vec3(glm::vec3(pipeModel4[0]).x, glm::vec3(pipeModel4[1]).y, glm::vec3(pipeModel4[2]).z)))
            exit(EXIT_SUCCESS);
        if (CheckBoundingBoxCollision(glm::vec3(playerModel[3]), glm::vec3(pipeModel5[3]), glm::vec3(glm::vec3(playerModel[0]).x, glm::vec3(playerModel[1]).y, glm::vec3(playerModel[2]).z), glm::vec3(glm::vec3(pipeModel5[0]).x, glm::vec3(pipeModel5[1]).y, glm::vec3(pipeModel5[2]).z)))
            exit(EXIT_SUCCESS);
        if (CheckBoundingBoxCollision(glm::vec3(playerModel[3]), glm::vec3(pipeModel6[3]), glm::vec3(glm::vec3(playerModel[0]).x, glm::vec3(playerModel[1]).y, glm::vec3(playerModel[2]).z), glm::vec3(glm::vec3(pipeModel6[0]).x, glm::vec3(pipeModel6[1]).y, glm::vec3(pipeModel6[2]).z)))
            exit(EXIT_SUCCESS);

        if (glm::vec3(pipeModel1[3]).x < -7.0f) {
            std::random_device rd;
            std::mt19937 gen(rd());

            std::uniform_real_distribution<float> dis(-0.03f, 0.03f);

            pipeModel1 = glm::mat4(1.0f);
            pipeModel1 = glm::scale(pipeModel1, glm::vec3(0.3f, 10.0f, 1.0f));
            pipeModel1 = glm::translate(pipeModel1, glm::vec3(8.0f, -0.55f + dis(gen), 0.0f));

            pipeModel2 = glm::mat4(1.0f);
            pipeModel2 = glm::scale(pipeModel2, glm::vec3(0.3f, 10.0f, 1.0f));
            pipeModel2 = glm::translate(pipeModel2, glm::vec3(8.0f, 0.55f + dis(gen), 0.0f));
        }

        if (glm::vec3(pipeModel3[3]).x < -7.0f) {
            std::random_device rd;
            std::mt19937 gen(rd());

            std::uniform_real_distribution<float> dis(-0.03f, 0.03f);

            pipeModel3 = glm::mat4(1.0f);
            pipeModel3 = glm::scale(pipeModel3, glm::vec3(0.3f, 10.0f, 1.0f));
            pipeModel3 = glm::translate(pipeModel3, glm::vec3(8.0f, -0.55f + dis(gen), 0.0f));

            pipeModel4 = glm::mat4(1.0f);
            pipeModel4 = glm::scale(pipeModel4, glm::vec3(0.3f, 10.0f, 1.0f));
            pipeModel4 = glm::translate(pipeModel4, glm::vec3(8.0f, 0.55f + dis(gen), 0.0f));
        }

        if (glm::vec3(pipeModel5[3]).x < -7.0f) {
            std::random_device rd;
            std::mt19937 gen(rd());

            std::uniform_real_distribution<float> dis(-0.03f, 0.03f);

            pipeModel5 = glm::mat4(1.0f);
            pipeModel5 = glm::scale(pipeModel5, glm::vec3(0.3f, 10.0f, 1.0f));
            pipeModel5 = glm::translate(pipeModel5, glm::vec3(8.0f, -0.55f + dis(gen), 0.0f));

            pipeModel6 = glm::mat4(1.0f);
            pipeModel6 = glm::scale(pipeModel6, glm::vec3(0.3f, 10.0f, 1.0f));
            pipeModel6 = glm::translate(pipeModel6, glm::vec3(8.0f, 0.55f + dis(gen), 0.0f));
        }
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}


