#include "stdafx.h"
#include "interfaceEngine.h"

namespace edt
{
	tButton::tButton(tAbstractBasicClass* _owner, sf::FloatRect rect) :
		tRenderRect(_owner, rect),
		self_code(tEvent::codes.Nothing),
		text_origin(text_origin_type.Left),
		side_offset(10),
		text_offset({ 0.f, 0.f }),
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
		vec<float> vf = js["text_offset"].get<vec<float>>();
		text_offset = { vf[0], vf[1] };
		self_code = js["code"].get<int>();
		text_origin = js["text_origin"].get<char>();
		path_to_skin[0] = js["path_to_skin_0"].get<std::string>();
		path_to_skin[1] = js["path_to_skin_1"].get<std::string>();
		if (checkOption(option_mask.custom_skin_loaded)) {
			loadCustomSkin(path_to_skin[0], 0);
			loadCustomSkin(path_to_skin[1], 1);
		}

		setAnchor(anchor);
	}

	tButton::tButton(const tButton& b) :
		tRenderRect(b),
		side_offset(b.side_offset),
		self_code(b.self_code),
		text_origin(b.text_origin),
		text_offset(b.text_offset),
		text(b.text)
	{
		custom_skin[0] = b.custom_skin[0];
		custom_skin[1] = b.custom_skin[1];
	}

	tButton::~tButton()
	{
		delete text;
	}

	void tButton::handleEvent(tEvent& e)
	{
		if (checkOption(option_mask.can_be_drawn))
		{
			text->handleEvent(e);
			switch (e.type)
			{
			case tEvent::types.Broadcast:
			{
				if (e.address == this)
				{	// Событие для этой кнопки
					switch (e.code) {
					case tEvent::codes.UpdateTexture:
					{	// Обновить текстуру
						message(getOwner(), tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
						need_rerender = true;
						clearEvent(e);
						break;
					}
					}
				}
				switch (e.code)
				{	// Событие для всех элементов
				case tEvent::codes.ResetButtons:
				{
					if (e.from != this && e.from != getOwner())
					{
						mouse_inside[0] = false;
						mouse_inside[1] = false;
						need_rerender = true;
					}
					break;
				}
				}
				break;
			}
			case tEvent::types.Mouse:
			{
				mouse_inside[1] = mouse_inside[0];
				mouse_inside[0] = pointIsInsideMe({ e.mouse.x, e.mouse.y });
				switch (e.code)
				{
				case tEvent::codes.MouseMoved:
				{
					if (mouse_inside[0] != mouse_inside[1])
					{	// Если мышь наехала на кнопку, то она выделяется
						message(nullptr, tEvent::types.Button, tEvent::codes.ResetButtons, this);
						message(this, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
						clearEvent(e);
					}
					break;
				}
				case tEvent::codes.MouseButton:
				{
					if (mouse_inside[0])
					{
						if (e.mouse.button == sf::Mouse::Left)
						{
							if (e.mouse.what_happened == sf::Event::MouseButtonReleased)
							{	// Если отжали левую кнопку мыши на кнопке, то она отправляет свой код владельцу
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
	
	void tButton::updateTexture()
	{
		render_texture.clear(clear_color);
		if (checkOption(option_mask.custom_skin_loaded))
		{	// Если загружен пользовательский скин, то выводим его
			sf::Sprite spr;
			int index = 0;
			if (mouse_inside[0])
			{
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
		else
		{	// Рисование стандартного скина
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

			for (int i = 0; i < 4; i++)
			{	// Центр кнопки
				arr[i].color = front_color;
				arr[i].position = point[4 + i];
			}
			render_texture.draw(arr);
			for (int i = 0; i < 4; i++)
			{	// Верх кнопки
				arr[i].color = top_color;
			}
			arr[0].position = point[0];
			arr[1].position = point[1];
			arr[2].position = point[5];
			arr[3].position = point[4];
			render_texture.draw(arr);
			for (int i = 0; i < 4; i++)
			{	// Бока кнопки
				arr[i].color = side_color;
			}
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
			for (int i = 0; i < 4; i++)
			{	// Низ кнопки
				arr[i].color = bottom_color;
			}
			arr[0].position = point[7];
			arr[1].position = point[6];
			arr[2].position = point[2];
			arr[3].position = point[3];
			render_texture.draw(arr);
		}
		if (checkOption(option_mask.text_can_be_showed))
		{
			if (text->getFontState())
			{	// Если подгружен шрифт, то выводим его
				// Выделение кнопки выполняется посредством установки у текста жирного шрифта
				mouse_inside[0] ? text->setStyle(sf::Text::Style::Bold) : text->setStyle(sf::Text::Style::Regular);
				text->setAnchor(text->getAnchor());
				text->draw(render_texture);
			}
			else
			{
				message(nullptr, tEvent::types.Broadcast, tEvent::codes.FontRequest, text);
			}
		}
		render_texture.display();
	}

	const bool tButton::pointIsInsideMe(sf::Vector2i point) const
	{
		sf::FloatRect rect = getGlobalBounds();
		return (
			point.x >= rect.left && 
			point.x <= rect.left + rect.width && 
			point.y >= rect.top && 
			point.y <= rect.top + rect.height
		);
	}

	void tButton::loadCustomSkin(const std::string& path_to_skin, const int& index)
	{
		if (custom_skin[index].loadFromFile(path_to_skin))
		{
			this->path_to_skin[index] = path_to_skin;
			setOneOption(option_mask.custom_skin_loaded, true);
			setTextureSize({ custom_skin[index].getSize().x, custom_skin[index].getSize().y });
			message(this, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
		}
		else
		{
			std::cout << "tButton.loadCustomSkin error: Invalid path_to_skin.\n";
		}
	}

	void tButton::setTextOrigin(const char& new_origin)
	{
		switch (new_origin)
		{
		case text_origin_type.Right:
		{
			text_origin = new_origin;
			text->setAnchor(anchors.upper_right_corner);
			text->setPosition({ getLocalBounds().width - side_offset, 0 });
			break;
		}
		case text_origin_type.Middle:
		{
			text_origin = new_origin;
			text->setAnchor(anchors.upper_side);
			text->setPosition({ getLocalBounds().width / 2, 0 });
			break;
		}
		case text_origin_type.Left:
		default:
		{
			text_origin = new_origin;
			text->setAnchor(anchors.upper_left_corner);
			text->setPosition({ (float)side_offset, 0 });
			break;
		}
		}
		text->move(text_offset);
		message(this, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
	}


	void tButton::setTextOffset(const sf::Vector2f& new_offset)
	{
		text_offset = new_offset;
		message(this, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
	}

	void tButton::setCode(const int& new_code)
	{
		self_code = new_code;
	}

	void tButton::setFont(const sf::Font& new_font)
	{
		text->setFont(new_font);
	}

	void tButton::setString(const std::wstring& new_string)
	{
		text->setString(new_string);
	}

	void tButton::setTextColor(const sf::Color& new_color)
	{
		text->setTextColor(new_color);
	}

	void tButton::setCharSize(const unsigned int& new_char_size)
	{
		text->setCharSize(new_char_size);
	}

	void tButton::setOutlineThickness(const unsigned char& new_thickness)
	{
		text->setOutlineThickness(new_thickness);
	}

	nlohmann::json tButton::getParamsInJson() const
	{
		nlohmann::json js = tRenderRect::getParamsInJson();

		js["what_is_it"] = objects_json_ids.tButton;
		js["what_is_it_string"] = "tButton";
		js["code"] = self_code;
		js["text_origin"] = text_origin;
		js["text_offset"] = { text_offset.x, text_offset.y };
		js["text"] = text->getParamsInJson();
		js["path_to_skin_0"] = path_to_skin[0];
		js["path_to_skin_1"] = path_to_skin[1];

		return js;
	}
}