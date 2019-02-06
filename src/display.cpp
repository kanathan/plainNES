#include "display.h"
#include "shader.h"
#include <glad/glad.h>
#include <SDL.h>
#include <iostream>

Display::Display()
{

}

Display::Display(int width, int height, const char* title,
    const char* vertexPath, const char* fragmentPath)
{
    init(width, height, title, vertexPath, fragmentPath);
}

Display::~Display()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
}

void Display::init(int width, int height, const char* title, const char* vertexPath, const char* fragmentPath)
{
    window = SDL_CreateWindow(title,
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                width, height,
                SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    gl_context = SDL_GL_CreateContext(window);

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    shader.init(vertexPath, fragmentPath);

    //Setup vertex data and buffers and configure
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Setup texture info
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);	// set texture wrapping to GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {0,0,0,1}; //Black border
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Display::loadTexture(int width, int height, uint8_t *data)
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    textureHeight = height;
    textureWidth = width;
}

void Display::renderFrame()
{
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, texture);

    shader.use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(window);
}

void Display::resizeImage()
{
    int winWidth, winHeight;
    float wRatio, hRatio;
    SDL_GL_GetDrawableSize(window, &winWidth, &winHeight);

    if((float)textureWidth/textureHeight > (float)winWidth/winHeight) { //Window taller
        wRatio = 1.0f;
        hRatio = ((float)winWidth/winHeight) / ((float)textureWidth/textureHeight);
    }
    else { //Window wider
        hRatio = 1.0f;
        wRatio = ((float)textureWidth/textureHeight) / ((float)winWidth/winHeight);
    }

    vertices[0] = vertices[5] = wRatio;
    vertices[10] = vertices[15] = -wRatio;
    vertices[1] = vertices[16] = hRatio;
    vertices[6] = vertices[11] = -hRatio;

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glViewport(0, 0, winWidth, winHeight);
}

uint32_t Display::getWindowID()
{
    return SDL_GetWindowID(window);
}