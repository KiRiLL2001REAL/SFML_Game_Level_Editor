#include <iostream>
#include "other.h"
#include "myEditor.h"
#include "huffman.h"

int main(int argc, char* argv[]) {
	std::string path_to_folder = cutOffLast(argv[0], 2);
	
	/*
	myDesktop* desk = new myDesktop(path_to_folder);
	desk->loadCustomFont(path_to_folder + "\\Content\\Fonts\\CyrilicOld.ttf");
	desk->run();
	delete desk;
	*/

	huf::huffman_compression huf(path_to_folder);
	huf.compress("\\Content\\Texts\\test.txt");
	/*
	sf::Clock timer;
	for (int i = 0; i < 25; i++) {
		timer.restart();
		huf.compress("\\Content\\Texts\\test.txt");
		sf::Time elapsed = timer.getElapsedTime();
		std::cout << i+1 << " turn : Compressed in " << elapsed.asMicroseconds() << " mcs (" << elapsed.asMilliseconds() << " ms)\n";
	}
	
	char i;
	std::cin >> i;
	*/

	/*
	// Загрузка шрифта
	font.loadFromFile(path_to_folder + "\\Content\\Fonts\\MurreyC.ttf");
	text.setFont(font);
	text.setCharacterSize(48);

	// Создание прямоугольной зоны для отрисовки карты внутри неё
	render_squad[0].position = sf::Vector2f(10.f, 10.f);
	render_squad[1].position = sf::Vector2f(10.f + ((float)window.getSize().y - 20.f) * 1.25f, 10.f);
	render_squad[2].position = sf::Vector2f(10.f + ((float)window.getSize().y - 20.f) * 1.25f, (float)window.getSize().y - 10.f);
	render_squad[3].position = sf::Vector2f(10.f, (float)window.getSize().y - 10.f);

	render_squad[0].color = sf::Color::Red;
	render_squad[1].color = sf::Color::Green;
	render_squad[2].color = sf::Color::Blue;
	render_squad[3].color = sf::Color::Yellow;

	// Создание текстуры, на которую будет выполняться отрисовка тайлов
	canvas.create(1325, 1060);
	background.setFillColor(sf::Color(0, 0, 0));
	background.setSize(sf::Vector2f(canvas.getSize()));
	canvas.draw(background);

	// Определение ключевых точек на текстуре
	render_squad[0].texCoords = sf::Vector2f(0, 0);
	render_squad[1].texCoords = sf::Vector2f(-1.f + canvas.getSize().x, 0);
	render_squad[2].texCoords = sf::Vector2f(-1.f + canvas.getSize().x, -1.f + canvas.getSize().y);
	render_squad[3].texCoords = sf::Vector2f(0, -1.f + canvas.getSize().y);
	
	background.setFillColor(sf::Color(40, 40, 40));
	background.setSize(sf::Vector2f(window.getSize()));

	instrumental_panel_background.setFillColor(sf::Color(60, 60, 60));
	instrumental_panel_background.setPosition(sf::Vector2f(20.f + ((float)window.getSize().y - 20.f) * 1.25f, 10.f));
	instrumental_panel_background.setSize(sf::Vector2f((float)window.getSize().x - 30.f - ((float)window.getSize().y - 20.f) * 1.25f, (float)window.getSize().y - 20.f));

	sf::Event event;
	while (window.isOpen())
	{
		
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				window.close();
			}

			if (event.type == sf::Event::Resized) {
				window.setSize(setSizeIn16_9Ratio(sf::Vector2u(window.getSize().x, window.getSize().y)));
			}

		}

		window.clear();

		window.draw(background);
		window.draw(render_squad, &canvas.getTexture());
		window.draw(instrumental_panel_background);

		string = "кукусики";
		text.setString(string);

		text.setPosition(6, -9);
		text.setFillColor(sf::Color::Black);
		window.draw(text);
		text.setPosition(5, -10);
		text.setFillColor(sf::Color::White);
		window.draw(text);

		window.display();

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	*/
}