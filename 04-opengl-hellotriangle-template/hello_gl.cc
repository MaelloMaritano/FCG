// must go before SFML!
#define GLAD_GL_IMPLEMENTATION // necessary when header-only
#include "glad/gl.h" // See file to check used options and generator URL

#include <SFML/Window.hpp>
#include <iostream>

int main()
{
    // Options for OpenGL context, to be kept in sync with GLAD options!
    sf::ContextSettings settings;
    settings.depthBits = 32;
    settings.stencilBits = 8;
    settings.antiAliasingLevel = 4;
    settings.attributeFlags = sf::ContextSettings::Attribute::Core;
    settings.majorVersion = 4;
    settings.minorVersion = 1;

    // Create the window with chosen options
    sf::Window window(
        sf::VideoMode({800, 600}),
        "SFML + OpenGL",
        sf::Style::Default,
        sf::State::Windowed,
        settings
    );
    window.setVerticalSyncEnabled (true);

    // Activate the window's OpenGL context
    if (!window.setActive (true)) {
        std::cerr << "Failure: error during SFML OpenGL Activation." << std::endl;
        return 1;
    }

    // Check what we have received back
    sf::ContextSettings gotten = window.getSettings();
    std::cout << "depth bits:" << gotten.depthBits << std::endl;
    std::cout << "stencil bits:" << gotten.stencilBits << std::endl;
    std::cout << "antialiasing level:" << gotten.antiAliasingLevel << std::endl;
    std::cout << "version:" << gotten.majorVersion << "." << gotten.minorVersion << std::endl;

    // GLAD magic!
    // SFML provides a function to retrieve OpenGL's functions at runtime
    // GLAD uses it to load all the OpenGL functions that are needed for our configuration
    int version = gladLoadGL (sf::Context::getFunction);
    if (!version) {
        std::cerr << "Failure: error during glad loading." << std::endl;
        return 1;
    }
    // Final check that we got what we want
    std::cout << "GLAD GL version"<<GLAD_VERSION_MAJOR(version)<<"."<< GLAD_VERSION_MINOR(version)<<std::endl;

    //vertices
    float points[]={
        0.0, 0.5, 0.0,
        0.5, -0.5, 0.0,
        -0.5, -0.5, 0.0,
    };

    //vertex buffer object
    GLuint vbo=0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    //vertex array object
    GLuint vao=0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    const char* vertex_shader=
        "version 410 core\n"
        "in vec3 vp;"
        "void main(){"
        "gl_Position=vec4(vp, 1.0); }";
    const char* fragment_shader=
        "version 410 core\n"
        "out vec4 frag_color;"
        "void main(){"
        "frag_color=vec4(0.5, 0.0, 0.5, 1,0); }";
    
    GLuint vert_s=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_s, 1, &vertex_shader, NULL);
    glCompileShader(vert_s);
    GLuint frag_s=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_s, 1, &fragment_shader, NULL);
    glCompileShader(frag_s);

    GLuint shader_program=glCreateProgram();
    glAttachShader(shader_program, vert_s);
    glAttachShader(shader_program, frag_s);
    glLinkProgram(shader_program);

    glUseProgram(shader_program);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // run the main loop
    bool running = true;
    while (running)
    {
        // handle events
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                // end the program
                running = false;
            }
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                // adjust the viewport when the window is resized
                glViewport (0, 0, resized->size.x, resized->size.y);
            }
        }

        // clear the buffers
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw here

        // end the current frame (internally swaps the front and back buffers)
        window.display();
    }

    return 0;
}
