#include <SFML/Graphics.hpp>
#include "textures.hpp"

//////////////////////
// Initial defaults //
//////////////////////

// window
const char* window_title = "blockbreaker";
const unsigned window_width = 800;
const unsigned window_height = 600;
const float max_frame_rate = 60;

const float ball_radius=10.f;
const sf::Vector2f paddle_size={100.f, 16.f};
float speed=10;

struct Ball
{
    float radius;
    sf::Vector2f position;
    sf::Texture texture;
    Ball()
	{
        radius=ball_radius;
        position.x=(window_width/2)-ball_radius;
        position.y=window_height-paddle_size.y-(ball_radius*2);
        texture=sf::Texture(ball_png, ball_png_len);
        texture.setSmooth(1);
	}
    void draw(sf::RenderWindow& window)
    {
        sf::CircleShape circle=sf::CircleShape(radius);

        position.y=position.y-speed;
        if(position.y<=0 || position.y>=(window_height-paddle_size.y-radius)) speed=-1*speed;
        position.x=position.x-speed;
        if(position.x<=0 || position.x>=(window_height-radius)) speed=-1*speed;

        circle.setPosition(position);
        circle.setTexture(&texture);
        window.draw(circle);
    }
};

struct Paddle
{
    sf::Vector2f size;
    sf::Vector2f position;
    sf::Texture texture;
    Paddle()
	{
        size=paddle_size;
        position.x=(window_width/2)-(paddle_size.x/2);
        position.y=window_height-paddle_size.y;
        texture=sf::Texture(paddle_png, paddle_png_len);
        texture.setSmooth(1);
	}
    void draw(sf::RenderWindow& window)
    {
        sf::RectangleShape rectangle=sf::RectangleShape(size);
        rectangle.setPosition(position);
        rectangle.setTexture(&texture);
        window.draw(rectangle);
    }
};

struct State
{
	Ball ball;
    Paddle paddle;
    State() {}
    void draw(sf::RenderWindow& window)
    {
        ball.draw(window);
        paddle.draw(window);
    }
};

////////////
// Events //
////////////

void handle_close (sf::RenderWindow& window)
{
    window.close();
}
void handle_resize (const sf::Event::Resized& resized, sf::RenderWindow& window)
{   // constrain aspect ratio and map always the same portion of the world
    float aspect = static_cast<float>(window_width)/static_cast<float>(window_height);
    sf::Vector2u ws = resized.size;
    float new_aspect = static_cast<float>(ws.x)/static_cast<float>(ws.y);
    if (new_aspect < aspect)
        ws = {ws.x,static_cast<unsigned>(ws.x/aspect)};
        else
        ws = {static_cast<unsigned>(ws.y*aspect),ws.y};
    window.setSize(ws);
}



//////////
// Loop //
//////////

int main()
{
    sf::RenderWindow window (sf::VideoMode ({window_width, window_height}), window_title);
    window.setFramerateLimit (max_frame_rate);
    window.setMinimumSize(window.getSize());

    State gs;

    while (window.isOpen())
    {
        // events
        window.handleEvents (
                             [&window](const sf::Event::Closed&) { handle_close (window); },
                             [&window](const sf::Event::Resized& event) { handle_resize (event, window); }
        );


        // display
        window.clear (sf::Color::Black);

        gs.draw(window);

        window.display ();
    }
}
