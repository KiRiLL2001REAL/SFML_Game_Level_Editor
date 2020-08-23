#include "stdafx.h"
#include "myEditor.h"

myDesktop::~myDesktop() {
}

void myDesktop::changeScreen(char new_screen_code) {
	edt::tDesktop::changeScreen(new_screen_code);

	switch (screen_code) {
		case static_cast<int>(screen_codes::MapEditor) : {

			break;
		}
		case static_cast<int>(screen_codes::NPCEditor) : {

			break;
		}
		case static_cast<int>(screen_codes::SPREditor) : {

			break;
		}
		case static_cast<int>(screen_codes::Menu) :
		default: {
			edt::tRectShape* background = new edt::tRectShape(this);
			background->setPosition(sf::Vector2f(0.f, 0.f));
			background->setSize(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
			background->setColor(sf::Color(40, 40, 40, 255));
			_insert(background);

			edt::tText* text = new edt::tText(this, { 0, 0 }, L"SFML_Game редактор игрового окружения");
			text->setFont(getFont());
			text->setCharSize(72);
			text->setTextColor({ 255, 255, 255, 255 });
			text->setPosition({ window.getSize().x / 2 - text->getLocalBounds().width / 2, 50 });
			_insert(text);

			unsigned int upper_offset = 300;
			unsigned char count_buttons = 5;

			edt::tButton* button = new edt::tButton(this, { 0, 0, 600, 80 });
			button->setString(L"Редактировать карту");
			button->setCode(static_cast<int>(screen_codes::MapEditor));
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor({ 255, 255, 0, 255 });
			button->setAlignment(static_cast<int>(edt::tButton::alignment_type::Middle));
			button->setTextOffset({ 0, 12 });
			button->setPosition({ window.getSize().x / 2 - button->getLocalBounds().width / 2, ((float)(window.getSize().y - upper_offset) / count_buttons) * 0 + upper_offset });
			_insert(button);

			button = new edt::tButton(this, { 0, 0, 600, 80 });
			button->setString(L"Редакторовать NPC");
			button->setCode(static_cast<int>(screen_codes::NPCEditor));
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor({ 255, 255, 0, 255 });
			button->setAlignment(static_cast<int>(edt::tButton::alignment_type::Middle));
			button->setTextOffset({ 0, 12 });
			button->setPosition({ window.getSize().x / 2 - button->getLocalBounds().width / 2, ((float)(window.getSize().y - upper_offset) / count_buttons) * 1 + upper_offset });
			_insert(button);

			button = new edt::tButton(this, { 0, 0, 600, 80 });
			button->setString(L"Редактор спрайтов");
			button->setCode(static_cast<int>(screen_codes::SPREditor));
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor({ 255, 255, 0, 255 });
			button->setAlignment(static_cast<int>(edt::tButton::alignment_type::Middle));
			button->setTextOffset({ 0, 12 });
			button->setPosition({ window.getSize().x / 2 - button->getLocalBounds().width / 2, ((float)(window.getSize().y - upper_offset) / count_buttons) * 2 + upper_offset });
			_insert(button);

			button = new edt::tButton(this, { 0, 0, 500, 80 });
			button->setString(L"О программе");
			button->setCode(code_about_paragraph);
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor({ 255, 255, 255, 255 });
			button->setAlignment(static_cast<int>(edt::tButton::alignment_type::Middle));
			button->setTextOffset({ 0, 12 });
			button->setPosition({ window.getSize().x / 2 - button->getLocalBounds().width / 2, ((float)(window.getSize().y - upper_offset) / count_buttons) * 3 + upper_offset });
			_insert(button);

			button = new edt::tButton(this, { 0, 0, 500, 80 });
			button->setString(L"Выход");
			button->setCode(static_cast<int>(edt::tEvent::codes::CloseApplication));
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor({ 255, 0, 0, 255 });
			button->setAlignment(static_cast<int>(edt::tButton::alignment_type::Middle));
			button->setTextOffset({0, 8});
			button->setPosition({ window.getSize().x / 2 - button->getLocalBounds().width / 2, ((float)(window.getSize().y - upper_offset) / count_buttons) * 4 + upper_offset });
			_insert(button);

			break;
		}
	}
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
						rect.width = 620;
						rect.height = 410;
						rect.left = (window.getSize().x - rect.width) / 2;
						rect.top = (window.getSize().y - rect.height) / 2;
						
						edt::tWindow *w = new edt::tWindow(this, rect, L"О программе");
						w->setFont(getFont());
						w->setCaptionOffset({4, 0});

						sf::Vector2f content_position = { 10, (float)w->getHeapHeight() };

						std::fstream file(path_to_folder + "\\Content\\Texts\\about.txt", std::fstream::in | std::fstream::binary);
						edt::tText* t;	// Текст для вывода в окно

						if (!file.is_open()) {
							font_size -= 2;
							std::wstring text = L"Файл ''..\\Content\\Texts\\about.txt'' не найден.\n\nСтранно всё это...\nНа всякий случай вызовите экзорцистов.";
							t = new edt::tText(w, { content_position.x, content_position.y }, text);
							t->setFont(getFont());
							t->setCharSize(font_size);
							w->_insert(t);
							content_position.y += std::max<float>(t->getLocalBounds().height, (float)font_size);
						}
						else {
							std::wstring str = L"";
							file.seekg(0, std::fstream::end);
							
							unsigned long long file_size = file.tellg();	// Размер файла
							file_size -= 2;

							file.clear(); file.seekg(0);
							file.seekg(2);									// Пропускаем BOM (FE FF)

							unsigned int high, low;
							unsigned char c;
							wchar_t wchar;
							for (unsigned long long i = 0; i < file_size / 2; i++) {
								file.read((char*)&c, sizeof(c));
								high = c & 0xFF;
								file.read((char*)&c, sizeof(c));
								low = c & 0xFF;

								wchar = high << 8 | low;
								str += wchar;
							}
							
							t = new edt::tText(w, { content_position.x, content_position.y }, str);
							t->setFont(getFont());
							t->setCharSize(font_size);
							w->_insert(t);
						}

						file.close();
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

bool myDesktop::pointIsInsideMe(sf::Vector2i point) {
	return false;
}
