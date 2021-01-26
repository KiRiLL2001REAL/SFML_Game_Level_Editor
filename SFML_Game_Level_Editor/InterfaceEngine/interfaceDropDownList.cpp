#include "stdafx.h"
#include "interfaceEngine.h"

namespace edt
{
	tDropDownVariant::tDropDownVariant(tAbstractBasicClass* _owner, std::wstring text, sf::FloatRect rect) :
		tButton(_owner, rect)
	{
		setString(text);
		setTextOrigin(text_origin_type.Left);
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
		/*
		Просто текст обведённый рамкой =)
		*/
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
		if (mouse_inside[0])
		{	// Если мышь внутри, то выделяем рамку
			color = { 0, 122, 204, 255 };
		}
		for (int i = 0; i < 5; i++)
		{
			arr[i].color = color;
		}
		render_texture.draw(arr);

		if (checkOption(option_mask.text_can_be_showed)) {
			if (text->getFontState())
			{	// Если текст подгружен, то выводим его
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
		direction(_direction),
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
		sf::Vector2f tex_size = (sf::Vector2f)render_texture.getSize();

		// Рисование кнопки
		sf::VertexArray a(sf::PrimitiveType::Quads, 4);
		sf::Color color = { 150, 150, 150, 255 };
		a[0].position = { 1, 0 };
		a[1].position = { tex_size.x , 0 };
		a[2].position = { tex_size.x , tex_size.y - 1 };
		a[3].position = { 1, tex_size.y - 1 };
		for (int i = 0; i < 4; i++)
		{
			a[i].color = color;
		}
		render_texture.draw(a);

		// Рисование стрелки
		sf::ConvexShape b;
		b.setPointCount(3);
		b.setOutlineThickness(1.f);
		b.setOutlineColor({ 0, 0, 0, 255 });
		b.setFillColor({ 200, 200, 200, 255});
		
		static const float pi = 3.1415926535f;
		float radius = MAX(tex_size.y * 0.5f - 5.f, 5.f);
		sf::Vector2f triangle_offset = { tex_size.x - tex_size.y * 0.5f - 5, tex_size.y * 0.45f  };
		static const float koef = pi / 180.f;
		sf::Vector3f triangle_point_angle = {	// Положение - вверх
			30.f * koef,
			150.f * koef,
			-90.f * koef
		};
		if (!direction)
		{	// Если список развёртывается вниз, то поворачиваем треугольник и немного смещаем, т.к. рисовка была произведена не совсем по середине высоты
			triangle_point_angle += {pi, pi, pi};
			triangle_offset += { 0.f, tex_size.y * 0.1f };
		}
		sf::Vector3f triangle_point_distance = {
			radius,
			radius,
			radius * 0.365f
		};
		b.setPoint(0, { triangle_offset.x + triangle_point_distance.x * cos(triangle_point_angle.x), triangle_offset.y + triangle_point_distance.x * sin(triangle_point_angle.x) });
		b.setPoint(1, { triangle_offset.x + triangle_point_distance.y * cos(triangle_point_angle.y), triangle_offset.y + triangle_point_distance.y * sin(triangle_point_angle.y) });
		b.setPoint(2, { triangle_offset.x + triangle_point_distance.z * cos(triangle_point_angle.z), triangle_offset.y + triangle_point_distance.z * sin(triangle_point_angle.z) });
		render_texture.draw(b);

		// Рамка вокруг кнопки
		sf::VertexArray с(sf::PrimitiveType::LinesStrip, 5);
		color = { 140, 140, 140, 255 };
		с[0].position = { 1, 0 };
		с[1].position = { tex_size.x, 0 };
		с[2].position = { tex_size.x, tex_size.y - 1 };
		с[3].position = { 1, tex_size.y - 1 };
		с[4].position = { 1, 0 };
		for (int i = 0; i < 5; i++)
		{
			с[i].color = color;
		}
		render_texture.draw(с);

		if (text->getFontState())
		{	// Если текст подгружен, то выводим его
			mouse_inside[0] ? text->setStyle(sf::Text::Style::Bold) : text->setStyle(sf::Text::Style::Regular);	// Ïðè íàâåäåíèè íà êíîïêó ìûøüþ, òåêñò âûäåëÿåòñÿ
			if (selected_variant_code != -1)
			{

			}
			else
			{
				text->setString(L"Не выбрано");
			}
			text->setAnchor(text->getAnchor());
			text->draw(render_texture);
		}
		else
		{
			message(nullptr, tEvent::types.Broadcast, tEvent::codes.FontRequest, text);
		}
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

	tDropDownWindow::tDropDownWindow(tAbstractBasicClass* _owner, sf::FloatRect rect) :
		tDisplay(_owner, rect),
		scrollbar_v(new tScrollbar((tRenderRect*)this, true, {0, 0, tScrollbar::thickness, getLocalBounds().height }))
	{
		scrollbar_v->setAnchor(anchors.upper_right_corner);
		scrollbar_v->setPosition({ -tScrollbar::thickness - 1, 0 });
		scrollbar_v->setTargetSize({ getLocalBounds().width, getLocalBounds().height });
		scrollbar_v->setTargetTextureSize(getTextureSize());
	}
	
	tDropDownWindow::tDropDownWindow(tAbstractBasicClass* _owner, nlohmann::json& js) :
		tDisplay(_owner, js),
		scrollbar_v(new tScrollbar((tRenderRect*)this, js["scrollbar_v"]))
	{
	}
	
	tDropDownWindow::tDropDownWindow(const tDropDownWindow& d) :
		tDisplay(d),
		scrollbar_v(d.scrollbar_v)
	{
	}
	
	tDropDownWindow::~tDropDownWindow()
	{
		delete scrollbar_v;
	}
	
	void tDropDownWindow::handleEvent(tEvent& e)
	{
	}

	nlohmann::json tDropDownWindow::getParamsInJson() const
	{
		nlohmann::json js = tDisplay::getParamsInJson();

		js["what_is_it"] = objects_json_ids.tDropDownWindow;
		js["what_is_it_string"] = "tDropDownWindow";

		js["scrollbar_v"] = scrollbar_v->getParamsInJson();

		return js;
	}
}