#include <iostream>
#include <SFML/Graphics.hpp>
#include "other.h"
#include "editor.h"

class tCloseProgramButton : public edt::tButton {
public:
	tCloseProgramButton(sf::FloatRect rect = { 0, 0, 128, 48 }, std::string text = "button") :
		edt::tButton(rect, text)
	{
	};

	virtual void handleEvent(edt::tEvent& e) {
		tButton::handleEvent(e);
		switch (e.type) {
		case edt::tEvent::types::Mouse:
			switch (e.code) {
			case edt::tEvent::codes::MouseButton:
				if (e.mouse.what_happened == sf::Event::MouseButtonReleased && e.mouse.button == sf::Mouse::Left &&
					pointIsInsideMe({ e.mouse.x, e.mouse.y }))
				{
					e.type = edt::tEvent::types::Broadcast;
					e.code = edt::tEvent::codes::CloseApplication;
					e.address = nullptr;
					putEvent(e);
					clearEvent(e);
				}
				break;
			}
			break;
		}
	};
};

class myDesktop : public edt::tDesktop {
public:
	myDesktop(std::string path_to_folder) : edt::tDesktop(path_to_folder) {};
	~myDesktop() { std::cout << "~myDesktop done.\n"; };

	virtual void changeScreen(char new_screen_code) {
		edt::tDesktop::changeScreen(new_screen_code);

		edt::tRenderRect* render = new edt::tRenderRect(sf::FloatRect(0.f, 0.f, (float)window.getSize().x, (float)window.getSize().y));

		switch (screen_code) {
		case tDesktop::screen_codes::Mapedt:

			break;
		case tDesktop::screen_codes::NPCedt:

			break;
		case tDesktop::screen_codes::Menu:
		default:
			edt::tRectShape* background = new edt::tRectShape;;
			background->setPosition(sf::Vector2f(0.f, 0.f));
			background->setSize(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
			background->setColor(sf::Color(40, 40, 40, 255));
			render->_insert(background);

			edt::tText* text = new edt::tText({0, 0}, "SFML_Game редактор карт");
			text->setFont(getFont());
			text->setCharSize(72);
			text->setTextColor(sf::Color(255, 255, 255, 255));
			text->setPosition(sf::Vector2f(window.getSize().x / 2 - text->getLocalBounds().width / 2, 50));
			render->_insert(text);

			edt::tButton* button = new edt::tButton({ 0, 0, 600, 80 }, "Редактировать карту");
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor(sf::Color(255, 255, 0, 255));
			button->setAlignment(edt::tButton::alignment_type::atMiddle);
			button->setTextOffset(sf::Vector2u(0, 12));
			button->setPosition(sf::Vector2f(window.getSize().x / 2 - button->getLocalBounds().width / 2, ((float)(window.getSize().y - 300) / 4) * 0 + 300));
			render->_insert(button);

			button = new edt::tButton({ 0, 0, 600, 80 }, "Редакторовать NPC");
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor(sf::Color(255, 255, 0, 255));
			button->setAlignment(edt::tButton::alignment_type::atMiddle);
			button->setTextOffset(sf::Vector2u(0, 12));
			button->setPosition(sf::Vector2f(window.getSize().x / 2 - button->getLocalBounds().width / 2, ((float)(window.getSize().y - 300) / 4) * 1 + 300));
			render->_insert(button);

			button = new edt::tButton({0, 0, 500, 80}, "О программе");
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor(sf::Color(255, 255, 255, 255));
			button->setAlignment(edt::tButton::alignment_type::atMiddle);
			button->setTextOffset(sf::Vector2u(0, 12));
			button->setPosition(sf::Vector2f(window.getSize().x / 2 - button->getLocalBounds().width / 2, ((float)(window.getSize().y - 300) / 4) * 2 + 300));
			render->_insert(button);

			tCloseProgramButton* clButton = new tCloseProgramButton({ 0, 0, 500, 80 }, "Выход");
			clButton->setFont(getFont());
			clButton->setCharSize(60);
			clButton->setTextColor(sf::Color(255, 0, 0, 255));
			clButton->setAlignment(edt::tButton::alignment_type::atMiddle);
			clButton->setTextOffset(sf::Vector2u(0, 8));
			clButton->setPosition(sf::Vector2f(window.getSize().x / 2 - button->getLocalBounds().width / 2, ((float)(window.getSize().y - 300) / 4) * 3 + 300));
			render->_insert(clButton);

			break;
		}

		_insert(render);
	};
};

int main(int argc, char* argv[]) {
	std::string path_to_folder = cutOffLast(argv[0], 2);

	myDesktop* desk = new myDesktop(path_to_folder);
	desk->loadCustomFont(path_to_folder + "\\Content\\Fonts\\CyrilicOld.ttf");
	desk->run();
	delete desk;

	//int i;
	//std::cin >> i;
	
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