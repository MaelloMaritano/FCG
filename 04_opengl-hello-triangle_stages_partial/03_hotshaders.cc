#define GLAD_GL_IMPLEMENTATION // Necessary for the header-only version.
#include "glad/gl.h"

#include <SFML/Window.hpp>
#include <iostream>
#include <cstdlib>

#include "hotshaders.hh"


struct Setup
{
    sf::Window* window;

    Setup ()
    {
        sf::ContextSettings settings;
        settings.depthBits = 32;
        settings.stencilBits = 8;
        settings.antiAliasingLevel = 4;
        settings.attributeFlags = sf::ContextSettings::Attribute::Core;
        settings.majorVersion = 4;
        settings.minorVersion = 1;

        window = new sf::Window (
                                 sf::VideoMode({800, 600}),
                                 "SFML + OpenGL",
                                 sf::Style::Default,
                                 sf::State::Windowed,
                                 settings
                                 );
        window->setVerticalSyncEnabled (true);

        if (!window->setActive (true)) {
            std::cerr << "Failure: error during SFML OpenGL Activation." << std::endl;
            exit (1);
        }
        sf::ContextSettings gotten = window->getSettings();

        std::cout << "depth bits:" << gotten.depthBits << std::endl;
        std::cout << "stencil bits:" << gotten.stencilBits << std::endl;
        std::cout << "antialiasing level:" << gotten.antiAliasingLevel << std::endl;
        std::cout << "SFML GL version:" << gotten.majorVersion << "." << gotten.minorVersion << std::endl;

        int version = gladLoadGL (sf::Context::getFunction);
        if (!version) {
            std::cerr << "Failure: error during glad loading." << std::endl;
            exit (1);
        }
        std::cout << "GLAD GL version"<<GLAD_VERSION_MAJOR(version)<<"."<< GLAD_VERSION_MINOR(version)<<std::endl;
    }

    ~Setup ()
    {
        delete window;
    }
};



struct Scene
{
public:

    std::vector<float> points;
    std::vector<unsigned int> indices;
    GLuint vbo;
    GLuint ebo;
    GLuint vao;

    // Location of the uniform "input_color" for the fragment shader
    // Note: in a more complex program, locations could be stored in a map
    GLint mod_color_location;
    // we define two different colors as RGB static arrays
    static constexpr float darken[3] = {0.2, 0.2, 0.2};
    static constexpr float lighten[3] = {-0.2, -0.2, -0.2};

    Scene () { load (); }
    ~Scene () { clean (); }

    void load ()
    {
        // coordinates in clip space! no transformation involved
        points = {
            -0.7, 0.5, 0.0,
            0.2, 0.2, 0.2,

            0.0f,  0.5f,  0.0f,     // x,y,z of first point.
            0.8, 0.0, 0.0,          //colore 1
            0.5f, -0.5f,  0.0f,     // x,y,z of second point.
            0.8, 0.8, 0.0,          //colore 2
            -0.5f, -0.5f,  0.0f,    // x,y,z of third point.
            0.8, 0.0, 0.8,          //colore 3

            0.7, 0.5, 0.0,
            0.8, 0.8, 0.8,

        };

        indices={
            3, 1, 0,
            3, 2, 1,    //scambiando 2 e 1 faccia girata
            2, 4, 1,
        };

        vbo = 0;
        // we want just one buffer, and we retrieve the name OpenGL assigns to it.
        glGenBuffers (1, &vbo);
        // bind it as the current ARRAY_BUFFER
        glBindBuffer (GL_ARRAY_BUFFER, vbo);
        // transfer data from CPU RAM to GPU RAM.
        glBufferData (GL_ARRAY_BUFFER,
                      points.size() * sizeof (float),
                      points.data(),
                      GL_STATIC_DRAW);

        vao = 0;
        // we want just one buffer container, and we retrieve the name OpenGL assigns to it.
        glGenVertexArrays (1, &vao);
        // bind it as the current vao.
        glBindVertexArray (vao);
        // we describe how the attribute 0 is organized inside our buffer
        glVertexAttribPointer (0,
                               3,
                               GL_FLOAT,
                               GL_FALSE,
                               6*sizeof(float),
                               (void*)0);
        // we enable attribute 0 to be passed as input to the vertex shader
        glEnableVertexAttribArray (0);

        // attributo colore
        glVertexAttribPointer (1,
                               3,
                               GL_FLOAT,
                               GL_FALSE,
                               6*sizeof(float),
                               (void*)(3*sizeof(float)));   //offset di tipo void* perché vecchio
        glEnableVertexAttribArray (1);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            indices.size()*sizeof(unsigned int),
            indices.data(),
            GL_STATIC_DRAW);
    }

    void clean ()
    {
        glDeleteVertexArrays (1, &vao);
        glDeleteBuffers (1, &vbo);
    }

    // // when data will be dynamically loaded, reloading will be useful
    // void reload ()
    // {
    //     clean ();
    //     load ();
    // }
};




/////////////
// Draw!!! //
/////////////

void draw (Scene& scene, Shaders& shaders, float elapsed)
{
    // clear the buffers
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static float accumulator=0.0;
    accumulator+=elapsed;
    
    if(accumulator<0.1) glUniform3fv(scene.mod_color_location, 1, scene.lighten);
    else if(accumulator<0.2 || (accumulator>=3.0 && accumulator<4.0)) glUniform3f(scene.mod_color_location, 0.0, 0.0, 0.0);
    else if(accumulator<0.3) glUniform3fv(scene.mod_color_location, 1, scene.darken);
    else accumulator=0.0;

    // Draw points 0-3 from the currently bound VAO with current in-use shader
    //glDrawArrays(GL_TRIANGLES, scene.points.size());
    glDrawElements(GL_TRIANGLES, scene.indices.size(), GL_UNSIGNED_INT, 0);
}



////////////////////
// SFML Callbacks //
////////////////////

void handle (const sf::Event::KeyPressed& key, Scene& scene, Shaders& shaders)
{
    switch (key.scancode) {
    case sf::Keyboard::Scancode::Space:
        shaders.reload ("03_vertex.vert", "03_fragment.frag");
        glUseProgram (shaders.program);
        return;
    default:
        return;
    }
}




int main()
{
    // setup SFML + OpenGL, using GLAD
    Setup setup;
    sf::Window& window = *setup.window;

    // create a default scene
    Scene scene;

    // create default shaders
    // Shaders shaders;
    
    // load shaders from files
    Shaders shaders ("03_vertex.vert", "03_fragment.frag");

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Put the shader program, and the VAO, in focus in OpenGL's state machine
    // these can changed dynamically as needed in the loop too.
    glUseProgram (shaders.program);
    glBindVertexArray (scene.vao);

    scene.mod_color_location=glGetUniformLocation(shaders.program, "mod_color");
    
    ///////////////
    // Main loop //
    ///////////////

    sf::Clock clock;

    bool running = true;
    while (running)
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                running = false;
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
                glViewport (0, 0, resized->size.x, resized->size.y);
            else if (const auto* key_pressed = event->getIf<sf::Event::KeyPressed>())
                handle (*key_pressed, scene, shaders);
        }

        // draw given scene, with given shaders
        draw (scene, shaders, clock.restart().asSeconds());

        // end the current frame (internally swaps the front and back buffers)
        window.display();
    }

    return 0;


}
