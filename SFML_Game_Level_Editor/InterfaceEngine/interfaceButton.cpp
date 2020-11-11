#include "stdafx.h"
#include "interfaceEngine.h"

namespace edt {
	tButton::tButton(tAbstractBasicClass* _owner, sf::FloatRect rect) :
		tRenderRect(_owner, rect),
		self_code(tEvent::codes.Nothing),
		alignment(text_alignment_type.Left),
		side_offset(10),
		text_offset(sf::Vector2u(0, 0)),
		text(new tText(this))
	{
		setOneOption(option_mask.custom_skin_loaded, false);
		setOneOption(option_mask.text_can_be_showed, true);
		path_to_skin[0] = "";
		path_to_skin[1] = "";
	}

	tButton::tButton(tAbstractBasicClass* _owner, nlohmann::json& js) :
		tRenderRect(_owner, js),
		side_offset(10),
		text(new tText(this, js["text"]))
	{
		vec<int> vi = js["text_offset"].get<vec<int>>();
		text_offset = { vi[0], vi[1] };
		self_code = js["code"].get<int>();
		alignment = js["alignment"].get<char>();
		path_to_skin[0] = js["path_to_skin_0"].get<std::string>();
		path_to_skin[1] = js["path_to_skin_1"].get<std::string>();
		if (checkOption(option_mask.custom_skin_loaded)) {
			loadCustomSkin(path_to_skin[0], 0);
			loadCustomSkin(path_to_skin[1], 1);
		}
	}

	tButton::tButton(const tButton& b) :
		tRenderRect(b),
		side_offset(b.side_offset),
		self_code(b.self_code),
		alignment(b.alignment),
		text_offset(b.text_offset),
		text(b.text)
	{
		custom_skin[0] = b.custom_skin[0];
		custom_skin[1] = b.custom_skin[1];
	}

	tButton::~tButton() {
		delete text;
	}

