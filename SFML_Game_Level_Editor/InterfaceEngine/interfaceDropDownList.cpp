#include "stdafx.h"
#include "interfaceEngine.h"

namespace edt
{
	tDropDownVariant::tDropDownVariant(tAbstractBasicClass* _owner, std::wstring text, sf::FloatRect rect) :
		tButton(_owner, rect)
	{
		setOneOption(option_mask.text_can_be_showed, true);
		setTextOrigin(text_origin_type.Left);
		setString(text);
	}

	tDropDownVariant::tDropDownVariant(tAbstractBasicClass* _owner, nlohmann::json& js) :
		tButton(_owner, js)
	{
	}

	tDropDownVariant::tDropDownVariant(const tDropDownVariant& d) :
		tButton(d)
	{
	}

	tDropDownVariant::~tDropDownVariant()
	{
	}

	void tDropDownVariant::updateTexture()
	{
		render_texture.clear(clear_color);
		
		// Рамка вокруг кнопки
		sf::Vector2f tex_size = (sf::Vector2f)render_texture.getSize();
		sf::VertexArray arr(sf::PrimitiveType::LinesStrip, 5);
		sf::Color color = { 140, 140, 140, 255 };
		arr[0].position = { 1, 0 };
		arr[1].position = { tex_size.x - 1, 0 };
		arr[2].position = { tex_size.x - 1, tex_size.y - 2 };
		arr[3].position = { 1, tex_size.y - 2 };
		arr[4].position = { 1, 1 };
		if (checkOption(tObject::option_mask.is_active))
			color = { 0, 122, 204, 255 };
		for (int i = 0; i < 5; i++)
			arr[i].color = color;
		render_texture.draw(arr);

		if (checkOption(option_mask.text_can_be_showed)) {
			if (text->getFontState())
			{	// Åñëè ïîäãðóæåí øðèôò, òî âûâîäèì òåêñò
				mouse_inside[0] ? text->setStyle(sf::Text::Style::Bold) : text->setStyle(sf::Text::Style::Regular);	// Ïðè íàâåäåíèè íà êíîïêó ìûøüþ, òåêñò âûäåëÿåòñÿ
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

	nlohmann::json tDropDownVariant::getParamsInJson() const
	{
		nlohmann::json js = tButton::getParamsInJson();

		js["what_is_it"] = objects_json_ids.tDropDownVariant;
		js["what_is_it_string"] = "tDropDownVariant";

		return js;
	}

	tDropDownList::tDropDownList(tAbstractBasicClass* _owner, sf::FloatRect rect, unsigned int _direction) :
		tDropDownVariant(_owner, L"-", rect),
		direction(direction_types.Down),
		selected_variant_code(-1),
		ddwindow(new tDropDownWindow(this, {0, rect.height, rect.width, 200}))
	{
	}

	tDropDownList::tDropDownList(tAbstractBasicClass* _owner, nlohmann::json& js) :
		tDropDownVariant(_owner, js),
		ddwindow(new tDropDownWindow(this, js["drop_down_window"]))
	{
		direction = js["direction"].get<int>();
		selected_variant_code = js["selected_variant_code"].get<int>();
	}

	tDropDownList::tDropDownList(const tDropDownList& d) :
		tDropDownVariant(d),
		direction(d.direction),
		selected_variant_code(d.selected_variant_code),
		ddwindow(d.ddwindow)
	{
	}
	
	tDropDownList::~tDropDownList()
	{
		delete ddwindow;
	}

	void tDropDownList::handleEvent(tEvent& e)
	{
	}

	void tDropDownList::updateTexture()
	{
		render_texture.clear(clear_color);

		render_texture.display();
	}

	void tDropDownList::insertVariant(const std::wstring& variant_text, const int& variant_code)
	{
	}

	bool tDropDownList::deleteVariant(const std::wstring& variant_text)
	{
		return false;
	}

	bool tDropDownList::deleteVariant(const int& variant_code)
	{
		return false;
	}

	void tDropDownList::setDirection(const unsigned int& new_direction)
	{
	}

	unsigned int tDropDownList::getSelectedVariantCode() const
	{
		return selected_variant_code;
	}

	nlohmann::json tDropDownList::getParamsInJson() const
	{
		nlohmann::json js = tButton::getParamsInJson();

		js["what_is_it"] = objects_json_ids.tDropDownList;
		js["what_is_it_string"] = "tDropDownList";

		js["direction"] = direction;
		js["selected_variant_code"] = selected_variant_code;
		js["drop_down_window"] = ddwindow->getParamsInJson();

		return js;
	}

	void tDropDownList::setSelectedVariantCode(const int& new_selected_variant_code)
	{
		selected_variant_code = new_selected_variant_code;
	}

	tDropDownList::tDropDownWindow::tDropDownWindow(tAbstractBasicClass* _owner, sf::FloatRect rect) :
		tDisplay(_owner, rect),
		scrollbar_v(new tScrollbar((tRenderRect*)this, true, {0, 0, tScrollbar::thickness, getLocalBounds().height }))
	{
		scrollbar_v->setAnchor(anchors.upper_right_corner);
		scrollbar_v->setPosition({ -tScrollbar::thickness - 1, 0 });
		scrollbar_v->setTargetSize({ getLocalBounds().width, getLocalBounds().height });
		scrollbar_v->setTargetTextureSize(getTextureSize());
	}
	
	tDropDownList::tDropDownWindow::tDropDownWindow(tAbstractBasicClass* _owner, nlohmann::json& js) :
		tDisplay(_owner, js),
		scrollbar_v(new tScrollbar((tRenderRect*)this, js["scrollbar_v"]))
	{
	}
	
	tDropDownList::tDropDownWindow::tDropDownWindow(const tDropDownWindow& d) :
		tDisplay(d),
		scrollbar_v(d.scrollbar_v)
	{
	}
	
	tDropDownList::tDropDownWindow::~tDropDownWindow()
	{
		delete scrollbar_v;
	}
	
	void tDropDownList::tDropDownWindow::handleEvent(tEvent& e)
	{
	}

	nlohmann::json tDropDownList::tDropDownWindow::getParamsInJson() const
	{
		nlohmann::json js = tDisplay::getParamsInJson();

		js["what_is_it"] = objects_json_ids.tDropDownWindow;
		js["what_is_it_string"] = "tDropDownWindow";

		js["scrollbar_v"] = scrollbar_v->getParamsInJson();

		return js;
	}
}