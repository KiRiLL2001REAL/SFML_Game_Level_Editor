#include "myEditor.h"
#include <fstream>
#include <string>

void myDesktop::changeScreen(char new_screen_code) {
	edt::tDesktop::changeScreen(new_screen_code);

	edt::tRenderRect *render = new edt::tRenderRect(sf::FloatRect(0.f, 0.f, (float)window.getSize().x, (float)window.getSize().y));

	switch (screen_code) {
		case static_cast<int>(screen_codes::MapEditor) : {

			break;
		}
		case static_cast<int>(screen_codes::NPCEditor) : {

			break;
		}
		case static_cast<int>(screen_codes::Menu) :
		default: {
			edt::tRectShape* background = new edt::tRectShape;;
			background->setPosition(sf::Vector2f(0.f, 0.f));
			background->setSize(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
			background->setColor(sf::Color(40, 40, 40, 255));
			render->_insert(background);

			edt::tText* text = new edt::tText({ 0, 0 }, "SFML_Game редактор игрового окружения");
			text->setFont(getFont());
			text->setCharSize(72);
			text->setTextColor({ 255, 255, 255, 255 });
			text->setPosition({ window.getSize().x / 2 - text->getLocalBounds().width / 2, 50 });
			render->_insert(text);

			edt::tButton* button = new edt::tButton({ 0, 0, 600, 80 }, "Редактировать карту");
			button->setCode(static_cast<int>(screen_codes::MapEditor));
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor({ 255, 255, 0, 255 });
			button->setAlignment(static_cast<int>(edt::tButton::alignment_type::Middle));
			button->setTextOffset({ 0, 12 });
			button->setPosition({ window.getSize().x / 2 - button->getLocalBounds().width / 2, ((float)(window.getSize().y - 300) / 4) * 0 + 300 });
			render->_insert(button);

			button = new edt::tButton({ 0, 0, 600, 80 }, "Редакторовать NPC");
			button->setCode(static_cast<int>(screen_codes::NPCEditor));
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor({ 255, 255, 0, 255 });
			button->setAlignment(static_cast<int>(edt::tButton::alignment_type::Middle));
			button->setTextOffset({ 0, 12 });
			button->setPosition({ window.getSize().x / 2 - button->getLocalBounds().width / 2, ((float)(window.getSize().y - 300) / 4) * 1 + 300 });
			render->_insert(button);

			button = new edt::tButton({ 0, 0, 500, 80 }, "О программе");
			button->setCode(code_about_paragraph);
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor({ 255, 255, 255, 255 });
			button->setAlignment(static_cast<int>(edt::tButton::alignment_type::Middle));
			button->setTextOffset({ 0, 12 });
			button->setPosition({ window.getSize().x / 2 - button->getLocalBounds().width / 2, ((float)(window.getSize().y - 300) / 4) * 2 + 300 });
			render->_insert(button);

			button = new edt::tButton({ 0, 0, 500, 80 }, "Выход");
			button->setCode(static_cast<int>(edt::tEvent::codes::CloseApplication));
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor({ 255, 0, 0, 255 });
			button->setAlignment(static_cast<int>(edt::tButton::alignment_type::Middle));
			button->setTextOffset({0, 8});
			button->setPosition({ window.getSize().x / 2 - button->getLocalBounds().width / 2, ((float)(window.getSize().y - 300) / 4) * 3 + 300 });
			render->_insert(button);

			break;
		}
	}
	_insert(render);
}

void myDesktop::handleEvent(edt::tEvent& e) {
	edt::tDesktop::handleEvent(e);
	if (e.address == this) {
		switch (e.type) {
			case static_cast<int>(edt::tEvent::types::Button) : {
				switch (e.code) {
					case code_about_paragraph : {
						sf::FloatRect rect;
						unsigned int font_size = 32;
						rect.width = 640;
						rect.height = 340;
						rect.left = (window.getSize().x - rect.width) / 2;
						rect.top = (window.getSize().y - rect.height) / 2;
						edt::tWindow *w = new edt::tWindow(rect, "О программе");
						w->setFont(getFont());
						w->initWindow();
						w->setCaptionOffset({4, 0});

						sf::Vector2f content_position = { 10, (float)w->getHeapHeight() };

						std::ifstream file(path_to_folder + "\\Content\\Texts\\about.txt");
						edt::tText* t;	// Текст для вывода в окно

						if (!file.is_open()) {
							font_size -= 2;
							std::string texts[] = {
								"Файл ''..\\Content\\Texts\\about.txt'' не найден.",
								"\nСтранно всё это...",
								"\nНа всякий случай вызовите экзорцистов."
							};
							for (int i = 0; i < 3; i++) {
								t = new edt::tText({ content_position.x, content_position.y }, texts[i]);
								t->setFont(getFont());
								t->setCharSize(font_size);
								w->_insert(t);
								content_position.y += std::max<float>(t->getLocalBounds().height, (float)font_size);
							}
						}
						else {
							std::string str;
							while (!file.eof()) {
								std::getline(file, str);
								t = new edt::tText({ content_position.x, content_position.y }, str);
								t->setFont(getFont());
								t->setCharSize(font_size);
								w->_insert(t);
								content_position.y += std::max<float>(t->getLocalBounds().height, (float)font_size);
							}
						}

						_insert(w);
						clearEvent(e);
						break;
					}
				}
				break;
			}
		}
	}
}

sf::FloatRect myDesktop::getLocalBounds() {
	return { 0, 0, 0, 0 };
}
