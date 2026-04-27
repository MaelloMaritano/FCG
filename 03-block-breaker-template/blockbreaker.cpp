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

const float ball_initial_speed=200;
const float ball_initial_angle=-90;
const float paddle_initial_speed=500;

struct Ball
{
	float radius;
	sf::Vector2f position;
	sf::Texture texture;
	float speed;
	float angle;

	Ball()
	{
		radius=ball_radius;
		position={((window_width/2)-ball_radius), (window_height-paddle_size.y-(ball_radius*2))};
		texture=sf::Texture(ball_png, ball_png_len);
		texture.setSmooth(1);
		speed=ball_initial_speed;
		angle=ball_initial_angle;
	}

	sf::Angle reflect_horizontal(sf::Angle angle)
	{

	}
	sf::Angle reflect_vertical(sf::Angle angle)
	{

	}
	void move(float elapsed, sf::RenderWindow& window)
	{
		if(position.x<=0 || position.x>=window.getSize().x-radius) reflect_horizontal(sf::degrees(angle));
		else if(position.y>=window.getSize().y) reflect_vertical(sf::degrees(angle));
		sf::Vector2f displacement=sf::Vector2f(speed*elapsed, sf::degrees(angle));
		position+=displacement;
	}
	void draw(sf::RenderWindow& window)
	{
		sf::CircleShape circle=sf::CircleShape(radius);
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
	float speed;

	Paddle()
	{
		size=paddle_size;
		position.x=(window_width/2)-(paddle_size.x/2);
		position.y=window_height-paddle_size.y;
		texture=sf::Texture(paddle_png, paddle_png_len);
		texture.setSmooth(1);
		speed=paddle_initial_speed;
	}

	void draw(sf::RenderWindow& window)
	{
		sf::RectangleShape rectangle=sf::RectangleShape(size);
		rectangle.setPosition(position);
		rectangle.setTexture(&texture);
		window.draw(rectangle);
	}
	void move_left(float elapsed)
	{
		position.x-=speed*elapsed;
		if(position.x<=0) position.x=0;
	}
	void move_right(float elapsed, sf::RenderWindow& window)
	{
		position.x+=speed*elapsed;
		if(position.x>=window.getSize().x-size.x) position.x=window.getSize().x-size.x;
	}
};

struct State
{
	Ball ball;
	Paddle paddle;
	bool pause=true;
	bool move_paddle_left=false;
	bool move_paddle_right=false;

	State() {}
	void update(float elapsed, sf::RenderWindow& window)
	{
		if(!pause)
		{
			ball.move(elapsed);
			if(move_paddle_left) paddle.move_left(elapsed);
			if(move_paddle_right) paddle.move_right(elapsed, window);
		}
	}
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

void handle (const sf::Event::KeyPressed& key, State& state)
{
	switch(key.scancode)
	{
		case sf::Keyboard::Scancode::Space:
			state.pause=!state.pause;
			break;
		case sf::Keyboard::Scancode::Left:
			state.move_paddle_left=true;
			break;
		case sf::Keyboard::Scancode::Right:
			state.move_paddle_right=true;
			break;
		default:
			break;
	}
}

void handle (const sf::Event::KeyReleased& key, State& state)
{
	switch(key.scancode)
	{
		case sf::Keyboard::Scancode::Left:
			state.move_paddle_left=false;
			break;
		case sf::Keyboard::Scancode::Right:
			state.move_paddle_right=false;
			break;
		default:
			break;
	}
}

void handle (const sf::Event::FocusLost&, State& state)
{
	state.pause=true;
}

template <typename T>
void handle (T& event, State& state) {}


//////////
// Loop //
//////////

int main()
{
	sf::RenderWindow window (sf::VideoMode ({window_width, window_height}), window_title);
	window.setFramerateLimit (max_frame_rate);
	window.setMinimumSize(window.getSize());

	State state;
	sf::Clock clock;

	while (window.isOpen())
	{
		// events
		window.handleEvents (
							[&window](const sf::Event::Closed&) { handle_close (window); },
							[&window](const sf::Event::Resized& event) { handle_resize (event, window); },
							[&state] (const auto& event) { handle (event, state); }
		);

		state.update(clock.restart().asSeconds(), window);

		// display
		window.clear (sf::Color::Black);

		state.draw(window);

		window.display ();
	}
}
