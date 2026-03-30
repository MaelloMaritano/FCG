#include <SFML/Graphics.hpp>

#include <vector>
#include <string>

const int FONT_SIZE = 24;	//in pixels
const int FONT_WIDTH = 14;	//in pixels

// GUI STATE

struct State
{
	sf::RenderWindow window;
	const sf::Font font{"resources/dejavu-sans-mono-font/DejavuSansMono-5m7L.ttf"};
	std::vector<std::string> log;
	sf::IntRect text_view;
	sf::Vector2i cursor_pos;

	State(unsigned w, unsigned h, std::string title)
	{
		window = sf::RenderWindow(sf::VideoMode({w, h}), title);
		window.setFramerateLimit(60);
		log.resize(1, "");
		text_view=sf::IntRect({0,0}, sf::Vector2i({static_cast<int>(w/FONT_WIDTH-2), static_cast<int>(h/FONT_SIZE-2)}));
		cursor_pos={0,0};
	}

	void adjustView()
	{
		//resize happened
		int w=(window.getSize().x/FONT_WIDTH)-2;
		int h=(window.getSize().y/FONT_SIZE)-2;

		if(w<0) w=0;
		if(h<0) h=0;
		
		text_view.size={w, h};

		//cursor moved
		if(cursor_pos.x<text_view.position.x) text_view.position.x=cursor_pos.x;
		if(cursor_pos.x>(text_view.position.x+text_view.size.x-1)) text_view.position.x=cursor_pos.x-text_view.size.x+1;
		if(cursor_pos.y<text_view.position.y) text_view.position.y=cursor_pos.y;
		if(cursor_pos.y>(text_view.position.y+text_view.size.y-1)) text_view.position.y=cursor_pos.y-text_view.size.y+1;
		// si possono usare anche assegnazioni con min e max per tenere cursore nei bordi
	}
};

// CALLBACK FUNCTIONS

void handle(const sf::Event::Closed &, State &gs)
{
	gs.window.close();
}

void handle(const sf::Event::KeyPressed &keyPressed, State &gs)
{
	switch(keyPressed.scancode)
	{
		case (sf::Keyboard::Scancode::Up):
		if(gs.cursor_pos.y==0) return;
		gs.cursor_pos.y--;
		if(gs.log[gs.cursor_pos.y].size()<gs.cursor_pos.x) gs.cursor_pos.x=gs.log[gs.cursor_pos.y].size();
		// oppure gs.cursor_pos.x=std::min((int)gs.log[gs.cursor_pos.y].size(), gs.cursor_pos.x);
		break;

		case (sf::Keyboard::Scancode::Down):
		if(gs.cursor_pos.y==gs.log.size()-1) return;
		gs.cursor_pos.y++;
		if(gs.log[gs.cursor_pos.y].size()<gs.cursor_pos.x) gs.cursor_pos.x=gs.log[gs.cursor_pos.y].size();
		break;

		case (sf::Keyboard::Scancode::Left):
		if(gs.cursor_pos.x==0)
		{
			if(gs.cursor_pos.y==0) return;
			gs.cursor_pos.y--;
			gs.cursor_pos.x=gs.log[gs.cursor_pos.y].size();
		}
		else gs.cursor_pos.x--;
		break;

		/* oppure
		if (gs.cursor_pos.x>0 || gs.cursor_pos.y>0)
		{
			gs.cursor_pos.x--;
			if(gs.cursor_pos.x<0)
			{
				gs.cursor_pos.y--;
				x=gs.log[gs.cursor_pos.y].size();
			}
		}
		*/

		case (sf::Keyboard::Scancode::Right):
		if(gs.cursor_pos.x==gs.log[gs.cursor_pos.y].size())
		{
			if(gs.cursor_pos.y==gs.log.size()-1) return;
			gs.cursor_pos.y++;
			gs.cursor_pos.x=0;
		}
		else gs.cursor_pos.x++;
		break;
	}

	gs.adjustView();
}

void handle(const sf::Event::TextEntered &textEnter, State &gs)
{
	//unsigned last = gs.log.size() - 1;
	unsigned current_line=gs.cursor_pos.y;
	unsigned current_char=gs.cursor_pos.x;

	if (textEnter.unicode == '\n' || textEnter.unicode == '\r') // enter
	{
		std::string buffer=gs.log[current_line].substr((size_t)current_char);
		gs.log[current_line].erase((size_t)current_char);
		gs.log.insert(gs.log.begin()+current_line+1, buffer);
		//fix cursor
		gs.cursor_pos.x=0;
		gs.cursor_pos.y++;
	}

	else if (textEnter.unicode >= ' ' && textEnter.unicode <= '~') // printable char
	{
		char character=static_cast<char>(textEnter.unicode);
		gs.log[current_line].insert (current_char, 1, character);
		//fix cursor
		gs.cursor_pos.x++;
	}

	gs.adjustView();
}

void handle(const sf::Event::Resized &resized, State &gs)
{
	sf::FloatRect visibleArea({0.f, 0.f}, sf::Vector2f(resized.size));
	gs.window.setView(sf::View(visibleArea));
	
	gs.adjustView();
}

template <typename T>
void handle(const T &, State &gs){}	//other events

// AUX FUNCTIONS

sf::Vector2f Vector2iTo2f(sf::Vector2i vector)
{
	return {static_cast<float>(vector.x), static_cast<float>(vector.y)};
}

void doGraphics(State &gs)
{
	sf::Text logText{gs.font, "", FONT_SIZE};

	gs.window.clear();

	char info[50];
	sprintf(info, "DEBUG: cur_pos={%d, %d}, text_view=({%d, %d}, {%d, %d})", gs.cursor_pos.x, gs.cursor_pos.y, gs.text_view.position.x, gs.text_view.position.y, gs.text_view.size.x, gs.text_view.size.y);
	logText.setString(info);
	gs.window.draw(logText);
	logText.setString("");
	
	int first_line=gs.text_view.position.y;
	int lines_to_print=std::min(static_cast<unsigned>(gs.log.size()-first_line), static_cast<unsigned>(gs.text_view.size.y));
	for (std::size_t i=0; i<lines_to_print; ++i)
	{
		logText.setPosition(Vector2iTo2f({FONT_WIDTH, static_cast<int>(i*FONT_SIZE+(2*FONT_SIZE))}));	//2* per linea di debug
		if(gs.text_view.position.x<gs.log[i+first_line].size())
		{
			logText.setString(gs.log[i+first_line].substr((size_t)gs.text_view.position.x, (size_t)gs.text_view.size.x));
			gs.window.draw(logText);
		}
	}

	sf::Text cursor{gs.font, "_", FONT_SIZE};
	cursor.setPosition(Vector2iTo2f({(gs.cursor_pos.x-gs.text_view.position.x+1)*FONT_WIDTH, (gs.cursor_pos.y-gs.text_view.position.y+2)*FONT_SIZE}));
	cursor.setFillColor(sf::Color::Green);
	gs.window.draw(cursor);

	gs.window.display();
}

// MAIN

int main()
{
	State gs(800, 600, "Text editor");
	
	while (gs.window.isOpen()) // main loop
	{
		// event loop and handler through callbacks
		gs.window.handleEvents([&](const auto &event){handle(event, gs);});
		// Show log
		doGraphics(gs);
	}
}
