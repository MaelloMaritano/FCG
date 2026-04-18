////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <math.h>

struct State
{
	sf::RenderWindow window;
	State(unsigned w, unsigned h, std::string title)
	{
		window = sf::RenderWindow(sf::VideoMode({w, h}), title);
	}
};

void handle(const sf::Event::Closed &, State &gs)
{
	gs.window.close();
}

template <typename T>
void handle(const T &, State &gs)
{
	// All unhandled events will end up here
}

int main()
{
	State gs(800, 600, "Draw shapes");
	gs.window.setFramerateLimit(50);

	while (gs.window.isOpen()) // main loop
	{
		gs.window.handleEvents([&](const auto &event)
							{ handle(event, gs); });
							
		gs.window.clear();
		sf::RectangleShape rect({100.f, 50.f});
		rect.setOrigin({50.f, 25.f});
		rect.setPosition({100.f, 100.f});
		rect.move({500.f, 200.f});
		gs.window.draw(rect);
		gs.window.display();
	}
}
