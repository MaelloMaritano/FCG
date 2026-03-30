////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <math.h>

////////////////////////////////////////////////////////////
/// Utility

template <typename T1, typename T2>
double dist(sf::Vector2<T1> p1, sf::Vector2<T2> p2)
{
	return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

const sf::Color GRAY(128, 128, 128, 255);
const sf::Color LIGHT_GRAY(170, 170, 170, 255);
const sf::Color DARK_GRAY(110, 110, 110, 255);

struct my_shape
{
	int type;
	int color;
	float size=30.f;
	sf::Vector2f position;
};


///
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// GUI state

struct State
{
	sf::RenderWindow window;
	int menubar_height=50;
	sf::Color colors[7]={sf::Color::White, sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Cyan, sf::Color::Yellow, sf::Color::Magenta};
	//bottoni selezionati in menubar
	int active_shape=0;
	int active_color=0;
	//lista delle shape disegnate
	std::vector<my_shape> shapes;
	int selected_shape=-1;

	sf::Vector2f mouse_pos={-1, -1};

	State(unsigned w, unsigned h, std::string title)
	{
		window = sf::RenderWindow(sf::VideoMode({w, h}), title);
	}
};

///
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Callback functions
void handle(const sf::Event::Closed &, State &gs)
{
	gs.window.close();
}

void handle(const sf::Event::TextEntered &textEnter, State &gs)
{
}

void handle(const sf::Event::KeyPressed &keyPressed, State &gs)
{
	if((keyPressed.scancode==sf::Keyboard::Scancode::Delete || keyPressed.scancode==sf::Keyboard::Scancode::Backspace) && gs.selected_shape!=-1)
	{
		gs.shapes.erase(gs.shapes.begin()+gs.selected_shape);
		gs.selected_shape=-1;
	}
}

void handle(const sf::Event::MouseMoved &mouseMoved, State &gs)
{
	if(gs.selected_shape!=-1)
	{
		gs.mouse_pos={static_cast<float>(mouseMoved.position.x), static_cast<float>(mouseMoved.position.y)};
		gs.shapes[gs.selected_shape].position={static_cast<float>(mouseMoved.position.x), static_cast<float>(mouseMoved.position.y)};
	}
}

void handle(const sf::Event::MouseButtonPressed &mouseBP, State &gs)
{
	sf::Vector2i pos=mouseBP.position;
	int button_size=gs.menubar_height;

	if(pos.y<button_size)	//click sulla menubar
	{
		if(pos.x<=button_size*5)
		{
			gs.active_shape=pos.x/button_size;
		}
		if(pos.x>=gs.window.getSize().x-(button_size*7))
		{
			gs.active_color=(pos.x-(gs.window.getSize().x-(button_size*7)))/button_size;
		}
	}
	else	//click sul canvas
	{
		if(mouseBP.button==sf::Mouse::Button::Left)
		{
			my_shape shape;

			shape.type=gs.active_shape;
			shape.color=gs.active_color;
			shape.position={static_cast<float>(pos.x), static_cast<float>(pos.y)};

			gs.shapes.emplace_back(shape);
		}
		else if(mouseBP.button==sf::Mouse::Button::Right)
		{
			for(int i=0; i<gs.shapes.size(); i++)
			{
				my_shape current=gs.shapes[i];

				if(dist(pos, current.position)<current.size)
				{
					gs.selected_shape=i;
					gs.mouse_pos={static_cast<float>(pos.x), static_cast<float>(pos.y)};
					return;
				}
			}
		}
	}
}

void handle(const sf::Event::MouseButtonReleased &, State &gs)
{
	if( gs.selected_shape!=-1) gs.selected_shape=-1;
}

void handle(const sf::Event::Resized &resized, State &gs)
{
	sf::FloatRect visibleArea({0.f, 0.f}, sf::Vector2f(resized.size));
	gs.window.setView(sf::View(visibleArea));
}

template <typename T>
void handle(const T &, State &gs)
{
	// All unhandled events will end up here
}
///
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Graphics
void doGUI(State &gs)
{
	float button_size=gs.menubar_height;
	float shape_radius=button_size*3.0/10.0;

	sf::RectangleShape button({button_size, button_size});
	
	button.setFillColor(sf::Color::Transparent);
	button.setOutlineThickness(-3.f);

	sf::CircleShape shape(shape_radius);
	shape.setOrigin({shape_radius, shape_radius});

	for(int i=0; i<5; i++)
	{
		button.setPosition({button_size*i, 0});
		if(i==gs.active_shape) button.setOutlineColor(LIGHT_GRAY);
		else button.setOutlineColor(GRAY);

		gs.window.draw(button);

		shape.setPosition({static_cast<float>(button_size*(i+0.5)), static_cast<float>(button_size*0.5)});
		if(i>0) shape.setPointCount(2+i);
		else shape.setPointCount(30);

		gs.window.draw(shape);
	}

	for(int i=0; i<7; i++)
	{
		button.setPosition({static_cast<float>(gs.window.getSize().x+button_size*(i-7)), 0.f});
		button.setFillColor(gs.colors[i]);
		if(i==gs.active_color) button.setOutlineColor(LIGHT_GRAY);
		else button.setOutlineColor(GRAY);

		gs.window.draw(button);
	}
}

void doGraphics(State &gs)
{
	gs.window.clear();
	doGUI(gs);

	// TODO: add here code to display shapes in your canvas
	for(int i=0; i<gs.shapes.size(); i++)
	{
		my_shape current=gs.shapes[i];

		sf::CircleShape shape(current.size);
		
		shape.setOrigin({current.size, current.size});
		if(current.type>0) shape.setPointCount(2+current.type);
		else shape.setPointCount(30);

		if(i==gs.selected_shape)
		{
			shape.setOutlineColor(LIGHT_GRAY);
			shape.setOutlineThickness(-3.f);
		}

		shape.setFillColor(gs.colors[current.color]);
		shape.setPosition(current.position);

		gs.window.draw(shape);
	}

	gs.window.display();
}
///
////////////////////////////////////////////////////////////

int main()
{
	State gs(800, 600, "Draw shapes");
	gs.window.setFramerateLimit(50);

	while (gs.window.isOpen()) // main loop
	{
		// event loop and handler through callbacks
		gs.window.handleEvents([&](const auto &event)
							{ handle(event, gs); });
		// Show update
		doGraphics(gs);
	}
}
