#include "stdafx.h"
#include "myEditor.h"

myDesktop::~myDesktop() {
}

void myDesktop::changeScreen(char new_screen_code) {
	edt::tDesktop::changeScreen(new_screen_code);

	switch (screen_code) {
		case button_codes.MapEditor : {
			//makeObjectsFromJson(this, json_configuration["mapEdit"]);
			break;
		}
		case button_codes.NpcEditor : {
			//makeObjectsFromJson(this, json_configuration["npcEdit"]);
			break;
		}
		case button_codes.SprEditor: {
			//makeObjectsFromJson(this, json_configuration["sprEdit"]);
			break;
		}
		case button_codes.Settings: {
			//makeObjectsFromJson(this, json_configuration["settings"]);
			break;
		}
		case button_codes.Menu :
		default: {
			edt::tText* text = new edt::tText(this, { 0, 0 }, L"SFML_Game редактор игрового окружения");
			text->setFont(getFont());
			text->setCharSize(66);
			text->setAnchor(edt::tObject::anchors.upper_side);
			text->setTextColor({ 255, 255, 255, 255 });
			text->setPosition({ (float)window.getSize().x / 2, 50 });
			_insert(text);

			edt::tButton* button = new edt::tButton(this, { 0, 0, 600, 80 });
			button->setString(L"Редактировать карту");
			button->setCode(button_codes.MapEditor);
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor({ 255, 255, 0, 255 });
			button->setTextOrigin(edt::tButton::text_origin_type.Middle);
			button->setTextOffset({ 0, 12 });
			button->setAnchor(edt::tObject::anchors.center);
			button->setPosition({ -300, -40 + 320 - 340 });
			_insert(button);

			button = new edt::tButton(this, { 0, 0, 600, 80 });
			button->setString(L"Редакторовать NPC");
			button->setCode(button_codes.NpcEditor);
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor({ 255, 255, 0, 255 });
			button->setTextOrigin(edt::tButton::text_origin_type.Middle);
			button->setTextOffset({ 0, 12 });
			button->setAnchor(edt::tObject::anchors.center);
			button->setPosition({ -300, -40 + 320 - 255 });
			_insert(button);

			button = new edt::tButton(this, { 0, 0, 600, 80 });
			button->setString(L"Редактор спрайтов");
			button->setCode(button_codes.SprEditor);
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor({ 255, 255, 0, 255 });
			button->setTextOrigin(edt::tButton::text_origin_type.Middle);
			button->setTextOffset({ 0, 12 });
			button->setAnchor(edt::tObject::anchors.center);
			button->setPosition({ -300, -40 + 320 - 170 });
			_insert(button);

			button = new edt::tButton(this, { 0, 0, 500, 80 });
			button->setString(L"О программе");
			button->setCode(button_codes.about_program);
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor({ 255, 255, 255, 255 });
			button->setTextOrigin(edt::tButton::text_origin_type.Middle);
			button->setTextOffset({ 0, 12 });
			button->setAnchor(edt::tObject::anchors.center);
			button->setPosition({ -250, -40 + 320 - 85 });
			_insert(button);

			button = new edt::tButton(this, { 0, 0, 500, 80 });
			button->setString(L"Выход");
			button->setCode(edt::tEvent::codes.CloseApplication);
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor({ 255, 0, 0, 255 });
			button->setTextOrigin(edt::tButton::text_origin_type.Middle);
			button->setTextOffset({0, 8});
			button->setAnchor(edt::tObject::anchors.center);
			button->setPosition({ -250, -40 + 320 });
			_insert(button);

			//makeObjectsFromJson(this, json_configuration["menu"]);

			break;
		}
	}
}

void myDesktop::handleEvent(edt::tEvent& e) {
	edt::tDesktop::handleEvent(e);
	if (e.address == this) {
		switch (e.type) {
			case edt::tEvent::types.Button: {
				switch (e.code) {
					case button_codes.about_program: {
						sf::FloatRect rect;
						unsigned int font_size = 32;
						rect.width = 630;
						rect.height = 410;
						rect.left = (window.getSize().x - rect.width) / 2;
						rect.top = (window.getSize().y - rect.height) / 2;

						edt::tWindow* w = new edt::tWindow(this, rect, L"О программе");
						w->setFont(getFont());
						w->setCaptionOffset({ 4, 0 });
					
						edt::tDisplay* display = w->getDisplayPointer();
						w->setDisplayTextureSize({ 1260, 600 });
					
						sf::Vector2f content_position = { 10, 0 };

						std::fstream file(path_to_folder + "\\Content\\Texts\\about.txt", std::fstream::in | std::fstream::binary);
						edt::tText* t = new edt::tText((edt::tRenderRect*)display, { content_position.x, content_position.y }, L"Some Text");
						t->setFont(getFont());
						t->setCharSize(font_size);

						if (!file.is_open()) {
							font_size -= 2;
							std::wstring text = L"Файл ''..\\Content\\Texts\\about.txt'' не найден.\n\nСтранно всё это...\nНа всякий случай вызовите экзорцистов.";
							t->setString(text);
						}
						else {
							std::wstring text = L"";
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
								text += wchar;
							}
							
							t->setString(text);
						}
						display->_insert(t);

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