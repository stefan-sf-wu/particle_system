#ifndef RENDERER_H_
#define RENDERER_H_

#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Common.h"
#include "Math.h"
#include "Particle.h"
#include "ParticleManager_MP.h"
#include "Timer.h"

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"   ourColor = aColor;\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, 1.0f);\n"
"}\n\0";


// View parameters
float theta = 0.0;
float phi = 0.0;
float camradius = 5.0;
float cameraspeed = 0.5;

float camX = camradius;
float camY = 0.0;
float camZ = 0.0;

// 
Vec generator_origin = {0.0, 0.0, 1.5};
float generator_speed = 0.01;

// Allow window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// Keyboard input: JKIL for camera motion (also escape to close window)
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    // J key will rotate left
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        theta -= cameraspeed;
        if (theta < 0.0) theta += 360.0;
        camX = camradius * cos(glm::radians(phi)) * cos(glm::radians(theta));
        camY = camradius * cos(glm::radians(phi)) * sin(glm::radians(theta));
        camZ = camradius * sin(glm::radians(phi));
    }
    // L key will rotate right
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        theta += cameraspeed;
        if (theta >= 360.0) theta -= 360.0;
        camX = camradius * cos(glm::radians(phi)) * cos(glm::radians(theta));
        camY = camradius * cos(glm::radians(phi)) * sin(glm::radians(theta));
        camZ = camradius * sin(glm::radians(phi));
    }
    // I key will rotate right
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        if (phi < 90.0-cameraspeed) phi += cameraspeed;
        camX = camradius * cos(glm::radians(phi)) * cos(glm::radians(theta));
        camY = camradius * cos(glm::radians(phi)) * sin(glm::radians(theta));
        camZ = camradius * sin(glm::radians(phi));
    }
    // K key will rotate right
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        if (phi > -90.0+cameraspeed) phi -= cameraspeed;
        camX = camradius * cos(glm::radians(phi)) * cos(glm::radians(theta));
        camY = camradius * cos(glm::radians(phi)) * sin(glm::radians(theta));
        camZ = camradius * sin(glm::radians(phi));
    }

    // A / D / W / S KEY TO MOVE GENERATOR ORIGIN OVER XY-PLAIN
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        generator_origin = vec_add(generator_origin, {0, -generator_speed, 0});
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        generator_origin = vec_add(generator_origin, {0, generator_speed, 0});
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        generator_origin = vec_add(generator_origin, {-generator_speed, 0, 0});
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        generator_origin = vec_add(generator_origin, {generator_speed, 0, 0});
    }

}

// This is a really bad "ball" - just an octahedron
float br = 0.025; // ball radius
float ball[] = {
    // positions   // colors
     br,  0,  0,   1.0f, 1.0f, 1.0f, // triangle 1
      0, br,  0,   1.0f, 1.0f, 1.0f,
      0,  0, br,   1.0f, 1.0f, 1.0f,
      0, br,  0,   1.0f, 1.0f, 1.0f, // triangle 2
    -br,  0,  0,   1.0f, 1.0f, 1.0f,
      0,  0, br,   1.0f, 1.0f, 1.0f,
    -br,  0,  0,   1.0f, 1.0f, 1.0f, // triangle 3
      0,-br,  0,   1.0f, 1.0f, 1.0f,
      0,  0, br,   1.0f, 1.0f, 1.0f,
      0,-br,  0,   1.0f, 1.0f, 1.0f, // triangle 4
     br,  0,  0,   1.0f, 1.0f, 1.0f,
      0,  0, br,   1.0f, 1.0f, 1.0f,
     br,  0,  0,   1.0f, 1.0f, 1.0f, // triangle 5
      0,-br,  0,   1.0f, 1.0f, 1.0f,
      0,  0,-br,   1.0f, 1.0f, 1.0f,
      0,-br,  0,   1.0f, 1.0f, 1.0f, // triangle 6
    -br,  0,  0,   1.0f, 1.0f, 1.0f,
      0,  0,-br,   1.0f, 1.0f, 1.0f,
    -br,  0,  0,   1.0f, 1.0f, 1.0f, // triangle 7
      0, br,  0,   1.0f, 1.0f, 1.0f,
      0,  0,-br,   1.0f, 1.0f, 1.0f,
      0, br,  0,   1.0f, 1.0f, 1.0f, // triangle 8
     br,  0,  0,   1.0f, 1.0f, 1.0f,
      0,  0,-br,   1.0f, 1.0f, 1.0f,
};

float plain[] = {
    // positions    // colors
    1,  -1,  0,     1.0f, 0.5f, 0.0f, 
    0.5, 0.2,  -1,      1.0f, 0.5f, 0.0f,
    -1.3,  -1, 0.5,      1.0f, 0.5f, 0.0f
};

