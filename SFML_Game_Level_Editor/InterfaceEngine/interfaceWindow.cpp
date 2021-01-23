#include "stdafx.h"
#include "interfaceEngine.h"

namespace edt
{
	void tWindow::initWindow()
	{
		sf::FloatRect rect = getLocalBounds();

		button_close->setAnchor(anchors.upper_right_corner);
		button_close->setPosition({ -heap_height + 2, 2 });
		button_close->setCode(tEvent::codes.Close);
		button_close->setString(L"x");
		button_close->setTextColor({ 255, 255, 255, 255 });
		button_close->setCharSize(20);
		button_close->setOutlineThickness(1);
		button_close->setTextAlignment(tButton::text_alignment_type.Middle);
		button_close->setTextOffset({ 0, -3 });
		message(button_close, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);

		display->setClearColor(color_area);

		sf::FloatRect display_bounds = display->getLocalBounds();

		scrollbar_v->setAnchor(anchors.upper_right_corner);
		scrollbar_v->setPosition({ -tScrollbar::thickness - 2, heap_height });
		scrollbar_v->setTargetSize({ display_bounds.width, display_bounds.height });
		scrollbar_v->setTargetTextureSize(display->getTextureSize());

		scrollbar_h->setAnchor(anchors.bottom_left_corner);
		scrollbar_h->setPosition({ 1, -tScrollbar::thickness - 2 });
		scrollbar_h->setTargetSize({ display_bounds.width, display_bounds.height });
		scrollbar_h->setTargetTextureSize(display->getTextureSize());
	}

	tWindow::tWindow(tAbstractBasicClass* _owner, sf::FloatRect rect, std::wstring _caption) :
		tRenderRect(_owner, rect),
		font_loaded(false),
		caption(_caption),
		color_heap(sf::Color(100, 100, 100, 255)),
		color_area(sf::Color(80, 80, 80, 255)),
		color_caption_active(sf::Color(255, 255, 255, 255)),
		color_caption_inactive(sf::Color(150, 150, 150, 255)),
		caption_offset({ 2, 2 }),
		button_close(new tButton(this, { 0, 0, heap_height - 4 , heap_height - 4 })),
		heap_shape(new tRectShape(this, { 0, 0, rect.width, heap_height }, color_heap)),
		display(new tDisplay(this, { 0, heap_height, rect.width, rect.height - heap_height })),
		scrollbar_v(new tScrollbar(this, true, { 0, 0, tScrollbar::thickness, rect.height - tScrollbar::thickness - heap_height - 1 })),
		scrollbar_h(new tScrollbar(this, false, { 0, 0, rect.width - tScrollbar::thickness - 2, tScrollbar::thickness })),
		last_scrollbar_offset({ 0.f, 0.f })
	{
		setOneOption(option_mask.can_be_moved, true);
		setOneOption(option_mask.can_be_resized, true);

		setPosition({ rect.left, rect.top });
		setSize({ rect.width, rect.height });
		setTextureSize({ (unsigned int)rect.width, (unsigned int)rect.height });

		initWindow();
	}

	tWindow::tWindow(tAbstractBasicClass* _owner, nlohmann::json& js) :
		tRenderRect(_owner, js),
		font_loaded(false),
		button_close(new tButton(this, js["button_close"])),
		heap_shape(new tRectShape(this, js["heap_shape"])),
		display(new tDisplay(this, js["display"])),
		scrollbar_v(new tScrollbar(this, js["scrollbar_v"])),
		scrollbar_h(new tScrollbar(this, js["scrollbar_h"])),
		last_scrollbar_offset({ 0.f, 0.f })
	{
		vec<int> vi = js["caption"].get<vec<int>>();
		caption = convertIntVectorToWstring(vi);
		vec<float> vf = js["caption_offset"].get<vec<float>>();
		caption_offset = { vf[0], vf[1] };
		vec<unsigned char> vuc = js["color_heap"].get<vec<unsigned char>>();
		color_heap = { vuc[0], vuc[1], vuc[2], vuc[3] };
		vuc.clear();
		vuc = js["color_area"].get<vec<unsigned char>>();
		color_area = { vuc[0], vuc[1], vuc[2], vuc[3] };
		vuc.clear();
		vuc = js["color_caption_active"].get<vec<unsigned char>>();
		color_caption_active = { vuc[0], vuc[1], vuc[2], vuc[3] };
		vuc.clear();
		vuc = js["color_caption_inactive"].get<vec<unsigned char>>();
		color_caption_inactive = { vuc[0], vuc[1], vuc[2], vuc[3] };
		vuc.clear();
	}

