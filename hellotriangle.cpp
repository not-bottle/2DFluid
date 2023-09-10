#include <iostream>
#include <stdlib.h>

#include <glad/glad.h>
#include <SDL2/SDL.h>

// Start up SDL and create a window
bool init();

// Code for checking attributes I might not need
void check_attributes();

// Event polling code contained in here
void event_handling();

// Free media and shut down SDL
void close();

// SDL Objects
SDL_Window* gWindow = NULL; // Main window
SDL_Surface* gScreenSurface = NULL; // The window's surface
SDL_GLContext gContext = NULL;

// Awful way to handle quitting in event loop (change this)
bool gQuit = false;

const int GLMAJORVERSION = 3;
const int GLMINORVERSION = 3;
const int GLPROFILEMASK = SDL_GL_CONTEXT_PROFILE_CORE;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5,  -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f,
};

// Simple shader to directly output triangle coordinates.
// Note: triangle coords already in NDC, so no further 
// transforms needed.
// Note: This implies I should put all the perspective 
// transformation/normalization code in here.
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);"
    "}\0";

// Simple fragment shader to output an orange-ish colour
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColour;\n"
    "void main()\n"
    "{\n"
    "  FragColour = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n";
// (Unused currently)
void check_attributes()
{
    // Check OpenGL attributes
    int check = 0;
    if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &check))
    {
        std::cerr << "SDL could not get major version! SDL Error: " << SDL_GetError() << '\n';
    } else {
        std::cerr << "Requested major version: " << GLMAJORVERSION << ", Actual major version: " << check << '\n';
    }
    if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &check))
    {
        std::cerr << "SDL could not get minor version! SDL Error: " << SDL_GetError() << '\n';
    } else {
        std::cerr << "Requested minor version: " << GLMINORVERSION << ", Actual minor version: " << check << '\n';
    }
    if (SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &check))
    {
        std::cerr << "SDL could not get profile mask! SDL Error: " << SDL_GetError() << '\n';
    } else {
        std::cerr << "Requested profile mask: " << GLPROFILEMASK << ", Actual profile mask: " << check << '\n';
    }
}

bool init()
{
    bool success = true;

    // Initialize SDL library (suprised by how much worked without this lol)
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    // Set OpenGL attributes
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    if ( SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << '\n';
        success = false;
    } else {
        gWindow = SDL_CreateWindow( "LearnOpenGL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                                    SCREEN_WIDTH, SCREEN_HEIGHT, 
                                    SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        
        if (gWindow == NULL)
        {
            std::cerr << "SDL Window could not be created! SDL Error: " << SDL_GetError() << '\n';
            success = false;
        } else {
            // Create OpenGL Context for window (and make it current)
            gContext = SDL_GL_CreateContext(gWindow);

            check_attributes();

            // Get Screen Surface from window
            gScreenSurface = SDL_GetWindowSurface(gWindow);

            // Initialize glad using SDLs loader
            // We are casting the SDL_GL_GetProcAddress function object to the GLADloadproc type
            // 
            // NOTE: Without this we would have to define a function pointer prototype for every 
            // OpenGL function we wish to call, then call SDL_GL_GetProcAddress and check that it 
            // returns a valid value.
            if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
            {
                std::cerr << "Failed to initialize GLAD" << std::endl;
                close();
                exit(1);
            }

            // Set OpenGL screen coordinates to match the SDL screen size
            // 
            // NOTE: It is possible to set these values as smaller than the
            // window size. This could be useful if you wanted to put otherwise 
            // rendered ui or other info around the main display.
            glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
    }
    return success;
}

void event_handling()
{
    SDL_Event e;

    // Poll event, removing it from the queue
    while (SDL_PollEvent(&e))
    {
        // If the event is X-ing out of the window set gQuit to true
        if (e.type == SDL_QUIT)
        {
            gQuit = true;
        }

        // Window event handling
        if (e.type == SDL_WINDOWEVENT)
        {
            switch (e.window.event)
            {
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    glViewport(0, 0, e.window.data1, e.window.data2);
                    break;
            }
        } 
    }
}

void close()
{
    SDL_DestroyWindow(gWindow); // This also destroys gscreenSurface
    gWindow = NULL;
    gScreenSurface = NULL;

    SDL_Quit();
}


int main(int argc, char* args[])
{
    if(!init())
    {
        exit(1);
    }

    // Create vertex Array object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    // Bind the vertex array
    // Note: This does not appear to do anything, but
    // internally it will save all the vertex attribute
    // settings, configurations and the associated buffer 
    // objects.
    // Note: in practice you would make multiple VAOs for
    // each object/settings configuration, set everything up,
    // then bind and unbind them as you draw each object.
    // Note: Without the array objects, settings would need
    // to be reconfigured before drawing Each Object.
    // Question: If settings are the same for all your objects
    // would you only need to setup the buffers? (I don't think so)
    glBindVertexArray(VAO);
    // Create vertex buffer object
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    // Bind vertex buffer to GL_ARRAY_BUFFER type
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData:
    // Function for passing user-defined data into an opengl buffer
    // GL_ARRAY_BUFFER: specifies we want data from the buffers
    //   currently bound to the GL_ARRAY_BUFFER type
    // GL_STATIC_DRAW: gives Opengl a hint to how the data will be used.
    //   GL_STATIC_DRAW tells Opengl that the data will be static, but drawn
    //   from frequently. This means it should be optimized for fast reads,
    //   but writes are not as important
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // SHADER CREATION:
    // Vertex shader:
    // error checking vars
    int success;
    char infoLog[512];
    // Similar object initialization code as above;
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // Attach source code string to the object
    // (2nd arg is how many strings we are passing)
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    // Compile! (dynamically!):
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
                    infoLog << std::endl;
        exit(1);
    }
    // Fragment shader:
    unsigned fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" <<
                    infoLog << std::endl;
        exit(1);
    }

    // Create shader program:
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    // Link compiled shaders together in shader program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Check if linking failed
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" <<
                    infoLog << std::endl;
        exit(1);
    }
    // Activate shader program object
    // Note: This workflow means you could preload and link
    // lots of shaders and switch them out
    glUseProgram(shaderProgram);

    // Delete shader objects (they are no longer needed)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // We need to tell OpenGL how it should interpret the
    // vertex data:
    // (Note: This is because you can specify your own input
    //  as a vertex attribute)
    // Lots of arguments:
    // 1 - vertex attribute number (specified in vertex shader)
    // 2 - size of vertex attribute (how many vertices)
    // 3 - data type
    // 4 - Normalize data? (?)
    // 5 - Stride - space between vertex attributes
    // 6 - Offset in buffer
    // Note: This will be called on the VBO currently bound to
    //   GL_ARRAY_BUFFER (unchanged in this case)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // Finally Enable the vertex attribute 
    // (using its location as an arg)
    glEnableVertexAttribArray(0);

    while (!gQuit)
    {
        // Input

        // Clear the screen buffer

        glClearColor(.2, .5, .5, 1.0); // State setting
        glClear(GL_COLOR_BUFFER_BIT); // State Using

        // Rendering
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Events
        event_handling();

        // Swap buffers
        SDL_GL_SwapWindow(gWindow);
    }
    
    close();

    return 0;
}