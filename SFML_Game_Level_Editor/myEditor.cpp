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
			/*
			edt::tText* text = new edt::tText(this, { 0, 0 }, L"SFML_Game �������� �������� ���������");
			text->setFont(getFont());
			text->setCharSize(66);
			text->setTextColor({ 255, 255, 255, 255 });
			text->setPosition({ window.getSize().x / 2 - text->getLocalBounds().width / 2, 50 });
			_insert(text);

			edt::tButton* button = new edt::tButton(this, { 0, 0, 600, 80 });
			button->setString(L"������������� �����");
			button->setCode(static_cast<int>(screen_codes::MapEditor));
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor({ 255, 255, 0, 255 });
			button->setTextAlignment(static_cast<int>(edt::tButton::text_alignment_type::Middle));
			button->setTextOffset({ 0, 12 });
			button->setAnchor(edt::tObject::anchors.center);
			button->setPosition({ -300, -40 + 320 - 340 });
			_insert(button);

			button = new edt::tButton(this, { 0, 0, 600, 80 });
			button->setString(L"������������� NPC");
			button->setCode(static_cast<int>(screen_codes::NPCEditor));
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor({ 255, 255, 0, 255 });
			button->setTextAlignment(static_cast<int>(edt::tButton::text_alignment_type::Middle));
			button->setTextOffset({ 0, 12 });
			button->setAnchor(edt::tObject::anchors.center);
			button->setPosition({ -300, -40 + 320 - 255 });
			_insert(button);

			button = new edt::tButton(this, { 0, 0, 600, 80 });
			button->setString(L"�������� ��������");
			button->setCode(static_cast<int>(screen_codes::SPREditor));
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor({ 255, 255, 0, 255 });
			button->setTextAlignment(static_cast<int>(edt::tButton::text_alignment_type::Middle));
			button->setTextOffset({ 0, 12 });
			button->setAnchor(edt::tObject::anchors.center);
			button->setPosition({ -300, -40 + 320 - 170 });
			_insert(button);

			button = new edt::tButton(this, { 0, 0, 500, 80 });
			button->setString(L"� ���������");
			button->setCode(code_about_paragraph);
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor({ 255, 255, 255, 255 });
			button->setTextAlignment(static_cast<int>(edt::tButton::text_alignment_type::Middle));
			button->setTextOffset({ 0, 12 });
			button->setAnchor(edt::tObject::anchors.center);
			button->setPosition({ -250, -40 + 320 - 85 });
			_insert(button);

			button = new edt::tButton(this, { 0, 0, 500, 80 });
			button->setString(L"�����");
			button->setCode(static_cast<int>(edt::tEvent::codes::CloseApplication));
			button->setFont(getFont());
			button->setCharSize(60);
			button->setTextColor({ 255, 0, 0, 255 });
			button->setTextAlignment(static_cast<int>(edt::tButton::text_alignment_type::Middle));
			button->setTextOffset({0, 8});
			button->setAnchor(edt::tObject::anchors.center);
			button->setPosition({ -250, -40 + 320 });
			_insert(button);
			*/

			makeObjectsFromJson(this, json_configuration["menu"]);

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
						
						edt::tWindow *w = new edt::tWindow(this, rect, L"� ���������");
						w->setFont(getFont());
						w->setCaptionOffset({4, 0});

						sf::Vector2f content_position = { 10, (float)w->getHeapHeight() };

						std::fstream file(path_to_folder + "\\Content\\Texts\\about.txt", std::fstream::in | std::fstream::binary);
						edt::tText* t;	// ����� ��� ������ � ����

						if (!file.is_open()) {
							font_size -= 2;
							std::wstring text = L"���� ''..\\Content\\Texts\\about.txt'' �� ������.\n\n������� �� ���...\n�� ������ ������ �������� �����������.";
							t = new edt::tText(w, { content_position.x, content_position.y }, text);
							t->setFont(getFont());
							t->setCharSize(font_size);
							w->_insert(t);
							content_position.y += std::max<float>(t->getLocalBounds().height, (float)font_size);
						}
						else {
							std::wstring str = L"";
							file.seekg(0, std::fstream::end);
							
							unsigned long long file_size = file.tellg();	// ������ �����
							file_size -= 2;

							file.clear(); file.seekg(0);
							file.seekg(2);									// ���������� BOM (FE FF)

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