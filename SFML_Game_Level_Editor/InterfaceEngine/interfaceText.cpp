#include "stdafx.h"
#include "interfaceEngine.h"

namespace edt
{
	tText::tText(tAbstractBasicClass* _owner, sf::Vector2f position, std::wstring string) :
		tObject(_owner),
		need_update_anchor(false)
	{
		tObject::setPosition(position);
		setOneOption(option_mask.is_font_loaded, false);
		setString(string);
		setTextColor({ 255, 255, 255, 255 });
		setCharSize(24);
		setOutlineThickness(1);
		setPosition(position);
	}

	tText::tText(tAbstractBasicClass* _owner, nlohmann::json& js) :
		tObject(_owner, js),
		need_update_anchor(true)
	{
		setOneOption(option_mask.is_font_loaded, false);
		vec<int> vi = js["text"].get<vec<int>>();
		setString(convertIntVectorToWstring(vi));

		vec<unsigned char> vuc = js["color"].get<vec<unsigned char>>();
		setTextColor({ vuc[0], vuc[1], vuc[2], vuc[3] });

		setCharSize(js["char_size"].get<unsigned int>());

		setOutlineThickness(js["outline_thickness"].get<unsigned int>());

		setPosition({ x, y });

		/*
		Ñåé÷àñ øðèôò åù¸ íå çàãðóæåí, çíà÷èò, íå èçâåñòíû ðàçìåðû òåêñòîâîãî îáúåêòà
		need_update_anchor = false
		ßêîðü îáíîâèòñÿ òîãäà, êîãäà áóäåò ïîëó÷åí øðèôò
		*/
	}

	tText::tText(const tText& t) :
		tObject(t),
		need_update_anchor(t.need_update_anchor),
		text_object(t.text_object),
		font(t.font)
	{
	}

	tText::~tText()
	{
	}

	void tText::draw(sf::RenderTarget& target)
	{
		if (checkOption(option_mask.can_be_drawn))
		{
			if (checkOption(option_mask.is_font_loaded))
			{	// Eñëè øðèôò çàãðóæåí, òî âûâîäèì ýëåìåíò íà ýêðàí
				target.draw(text_object);
				return;
			}
			// Â ïðîòèâíîì ñëó÷àå çàïðàøèâàåì åãî ó Desktop
			message(nullptr, tEvent::types.Broadcast, tEvent::codes.FontRequest, this);
			tEvent e;
			e.address = getOwner();
			e.from = this;
			e.type = tEvent::types.Broadcast;
			e.code = tEvent::codes.UpdateTexture;
			putEvent(e);
		}
	}

