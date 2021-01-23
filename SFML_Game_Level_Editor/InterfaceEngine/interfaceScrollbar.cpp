#include "stdafx.h"
#include "interfaceEngine.h"

namespace edt
{
	void tScrollbar::initScrollbar(const bool vertical)
	{
		slider->setOneOption(tRectShape::option_mask.can_be_moved, true);
		slider->setColor({ 60, 60, 60, 255 });
		if (vertical)
		{
			arrow_first->setString(L"^");
			arrow_first->setPosition({ 0, 0 });
			arrow_second->setString(L"v");
			arrow_second->setAnchor(anchors.bottom_left_corner);
			arrow_second->setPosition({ 0, -thickness });
			slider->setPosition({ 1, thickness + 1 });
		}
		else
		{
			arrow_first->setString(L"<");
			arrow_first->setPosition({ 0, 0 });
			arrow_second->setString(L">");
			arrow_second->setAnchor(anchors.upper_right_corner);
			arrow_second->setPosition({ -thickness, 0 });
			slider->setPosition({ thickness + 1, 1 });
		}
		updateScrollerSize();
	}

	tScrollbar::tScrollbar(tAbstractBasicClass* _owner, const bool vertical, sf::FloatRect rect) :
		tRenderRect(_owner),
		arrow_first(new tButton(this, { 0, 0, thickness, thickness })),
		arrow_second(new tButton(this, { 0, 0, thickness, thickness })),
		slider(new tRectShape(this, { 0, 0, thickness - 2, thickness - 2 })),
		old_position(0.f, 0.f),
		color_scroller_area({ 241, 241, 241, 255 }),
		target_size({ 1.f, 1.f }),
		target_texture_size({ 1, 1 })
	{
		setOneOption(option_mask.vectically_orientated, vertical);

		clear_color = color_scroller_area;

		setPosition({ rect.left, rect.top });
		setSize({ rect.width, rect.height });
		setTextureSize({ (unsigned int)rect.width, (unsigned int)rect.height });

		initScrollbar(vertical);
	}

	tScrollbar::tScrollbar(tAbstractBasicClass* _owner, nlohmann::json& js) :
		tRenderRect(_owner, js),
		arrow_first(new tButton(this, js["arrow_first"])),
		arrow_second(new tButton(this, js["arrow_second"])),
		slider(new tRectShape(this, js["slider"])),
		old_position(0.f, 0.f),
		target_size({ 1.f, 1.f }),
		target_texture_size({ 1, 1 })
	{
		vec<unsigned char> vuc = js["color_scroller_area"].get<vec<unsigned char>>();
		color_scroller_area = { vuc[0], vuc[1], vuc[2], vuc[3] };
		vuc.clear();

		vec<unsigned int> vui = js["target_texture_size"].get<vec<unsigned int>>();
		target_texture_size = { vui[0], vui[1] };
		vui.clear();

		vec<float> vuf = js["target_size"].get<vec<float>>();
		target_size = { vuf[0], vuf[1] };
		vuf.clear();

		clear_color = color_scroller_area;

		updateScrollerSize();
	}

	tScrollbar::tScrollbar(const tScrollbar& s) :
		tRenderRect(s),
		arrow_first(s.arrow_first),
		arrow_second(s.arrow_second),
		slider(s.slider),
		old_position(s.old_position),
		color_scroller_area(s.color_scroller_area),
		target_size(s.target_size),
		target_texture_size(s.target_texture_size)
	{
	}

	tScrollbar::~tScrollbar()
	{
		delete arrow_first;
		delete arrow_second;
		delete slider;
	}

	void tScrollbar::updateScrollerSize()
	{
		sf::FloatRect scrollbar_rect = getLocalBounds();
		sf::Vector2f scroller_size;

		if (checkOption(option_mask.vectically_orientated))
		{	// Отношение лежит в диапазоне от 0 до 1. Домножаем его на максимальный размер ползунка
			// с учётом "стрелок" на скроллбаре и получаем текущий размер ползунка.
			scroller_size.x = thickness - 3;
			scroller_size.y = MIN((target_size.y / target_texture_size.y) * (scrollbar_rect.height - 2 * thickness) - 2, scrollbar_rect.height - 2 * thickness);
		}
		else
		{
			scroller_size.x = MIN((target_size.x / target_texture_size.x) * (scrollbar_rect.width - 2 * thickness) - 2, scrollbar_rect.width - 2 * thickness);
			scroller_size.y = thickness - 3;
		}
		slider->setSize(scroller_size);
	}

	void tScrollbar::updateTexture()
	{
		render_texture.clear(clear_color);

		arrow_first->draw(render_texture);
		arrow_second->draw(render_texture);
		slider->draw(render_texture);

		render_texture.display();
	}