float plain_2[] = {
    // positions    // colors

    0.5,  -1,  -1,    1.0f, 0.25f, 0.25f, 
    -1.3,  -0.5, -1.5,  1.0f, 0.25f, 0.25f,
    -0.5,  0.3,  -0.5,     1.0f, 0.25f, 0.25f
};

    
class Renderer
{
private:
    GLFWwindow* window;
    unsigned int shaderProgram;
    unsigned int fragmentShader;
    unsigned int vertexShader;

    unsigned int modelLoc;
    unsigned int viewLoc;
    unsigned int projectionLoc;

    glm::mat4 projection;
    glm::mat4 model;
    glm::mat4 view;

    glm::mat4* modelMatrices;
    unsigned int plain_buffer, ballbuffer, VAO, plain_buffer_2;

    ParticleManager particle_manager;
    Timer timer;

    void draw() 
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Set view matrix
        view = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // render and draw the plain
        glBindBuffer(GL_ARRAY_BUFFER, plain_buffer);
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // draw the plain (no model transform needed)
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // render and draw the plain_2
        glBindBuffer(GL_ARRAY_BUFFER, plain_buffer_2);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // render the ball
        glBindBuffer(GL_ARRAY_BUFFER, ballbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        for(int i = 0; i < PARTICLE_NUMBER; i++)
        {
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrices[i]));
            glDrawArrays(GL_TRIANGLES, 0, 24);
        }
        
        glfwSwapBuffers(window);
    }

public:
    Renderer();

    void initialize()
    {
        modelMatrices = new glm::mat4[PARTICLE_NUMBER];
        update_position_from_manager();
        particle_manager.add_plain(plain);
        particle_manager.add_plain(plain_2);

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, PROJ_NAME, NULL, NULL);
        if (window == NULL) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(window);
    
        // GLAD manages function pointers for OpenGL, so we want to initialize GLAD before we call any OpenGL function
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }

        // tell OpenGL the size of the rendering window so OpenGL knows how we want to display the data and coordinates with respect to the window.
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        // register a callback function on the window that gets called each time the window is resized.
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        // Enable depth buffering, backface culling
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // vertex shader
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        // fragment shader
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        // link shaders
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glUseProgram(shaderProgram);

        // Set up vertex array object (VAO) and vertex buffers for box and ball
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &plain_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, plain_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(plain), plain, GL_STATIC_DRAW);

        glGenBuffers(1, &plain_buffer_2);
        glBindBuffer(GL_ARRAY_BUFFER, plain_buffer_2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(plain_2), plain_2, GL_STATIC_DRAW);

        glGenBuffers(1, &ballbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, ballbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ball), ball, GL_STATIC_DRAW);

        // Declare model/view/projection matrices
        model = glm::mat4(1.0f);
        modelLoc = glGetUniformLocation(shaderProgram, "model");
        view = glm::mat4(1.0f);
        viewLoc = glGetUniformLocation(shaderProgram, "view");
        projection = glm::mat4(1.0f);
        projectionLoc = glGetUniformLocation(shaderProgram, "projection");

        // Set Projection matrix
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    }

    void start_looping()
    {
        while(!glfwWindowShouldClose(window) && !timer.is_time_to_stop())
        {
            processInput(window);

            if (timer.is_time_to_draw()) 
            {
                timer.update_next_display_time();
                update_position_from_manager();
                draw();
            }
            
            particle_manager.kill_particle();
            particle_manager.generate_particle(transform_gl2phy(generator_origin));
            particle_manager.compute_new_state();
            

            glfwPollEvents();
            
            timer.update_simulation_time();
        }
        shut_down();
    }


    void update_position_from_manager()
    {
        Vec pos;

        for(int i = 0; i < PARTICLE_NUMBER; i++)
        {
            if(particle_manager.activated_particle_[i] == true)
            {
                pos = particle_manager.particle_list_[i]->get_transformed_postion_for_renderer();
            }
            else
            {
                pos = {10.0, 10.0, 10.0};
            }

            model = glm::mat4(1.0f);
            model = glm::translate(model, (glm::vec3(pos.x,
                                                     pos.y, 
                                                     pos.z)));
            modelMatrices[i] = model;
        }


    }

    void shut_down()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &plain_buffer);
        glDeleteBuffers(1, &plain_buffer_2);
        glDeleteBuffers(1, &ballbuffer);
        glDeleteProgram(shaderProgram);
    }

    ~Renderer();
};

Renderer::Renderer() 
{
    timer.reset();
    particle_manager.reset(&timer);
}

Renderer::~Renderer() {}



#endif // RENDERER_H_