	tWindow::tWindow(const tWindow& w) :
		tRenderRect(w),
		font_loaded(w.font_loaded),
		font(w.font),
		caption(w.caption),
		color_heap(w.color_heap),
		color_area(w.color_area),
		color_caption_active(w.color_caption_active),
		color_caption_inactive(w.color_caption_inactive),
		caption_offset(w.caption_offset),
		button_close(w.button_close),
		heap_shape(w.heap_shape),
		display(w.display),
		scrollbar_v(w.scrollbar_v),
		scrollbar_h(w.scrollbar_h),
		last_scrollbar_offset(w.last_scrollbar_offset)
	{
	}

	tWindow::~tWindow()
	{
		delete button_close;
		delete heap_shape;
		delete display;
		delete scrollbar_v;
		delete scrollbar_h;
	}

	bool tWindow::pointIsInHeap(sf::Vector2i point) const
	{
		sf::FloatRect rect = getGlobalBounds();
		return (
			point.x >= rect.left && 
			point.x <= rect.left + rect.width && 
			point.y >= rect.top && 
			point.y <= rect.top + heap_height
		);
	}

	void tWindow::handleEvent(tEvent& e)
	{
		if (checkOption(option_mask.can_be_drawn))
		{

			button_close->handleEvent(e);
			display->handleEvent(e);
			scrollbar_v->handleEvent(e);
			scrollbar_h->handleEvent(e);

			/*
			=================================================================================================================
			>>>>>>>>>> Временная штука. Это нужно будет переместить в блок изменения размеров окна (сделать ивент) <<<<<<<<<<
			=================================================================================================================
			*/

			auto dispTexSize = display->getTextureSize();

			if (scrollbar_v->checkOption(option_mask.can_be_drawn) && dispTexSize.y <= getTextureSize().y - heap_height)
			{	// Если скроллбар активен и размер текстуры дисплея влезают в зону отрисовки окна, тогда выключаем скроллбар
				display->setSize({ 
					getLocalBounds().width, 
					display->getLocalBounds().height 
				});
				scrollbar_v->setOneOption(option_mask.can_be_drawn, false);
			}
			else if (!scrollbar_v->checkOption(option_mask.can_be_drawn) && dispTexSize.y > getTextureSize().y - heap_height)
			{
				display->setSize({ 
					getLocalBounds().width - tScrollbar::thickness - 2, 
					display->getLocalBounds().height 
				});
				scrollbar_v->setOneOption(option_mask.can_be_drawn, true);
			}

			if (scrollbar_h->checkOption(option_mask.can_be_drawn) && display->getTextureSize().x <= getTextureSize().x)
			{	// Если скроллбар активен и размер текстуры дисплея влезает в зону отрисовки окна, тогда выключаем скроллбар
				display->setSize({ 
					display->getLocalBounds().width, 
					getLocalBounds().height - heap_height 
				});
				scrollbar_h->setOneOption(option_mask.can_be_drawn, false);
			}
			else if (!scrollbar_h->checkOption(option_mask.can_be_drawn) && display->getTextureSize().x > getTextureSize().x)
			{
				display->setSize({ 
					display->getLocalBounds().width, 
					getLocalBounds().height - heap_height - tScrollbar::thickness - 2 
				});
				scrollbar_h->setOneOption(option_mask.can_be_drawn, true);
			}

			/*
			=================================================================================================================
			>>>>>>>>>> Временная штука. Это нужно будет переместить в блок изменения размеров окна (сделать ивент) <<<<<<<<<<
			=================================================================================================================
			*/

			sf::Vector2f scrollbar_offset = { scrollbar_h->getPixelOffset(), scrollbar_v->getPixelOffset() };
			if (scrollbar_offset != last_scrollbar_offset)
			{
				setCameraOffset(scrollbar_offset);
				last_scrollbar_offset = scrollbar_offset;
			}

			switch (e.type)
			{
			case tEvent::types.Broadcast:
			{
				if (e.address == this)
				{	// Для конкретно этого окна
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
					case tEvent::codes.UpdateTexture: {	// Обновить текстуру
						message(getOwner(), tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
						need_rerender = true;
						clearEvent(e);
						break;
					}
					case tEvent::codes.FontAnswer:
					{	// Забрать выданный системой шрифт
						font_loaded = true;
						setFont(*e.font.font);
						message(this, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
						message(getOwner(), tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
						clearEvent(e);
						break;
					}
					case tEvent::codes.StopAndDoNotMove:
					{	// Сбросить флаг перетаскивания мышью
						message(button_close, tEvent::types.Broadcast, tEvent::codes.StopAndDoNotMove, this);
						message(heap_shape, tEvent::types.Broadcast, tEvent::codes.StopAndDoNotMove, this);
						message((tRenderRect*)display, tEvent::types.Broadcast, tEvent::codes.StopAndDoNotMove, this);
						message(scrollbar_v, tEvent::types.Broadcast, tEvent::codes.StopAndDoNotMove, this);
						message(scrollbar_h, tEvent::types.Broadcast, tEvent::codes.StopAndDoNotMove, this);
						setOneOption(option_mask.is_moving_by_mouse, false);
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
				{	// Для конкретно этого окна
					switch (e.code)
					{
					case tEvent::codes.Close:
					{	// Закрыть окно
						e.type = tEvent::types.Broadcast;
						e.code = tEvent::codes.Delete;
						e.address = getOwner();
						e.from = this;
						putEvent(e);
						clearEvent(e);
						break;
					}
					default:
					{	// Если не обработалось, то "проталкиваем" на уровень ниже
						e.address = getOwner();
						putEvent(e);
						clearEvent(e);
					}
					}
				}
				break;
			}
			case tEvent::types.Mouse:
			{
				mouse_inside[1] = mouse_inside[0];
				mouse_inside[0] = pointIsInsideMe({ e.mouse.x, e.mouse.y });
				switch (e.code) {
				case tEvent::codes.MouseButton:
				{
					if (mouse_inside[0])
					{	// Если мышь внутри, ...
						if (e.mouse.what_happened == sf::Event::MouseButtonReleased)
						{	// Если отпустили кнопку
							setOneOption(option_mask.is_moving_by_mouse, false);
						}
						else if (e.mouse.what_happened == sf::Event::MouseButtonPressed)
						{
							if (e.mouse.button == sf::Mouse::Left)
							{	// Если в окно тыкнули левой кнопкой мыши
								if (pointIsInHeap({ e.mouse.x, e.mouse.y }) && checkOption(option_mask.can_be_moved))
								{
									setOneOption(option_mask.is_moving_by_mouse, true);
								}
								message(getOwner(), tEvent::types.Broadcast, tEvent::codes.Activate, this);
								need_rerender = true;
							}
						}
						clearEvent(e);
					}
					break;
				}
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
					if (checkOption(option_mask.is_moving_by_mouse))
					{
						move({ (float)e.mouse.dX, (float)e.mouse.dY });
					}
					break;
				}
				}
				break;
			}
			}
		}
	}

	void tWindow::updateTexture()
	{
		render_texture.clear(clear_color);

		display->draw(render_texture);
		heap_shape->draw(render_texture);
		button_close->draw(render_texture);
		scrollbar_v->draw(render_texture);
		scrollbar_h->draw(render_texture);

		if (font_loaded)
		{
			sf::Text text;
			text.setString(caption);
			text.setCharacterSize(caption_char_size);
			text.setFont(font);
			text.setFillColor(checkOption(option_mask.is_active) ? color_caption_active : color_caption_inactive);
			text.setPosition(caption_offset);
			text.setOutlineThickness(1);
			render_texture.draw(text);
		}
		else
		{	// Если шрифта нет, мы должны запросить его, и после получения обновить текстуру
			message(nullptr, tEvent::types.Broadcast, tEvent::codes.FontRequest, this);
		}
		// Рисование обводки окна
		sf::VertexArray frame(sf::LineStrip, 5);
		sf::FloatRect rect = getLocalBounds();
		sf::Color color = { 140, 140, 140, 255 };
		frame[0].position = { 1, 0 };
		frame[1].position = { rect.width - 1, 0 };
		frame[2].position = { rect.width - 1, rect.height - 2 };
		frame[3].position = { 1, rect.height - 2 };
		frame[4].position = { 1, 1 };
		if (checkOption(tObject::option_mask.is_active))
		{
			color = { 0, 122, 204, 255 };
		}
		for (int i = 0; i < 5; i++)
		{
			frame[i].color = color;
		}
		render_texture.draw(frame);

		render_texture.display();
	}

	bool tWindow::pointIsInsideMe(sf::Vector2i point) const
	{
		sf::FloatRect rect = getGlobalBounds();
		return (
			point.x >= rect.left && 
			point.x <= rect.left + rect.width && 
			point.y >= rect.top && 
			point.y <= rect.top + rect.height
		);
	}

	void tWindow::setCaption(const std::wstring& new_caption)
	{
		caption = new_caption;
	}

	void tWindow::setHeapColor(const sf::Color& new_color)
	{
		color_heap = new_color;
	}

	void tWindow::setAreaColor(const sf::Color& new_color)
	{
		color_area = new_color;
	}

	void tWindow::setActiveCaptionColor(const sf::Color& new_color)
	{
		color_caption_active = new_color;
	}

	void tWindow::setInactiveCaptionColor(const sf::Color& new_color)
	{
		color_caption_inactive = new_color;
	}

	void tWindow::setFont(const sf::Font& new_font)
	{
		font_loaded = true;
		font = new_font;
	}

	void tWindow::setCaptionOffset(const sf::Vector2f& new_offset)
	{
		caption_offset = new_offset;
	}

	void tWindow::setCameraOffset(const sf::Vector2f& new_offset)
	{
		display->setCameraOffset(new_offset);
	}

	void tWindow::setDisplaySize(const sf::Vector2f& new_size)
	{
		display->setSize(new_size);
		scrollbar_v->setTargetSize(new_size);
		scrollbar_h->setTargetSize(new_size);
		scrollbar_v->updateScrollerSize();
		scrollbar_h->updateScrollerSize();
		message(scrollbar_v, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
		message(scrollbar_h, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
	}

	void tWindow::setDisplayTextureSize(const sf::Vector2u& new_size)
	{
		display->setTextureSize(new_size);
		scrollbar_v->setTargetTextureSize(new_size);
		scrollbar_h->setTargetTextureSize(new_size);
		scrollbar_v->updateScrollerSize();
		scrollbar_h->updateScrollerSize();
		message(scrollbar_v, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
		message(scrollbar_h, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
	}

	tDisplay* tWindow::getDisplayPointer() const
	{
		return display;
	}

	std::wstring tWindow::getCaption() const
	{
		return caption;
	}

	const int tWindow::getHeapHeight() const
	{
		return heap_height;
	}

	nlohmann::json tWindow::getParamsInJson() const
	{
		nlohmann::json js = tRenderRect::getParamsInJson();

		js["what_is_it"] = objects_json_ids.tWindow;
		js["what_is_it_string"] = "tWindow";

		js["caption"] = caption;
		js["caption_offset"] = { caption_offset.x, caption_offset.y };
		js["color_heap"] = { color_heap.r, color_heap.g, color_heap.b, color_heap.a };
		js["color_area"] = { color_area.r, color_area.g, color_area.b, color_area.a };
		js["color_caption_active"] = { color_caption_active.r, color_caption_active.g, color_caption_active.b, color_caption_active.a };
		js["color_caption_inactive"] = { color_caption_inactive.r, color_caption_inactive.g, color_caption_inactive.b, color_caption_inactive.a };

		js["button_close"] = button_close->getParamsInJson();
		js["heap_shape"] = heap_shape->getParamsInJson();
		js["display"] = display->getParamsInJson();
		js["scrollbar_v"] = scrollbar_v->getParamsInJson();
		js["scrollbar_h"] = scrollbar_h->getParamsInJson();

		return js;
	}
}