	void tScrollbar::handleEvent(tEvent& e)
	{
		if (checkOption(option_mask.can_be_drawn))
		{
			arrow_first->handleEvent(e);
			arrow_second->handleEvent(e);
			slider->handleEvent(e);

			switch (e.type)
			{
			case tEvent::types.Broadcast:
			{
				if (e.address == this)
				{
					switch (e.code)
					{
					case tEvent::codes.Activate:
					{	// Установить фокус на объект
						((tObject*)e.from)->setOneOption(tObject::option_mask.is_active, true);
						message(getOwner(), tEvent::types.Broadcast, tEvent::codes.Activate, this);
						clearEvent(e);
						break;
					}
					case tEvent::codes.Deactivate:
					{	// Снять фокус с объекта
						((tObject*)e.from)->setOneOption(tObject::option_mask.is_active, false);
						clearEvent(e);
						break;
					}
					case tEvent::codes.Show:
					{	// Показать объект (если он скрыт)
						((tObject*)e.from)->setOneOption(tObject::option_mask.can_be_drawn, true);
						clearEvent(e);
						break;
					}
					case tEvent::codes.Hide:
					{	// Спрятать объект (если он не скрыт)
						((tObject*)e.from)->setOneOption(tObject::option_mask.can_be_drawn, false);
						clearEvent(e);
						break;
					}
					case tEvent::codes.UpdateTexture:
					{	// Обновить текстуру
						message(getOwner(), tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
						need_rerender = true;
						clearEvent(e);
						break;
					}
					case tEvent::codes.StopAndDoNotMove:
					{	// Сбросить флаг перетаскивания мышью
						message(slider, tEvent::types.Broadcast, tEvent::codes.StopAndDoNotMove, this);
						// Не обнуляем событие
						break;
					}
					default:
					{	// Если не обработалось, то "проталкиваем" на уровень ниже
						e.address = getOwner();
						message(e);
						clearEvent(e);
						break;
					}
					}
				}
				break;
			}
			case tEvent::types.Button:
			{
				if (e.address == this)
				{
					if ((e.from == arrow_first) || (e.from == arrow_second))
					{
						if (e.from == arrow_first)
						{
							moveSlider(-default_step);
						}
						else
						{
							moveSlider(default_step);
						}
						message(this, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
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
					{
						message(nullptr, tEvent::types.Broadcast, tEvent::codes.ResetButtons, this);
						clearEvent(e);
					}
					if (mouse_inside[0])
					{
						message(this, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
						clearEvent(e);
					}
					break;
				}
				}
				break;
			}
			}

			// Нужно вернуть из-за пределов скроллбара (если он там окажется)
			sf::FloatRect slider_rect = slider->getLocalBounds();
			sf::FloatRect rect = getLocalBounds();

			if ((old_position.x != slider_rect.left) || (old_position.y != slider_rect.top))
			{
				if (checkOption(option_mask.vectically_orientated))
				{
					slider_rect.left = 1;
					slider_rect.top = CLAMP(slider_rect.top, thickness + 1, rect.height - slider_rect.height - thickness - 1);
				}
				else
				{
					slider_rect.top = 1;
					slider_rect.left = CLAMP(slider_rect.left, thickness + 1, rect.width - slider_rect.width - thickness - 1);
				}
				slider->setPosition({ slider_rect.left, slider_rect.top });
				old_position = { slider_rect.left, slider_rect.top };
				message(this, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
			}
		}
	}

	bool tScrollbar::pointIsInsideMe(sf::Vector2i point) const
	{
		sf::FloatRect rect = getGlobalBounds();
		return (
			point.x >= rect.left && 
			point.x <= rect.left + rect.width && 
			point.y >= rect.top && 
			point.y <= rect.top + rect.height
		);
	}

	void tScrollbar::moveSlider(const int& _step)
	{
		sf::Vector2f step = { 0.f, 0.f };
		if (checkOption(option_mask.vectically_orientated))
		{
			step.y = (float)_step / target_texture_size.y;
		}
		else
		{
			step.x = (float)_step / target_texture_size.x;
		}
		sf::Vector2f length = {
			MAX(getLocalBounds().width - 2 * thickness, 0.f),
			MAX(getLocalBounds().height - 2 * thickness, 0.f)
		};
		step.x *= length.x;
		step.y *= length.y;
		slider->move(step);
		message(this, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
	}

	void tScrollbar::setTargetSize(const sf::Vector2f& new_size)
	{
		target_size = new_size;
	}

	void tScrollbar::setTargetTextureSize(const sf::Vector2u& new_size)
	{
		target_texture_size = new_size;
	}

	void tScrollbar::setTextureSize(const sf::Vector2u& new_size) {
		render_texture.create(new_size.x, new_size.y);
		render_squad[0].texCoords = { 0.f, 0.f };
		render_squad[1].texCoords = { (float)new_size.x - 1, 0.f };
		render_squad[2].texCoords = { (float)new_size.x - 1, (float)new_size.y - 1 };
		render_squad[3].texCoords = { 0.f, (float)new_size.y - 1 };
		need_rerender = true;
		updateScrollerSize();
	}

	float tScrollbar::getPixelOffset() const
	{
		float length_of_moving_zone, slider_pos, targ_tex_size, targ_size;
		if (checkOption(option_mask.vectically_orientated))
		{
			length_of_moving_zone = getLocalBounds().height - slider->getLocalBounds().height - thickness;
			slider_pos = slider->getLocalBounds().top;
			targ_tex_size = (float)target_texture_size.y;
			targ_size = target_size.y;
		}
		else
		{
			length_of_moving_zone = getLocalBounds().width - slider->getLocalBounds().width - thickness;
			slider_pos = slider->getLocalBounds().left;
			targ_tex_size = (float)target_texture_size.x;
			targ_size = target_size.x;
		}
		/*
		lengthOfMovingZone				targetTextureSize - targetSize
		_________________________	=	______________________________
		sliderPos - thickness - 1		offset
		*/
		return (targ_tex_size - targ_size) * (slider_pos - thickness - 1) / length_of_moving_zone;
	}

	nlohmann::json tScrollbar::getParamsInJson() const
	{
		nlohmann::json js = tRenderRect::getParamsInJson();

		js["what_it_it"] = objects_json_ids.tScrollbar;
		js["what_is_it_string"] = "tScrollbar";

		js["arrow_first"] = arrow_first->getParamsInJson();
		js["arrow_second"] = arrow_second->getParamsInJson();
		js["slider"] = slider->getParamsInJson();

		sf::Color color = color_scroller_area;
		js["color_scroller_area"] = { color.r, color.g, color.b, color.a };

		js["target_texture_size"] = { target_texture_size.x, target_texture_size.y };
		js["target_size"] = { target_size.x, target_size.y };

		return js;
	}
}