	void tText::handleEvent(tEvent& e)
	{
		if (checkOption(option_mask.can_be_drawn))
		{
			switch (e.type)
			{
			case tEvent::types.Broadcast:
			{
				if (e.address == this)
				{
					switch (e.code)
					{
					case tEvent::codes.FontAnswer:
					{
						setOneOption(option_mask.is_font_loaded, true);
						text_object.setFont(*e.font.font);
						if (need_update_anchor)
						{
							setAnchor(anchor);
						}
						message(getOwner(), tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
						clearEvent(e);
						break;
					}
					}
				}
				break;
			}
			case tEvent::types.Mouse:
			{
				mouse_inside[1] = mouse_inside[0];
				mouse_inside[0] = pointIsInsideMe({ e.mouse.x, e.mouse.y });
			}
			}
		}
	}

	void tText::move(sf::Vector2f delta)
	{
		tObject::move(delta);
		text_object.move(delta);
	}

	void tText::updateTexture()
	{
		return;
	}
	
	void tText::setStyle(const sf::Text::Style& new_style)
  {
		text_object.setStyle(new_style);
	}

	void tText::setAnchor(const unsigned char& new_anchor)
  {
		tObject::setAnchor(new_anchor);
		/*
		0b00001001;		// ßêîðü íà âåðõíèé ëåâûé óãîë ðîäèòåëÿ
		0b00001010;		// ßêîðü íà âåðõíþþ ñòîðîíó ðîäèòåëÿ
		0b00001100;		// ßêîðü íà âåðõíèé ïðàâûé óãîë	ðîäèòåëÿ
		0b00010001;		// ßêîðü íà ëåâóþ ñòîðîíó ðîäèòåëÿ
		0b00010010;		// ßêîðü íà öåíòð ðîäèòåëÿ
		0b00010100;		// ßêîðü íà ïðàâóþ ñòîðîíó ðîäèòåëÿ
		0b00100001;		// ßêîðü íà íèæíèé ëåâûé óãîë ðîäèòåëÿ
		0b00100010;		// ßêîðü íà íèæíþþ ñòîðîíó ðîäèòåëÿ
		0b00100100;		// ßêîðü íà íèæíèé ïðàâûé óãîë ðîäèòåëÿ
		*/
		sf::Vector2f origin = { 0.f, 0.f };
		sf::FloatRect text_bounds = text_object.getLocalBounds();
		switch (new_anchor & 0b111)
    {
			case 0b001:
      {	// Ëåâî
				origin.x = 0;
				break;
			}
			case 0b010:
      {	// Ñåðåäèíà
				origin.x = text_bounds.width / 2;
				break;
			}
			case 0b100:
      {	// Ïðàâî
				origin.x = text_bounds.width;
				break;
			}
		}
		switch ((new_anchor >> 3) & 0b111)
    {
			case 0b001:
      {	// Âåðõ
				origin.y = 0;
				break;
			}
			case 0b010:
      {	// Ñåðåäèíà
				origin.y = text_bounds.height / 2;
				break;
			}
			case 0b100:
      {	// Íèç
				origin.y = text_bounds.height;
				break;
			}
		}
		text_object.setOrigin(origin);
	}

	void tText::setString(const std::wstring& new_string)
  {
		text_object.setString(new_string);
	}

	void tText::setTextColor(const sf::Color& new_color)
	{
		text_object.setFillColor(new_color);
	}

	void tText::setFont(const sf::Font& new_font)
	{
		setOneOption(option_mask.is_font_loaded, true);
		font = new_font;
		text_object.setFont(font);
	}

	void tText::setCharSize(const unsigned int& new_char_size)
	{
		text_object.setCharacterSize(new_char_size);
	}

	void tText::setOutlineThickness(const unsigned char& new_thickness)
	{
		text_object.setOutlineThickness(new_thickness);
	}

	void tText::setPosition(const sf::Vector2f& new_position)
	{
		tObject::setPosition(new_position);
		text_object.setPosition(new_position);
	}

	std::wstring& tText::getString() const
	{
		return (std::wstring&)text_object.getString();
	}

	bool tText::getFontState() const
	{
		return checkOption(option_mask.is_font_loaded);
	}

	sf::Text& tText::getTextObject() const
	{
		return (sf::Text&)text_object;
	}

	sf::Color tText::getFillColor() const
	{
		return text_object.getFillColor();
	}

	sf::FloatRect tText::getLocalBounds() const
	{
		return text_object.getLocalBounds();
	}

	bool tText::pointIsInsideMe(sf::Vector2i point) const
	{
		sf::FloatRect rect = getGlobalBounds();
		return (
			point.x >= rect.left && 
			point.x <= rect.left + rect.width && 
			point.y >= rect.top && 
			point.y <= rect.top + rect.height
		);
	}

	nlohmann::json tText::getParamsInJson() const
	{
		nlohmann::json js = tObject::getParamsInJson();

		sf::Color color = text_object.getFillColor();
		std::wstring text = (std::wstring)text_object.getString();
		unsigned int char_size = text_object.getCharacterSize();
		unsigned int thickness = (unsigned int)text_object.getOutlineThickness();

		js["what_is_it"] = objects_json_ids.tText;
		js["what_is_it_string"] = "tText";
		js["color"] = { color.r, color.g, color.b, color.a };
		js["char_size"] = char_size;
		js["outline_thickness"] = thickness;
		js["text"] = text;

		return js;
	}
}