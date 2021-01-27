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

		if (text->getFontState())
		{	// Если текст подгружен, то выводим его
			text->setAnchor(text->getAnchor());
			text->draw(render_texture);
		}
		else
		{
			message(nullptr, tEvent::types.Broadcast, tEvent::codes.FontRequest, text);
		}
		render_texture.display();
	}

	const int& tDropDownVariant::getCode() const
	{
		return self_code;
	}

	const std::wstring& tDropDownVariant::getString() const
	{
		return text->getString();
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
		selected_variant_code(-1)
	{
		if (!_direction)
		{	// Раскрытие вниз
			//ddwindow = new tDropDownWindow(this, { rect.left, rect.top + rect.height, rect.width, 200 });
			ddwindow = new tDropDownWindow(this, { 0, rect.height, rect.width, 200 });
		}
		else
		{	// Раскрытие вверх
			//ddwindow = new tDropDownWindow(this, { rect.left, rect.top - 200, rect.width, 200 });
			ddwindow = new tDropDownWindow(this, { 0, -200, rect.width, 200 });
		}
		ddwindow->setClearColor({ 80, 80, 80, 255 });
		ddwindow->setOneOption(tDropDownWindow::option_mask.can_be_drawn, false);
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
		if (checkOption(option_mask.can_be_drawn))
		{
			ddwindow->handleEvent(e);
			text->handleEvent(e);
			switch (e.type)
			{
			case tEvent::types.Broadcast:
			{
				if (e.address == this)
				{	// Для конкретно этого объекта
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
				{	// Для любого объекта
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
					{	// Если навели мышь, то корректируем текстуру
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
							{	// Если нажали на кнопку, то открываем окно выбора
								ddwindow->setOneOption(tDropDownWindow::option_mask.can_be_drawn, !ddwindow->checkOption(tDropDownWindow::option_mask.can_be_drawn));
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
		b.setOutlineThickness(1.5f);
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
		if (mouse_inside[0])
		{
			triangle_point_distance.x *= 1.2f;
			triangle_point_distance.y *= 1.2f;
			triangle_point_distance.z *= 1.2f;
		}
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

	void tDropDownList::draw(sf::RenderTarget& target)
	{
		if (checkOption(option_mask.can_be_drawn))
		{
			if (need_rerender)
			{
				need_rerender = false;
				updateTexture();
			}
			message((tRenderRect*)ddwindow, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
			
			if (ddwindow->checkOption(tDropDownWindow::option_mask.can_be_drawn))
			{
				sf::FloatRect bounds = getLocalBounds();
				sf::Vector2f ddw_position = ddwindow->getPosition();
				ddwindow->setPosition(ddw_position + sf::Vector2f{ bounds.left, bounds.top });
				ddwindow->draw(target);
				ddwindow->setPosition(ddw_position);
			}
			
			// Отобразиться
			target.draw(render_squad, &render_texture.getTexture());
		}
	}

	void tDropDownList::insertVariant(const std::wstring& variant_text, const int& variant_code, const float& height, const sf::Vector2f& text_offset)
	{
		tDropDownVariant* variant = new tDropDownVariant(this, variant_text, { 0.f, 0.f, ddwindow->getLocalBounds().width - tScrollbar::thickness, height});
		variant->setCode(variant_code);
		ddwindow->_insert(variant);
	}

	bool tDropDownList::deleteVariant(const std::wstring& variant_text)
	{
		bool success = false;
		std::list<tDropDownVariant*>* elem = ddwindow->getVariantList();
		for (std::list<tDropDownVariant*>::iterator it = elem->begin(); it != elem->end(); it++)
		{	// Поиск удаляемого элемента
			if ((*it)->getString() == variant_text)
			{
				success = true;
				delete* it;		// Удаляем его	1) из памяти
				elem->erase(it);	//			2) из контейнера
				((tObject*)elem->back())->setOneOption(tObject::option_mask.is_active, true);
				message(elem->back(), tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
				break;	// Вываливаемся из перебора, чтобы не поймать аксес виолэйшн
			}
		}
		return success;
	}

	bool tDropDownList::deleteVariant(const int& variant_code)
	{
		bool success = false;
		std::list<tDropDownVariant*>* elem = ddwindow->getVariantList();
		for (std::list<tDropDownVariant*>::iterator it = elem->begin(); it != elem->end(); it++)
		{	// Поиск удаляемого элемента
			if ((*it)->getCode() == variant_code)
			{
				success = true;
				delete* it;		// Удаляем его	1) из памяти
				elem->erase(it);	//			2) из контейнера
				((tObject*)elem->back())->setOneOption(tObject::option_mask.is_active, true);
				message(elem->back(), tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
				break;	// Вываливаемся из перебора, чтобы не поймать аксес виолэйшн
			}
		}
		return success;
	}

	void tDropDownList::setDropDownWindowSize(const sf::Vector2f& size)
	{
		ddwindow->setSize(size);
		if (direction)
		{	// Если раскрывается вверх, то меняем положение
			//sf::Vector2f owner_offset = { getLocalBounds().width, getLocalBounds().height };
			//ddwindow->setPosition( owner_offset + sf::Vector2f{ 0.f, -size.y });
			ddwindow->setPosition({ 0.f, -size.y });
		}
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
		scrollbar_v(new tScrollbar((tRenderRect*)this, true, {0, 0, tScrollbar::thickness, rect.height }))
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
		if (checkOption(option_mask.can_be_drawn))
		{
			tGroup::handleEvent(e);
			scrollbar_v->handleEvent(e);
			switch (e.type)
			{
			case tEvent::types.Broadcast:
			{
				if (e.address == (tRenderRect*)this)
				{
					switch (e.code)
					{
					case tEvent::codes.Activate:
					{	// Установить фокус на объекта
						((tObject*)e.from)->setOneOption(option_mask.is_active, true);
						tAbstractBasicClass::clearEvent(e);
						break;
					}
					case tEvent::codes.Deactivate:
					{	// Снять фокус с объекта
						((tObject*)e.from)->setOneOption(option_mask.is_active, false);
						tAbstractBasicClass::clearEvent(e);
						break;
					}
					case tEvent::codes.Show:
					{	// Показать объект (если он скрыт)
						((tObject*)e.from)->setOneOption(option_mask.can_be_drawn, true);
						tAbstractBasicClass::clearEvent(e);
						break;
					}
					case tEvent::codes.Hide:
					{	// Спрятать объект (если он не скрыт)
						((tObject*)e.from)->setOneOption(option_mask.can_be_drawn, false);
						tAbstractBasicClass::clearEvent(e);
						break;
					}
					case tEvent::codes.Adopt:
					{	// Стать владельцем объекта
						e.from->setOwner((tRenderRect*)this);
						tAbstractBasicClass::clearEvent(e);
						break;
					}
					case tEvent::codes.UpdateTexture:
					{	// Обновить текстуру
						tAbstractBasicClass::message(getOwner(), tEvent::types.Broadcast, tEvent::codes.UpdateTexture, (tRenderRect*)this);
						need_rerender = true;
						tAbstractBasicClass::clearEvent(e);
						break;
					}
					case tEvent::codes.StopAndDoNotMove:
					{	// Сбросить флаг перетаскивания мышью
						forEach(tEvent::codes.StopAndDoNotMove);
						// Не обнуляем событие
						break;
					}
					default:
					{	// Если не обработалось, то "проталкиваем" на уровень ниже
						e.address = getOwner();
						tAbstractBasicClass::message(e);
						tAbstractBasicClass::clearEvent(e);
						break;
					}
					}
				}
				break;
			}
			case tEvent::types.Button:
			{
				if (e.address == (tRenderRect*)this)
				{
					tAbstractBasicClass::message(getOwner(), tEvent::types.Button, e.code, e.from);
					setOneOption(option_mask.can_be_drawn, false);
					tAbstractBasicClass::clearEvent(e);
					break;
				}
			}
			case tEvent::types.Mouse:
			{
				mouse_inside[1] = mouse_inside[0];
				mouse_inside[0] = pointIsInsideMe({ e.mouse.x, e.mouse.y });
			}
			}
		}
	}

	void tDropDownWindow::updateTexture()
	{
		render_texture.clear(clear_color);	// Очиститься
		tGroup::draw(render_texture);		// Нарисовать подэлементы
		scrollbar_v->draw(render_texture);	// Нарисовать scrollbar

		sf::VertexArray arr(sf::PrimitiveType::LinesStrip, 5);
		sf::Color color = { 150, 150, 150, 255 };
		for (int i = 0; i < 5; i++)
		{
			arr[i].color = color;
		}
		sf::FloatRect bounds = getLocalBounds();
		arr[0].position = { 1.f, 0.f };
		arr[1].position = { bounds.width, 0};
		arr[2].position = { bounds.width, bounds.height - 1 };
		arr[3].position = { 1.f, bounds.height - 1 };
		arr[4].position = { 1.f, 0.f };
		render_texture.draw(arr);

		render_texture.display();			// Обновить "лицевую" текстуру
	}

	void tDropDownWindow::setSize(const sf::Vector2f& size)
	{
		tDisplay::setSize(size);
		tDisplay::setTextureSize({ (unsigned int)size.x, (unsigned int)size.y });

		float scrollbar_width = scrollbar_v->getLocalBounds().width;
		scrollbar_v->setSize({ scrollbar_width, size.y - 2 });
		scrollbar_v->setTextureSize({ (unsigned int)scrollbar_width, (unsigned int)size.y - 2 });
		scrollbar_v->setPosition({ -scrollbar_width - 2, 1 });
		scrollbar_v->setTargetSize({ getLocalBounds().width, getLocalBounds().height });
		scrollbar_v->setTargetTextureSize(getTextureSize());
		tAbstractBasicClass::message(scrollbar_v, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, (tRenderRect*)this);
	}

	std::list<tDropDownVariant*>* tDropDownWindow::getVariantList()
	{
		return (std::list<tDropDownVariant*>*)&elem;
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