	void tButton::handleEvent(tEvent& e) {
		if (checkOption(option_mask.can_be_drawn)) {
			text->handleEvent(e);
			switch (e.type) {
			case tEvent::types.Broadcast: {
				if (e.address == this) {	// Для конкретно этой кнопки
					switch (e.code) {
					case tEvent::codes.UpdateTexture: {	// Обновить текстуру
						message(getOwner(), tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
						need_rerender = true;
						clearEvent(e);
						break;
					}
					case tEvent::codes.StopAndDoNotMove: {	// Сбросить флаг перетаскивания мышью
						message(text, tEvent::types.Broadcast, tEvent::codes.StopAndDoNotMove, this);
						setOneOption(option_mask.is_moving_by_mouse, false);
						// Не обнуляем событие
						break;
					}
					}
				}
				switch (e.code) {			// Для всех остальных
				case tEvent::codes.ResetButtons: {
					if (e.from != this && e.from != getOwner()) {
						mouse_inside[0] = false;
						mouse_inside[1] = false;
						need_rerender = true;
					}
					break;
				}
				}
				break;
			}
			case tEvent::types.Mouse: {
				mouse_inside[1] = mouse_inside[0];
				mouse_inside[0] = pointIsInsideMe({ e.mouse.x, e.mouse.y });
				switch (e.code) {
				case tEvent::codes.MouseMoved: {
					if (mouse_inside[0] != mouse_inside[1]) {	// Если произошло изменение, то генерируем текстуру заново с подчёркнутым текстом
						message(nullptr, tEvent::types.Button, tEvent::codes.ResetButtons, this);
						message(this, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
						clearEvent(e);
					}
					break;
				}
				case tEvent::codes.MouseButton: {
					if (mouse_inside[0]) {
						if (e.mouse.button == sf::Mouse::Left) {
							if (e.mouse.what_happened == sf::Event::MouseButtonReleased) {	// Если левая кнопка мыши отпущена, и мышь находится внутри кнопки, то передаём послание
								message(getOwner(), tEvent::types.Button, self_code, this);
							}
						}
						clearEvent(e);
					}

					break;
				}
				}
				break;
			}
			}
		}
	}
	
	void tButton::updateTexture() {
		render_texture.clear(clear_color);
		if (checkOption(option_mask.custom_skin_loaded)) {	// Если загружен пользовательский скин кнопки, то выводим его
			sf::Sprite spr;
			int index = 0;
			if (mouse_inside[0]) {
				index = 1;
			}
			spr.setTexture(custom_skin[index]);
			spr.setScale({
				(float)render_texture.getSize().x / custom_skin[index].getSize().x,
				(float)render_texture.getSize().y / custom_skin[index].getSize().y
				});
			spr.setPosition({ 0.f, 0.f });
			render_texture.draw(spr);
		}
		else {						// Иначе - рисуем стандартную кнопку
			sf::Vector2f tex_size = (sf::Vector2f)render_texture.getSize();
			float offset = (float)side_offset / 2;
			sf::VertexArray arr(sf::Quads, 4);

			sf::Color front_color = sf::Color(150, 150, 150, 255);	// 0--------------------------------1
			sf::Color top_color = sf::Color(120, 120, 120, 255);	// |\                              /|
			sf::Color side_color = sf::Color(70, 70, 70, 255);		// | 4----------------------------5 |
			sf::Color bottom_color = sf::Color(20, 20, 20, 255);	// | |                            | |
																	// | |                            | |
			sf::Vector2f point[] = {								// | |                            | |
				{0, 0},												// | 7----------------------------6 |
				{tex_size.x, 0},									// |/                              \|
				{tex_size.x, tex_size.y},							// 3--------------------------------2
				{0, tex_size.y},
				{offset, offset},
				{tex_size.x - offset, offset},
				{tex_size.x - offset, tex_size.y - offset},
				{offset, tex_size.y - offset}
			};

			for (int i = 0; i < 4; i++) {					// Лицевая сторона
				arr[i].color = front_color;
				arr[i].position = point[4 + i];
			}
			render_texture.draw(arr);
			for (int i = 0; i < 4; i++)						// Верхняя сторона
				arr[i].color = top_color;
			arr[0].position = point[0];
			arr[1].position = point[1];
			arr[2].position = point[5];
			arr[3].position = point[4];
			render_texture.draw(arr);
			for (int i = 0; i < 4; i++)						// Боковые стороны
				arr[i].color = side_color;
			arr[0].position = point[0];
			arr[1].position = point[4];
			arr[2].position = point[7];
			arr[3].position = point[3];
			render_texture.draw(arr);
			arr[0].position = point[5];
			arr[1].position = point[1];
			arr[2].position = point[2];
			arr[3].position = point[6];
			render_texture.draw(arr);
			for (int i = 0; i < 4; i++)						// Нижжняя сторона
				arr[i].color = bottom_color;
			arr[0].position = point[7];
			arr[1].position = point[6];
			arr[2].position = point[2];
			arr[3].position = point[3];
			render_texture.draw(arr);
		}
		if (checkOption(option_mask.text_can_be_showed)) {
			if (text->getFontState()) {				// Если подгружен шрифт, то выводим текст
				sf::Text text_to_display = text->getTextObject();
				mouse_inside[0] ? text_to_display.setStyle(sf::Text::Style::Bold) : text_to_display.setStyle(sf::Text::Style::Regular);	// При наведении на кнопку мышью, текст подчёркивается
				switch (alignment) {			// Настройка выравнивания
				case text_alignment_type.Right: {
					text_to_display.setOrigin({
						(float)text_to_display.getLocalBounds().width,
						(float)text_to_display.getLocalBounds().height
						});
					text_to_display.setPosition({
						(float)render_texture.getSize().x - side_offset - text_offset.x,
						(float)render_texture.getSize().y / 2 + text_offset.y
						});
					break;
				}
				case text_alignment_type.Middle: {
					text_to_display.setOrigin({
						text_to_display.getLocalBounds().width / 2,
						(float)text_to_display.getLocalBounds().height
						});
					text_to_display.setPosition({
						(float)render_texture.getSize().x / 2 + text_offset.x,
						(float)render_texture.getSize().y / 2 + text_offset.y
						});
					break;
				}
				case text_alignment_type.Left:
				default: {
					text_to_display.setOrigin({
						0,
						(float)text_to_display.getLocalBounds().height
						});
					text_to_display.setPosition({
						(float)side_offset + text_offset.x,
						(float)render_texture.getSize().y / 2 + text_offset.x
						});
					break;
				}
				}
				text_to_display.setFillColor(sf::Color::Black);	// Немного контраста
				text_to_display.move({ 1, 1 });
				render_texture.draw(text_to_display);
				text_to_display.setFillColor(text->getFillColor());		// А это уже сам вывод текста
				text_to_display.move({ -1, -1 });
				render_texture.draw(text_to_display);
			}
			else {
				message(nullptr, tEvent::types.Broadcast, tEvent::codes.FontRequest, text);
			}
		}
		render_texture.display();
	}

	bool tButton::pointIsInsideMe(sf::Vector2i point) const {
		sf::FloatRect rect = getGlobalBounds();
		return (point.x >= rect.left && point.x <= rect.left + rect.width && point.y >= rect.top && point.y <= rect.top + rect.height);
	}

	void tButton::loadCustomSkin(const std::string& path_to_skin, const int& cell) {
		if (custom_skin[cell].loadFromFile(path_to_skin)) {
			this->path_to_skin[cell] = path_to_skin;
			setOneOption(option_mask.custom_skin_loaded, true);
			setTextureSize({ custom_skin[cell].getSize().x, custom_skin[cell].getSize().y });
			message(this, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
		}
		else {
			std::cout << "tButton.loadCustomSkin error: Invalid path_to_skin.\n";
		}
	}

	void tButton::setTextAlignment(const char& new_alignment) {
		switch (new_alignment) {
		case text_alignment_type.Middle:
		case text_alignment_type.Right: {
			alignment = new_alignment;
			break;
		}
		case text_alignment_type.Left:
		default: {
			alignment = text_alignment_type.Left;
			break;
		}
		}
		message(this, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
	}

	void tButton::setTextOffset(const sf::Vector2i& new_offset) {
		text_offset = new_offset;
		message(this, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
	}

	void tButton::setCode(const int& new_code) {
		self_code = new_code;
	}

	void tButton::setFont(const sf::Font& new_font) {
		text->setFont(new_font);
	}

	void tButton::setString(const std::wstring& new_string) {
		text->setString(new_string);
	}

	void tButton::setTextColor(const sf::Color& new_color) {
		text->setTextColor(new_color);
	}

	void tButton::setCharSize(const unsigned int& new_char_size) {
		text->setCharSize(new_char_size);
	}

	void tButton::setOutlineThickness(const unsigned char& new_thickness) {
		text->setOutlineThickness(new_thickness);
	}

	nlohmann::json tButton::getParamsInJson() const {
		nlohmann::json js = tRenderRect::getParamsInJson();

		js["what_is_it"] = objects_json_ids.tButton;
		js["what_is_it_string"] = "tButton";
		js["code"] = self_code;
		js["alignment"] = alignment;
		js["text_offset"] = { text_offset.x, text_offset.y };
		js["text"] = text->getParamsInJson();
		js["path_to_skin_0"] = path_to_skin[0];
		js["path_to_skin_1"] = path_to_skin[1];

		return js;
	}
}