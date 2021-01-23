#include "stdafx.h"
#include "interfaceEngine.h"

namespace edt
{
	tRectShape::tRectShape(tAbstractBasicClass* _owner, sf::FloatRect rect, sf::Color fill_color) :
		tObject(_owner)
	{
		setOneOption(option_mask.can_be_moved, true);
		setPosition({ rect.left, rect.top });
		setSize({ rect.width, rect.height });
		setColor(fill_color);
	}

	tRectShape::tRectShape(tAbstractBasicClass* _owner, nlohmann::json& js) :
		tObject(_owner, js)
	{
		vec<float> vf = js["size"].get<vec<float>>();
		setSize({ vf[0], vf[1] });

		vec<unsigned char> vuc = js["color"].get<vec<unsigned char>>();
		setColor({ vuc[0], vuc[1], vuc[2], vuc[3] });

		setPosition({ x, y });
	}

	tRectShape::tRectShape(const tRectShape& s) :
		tObject(s),
		shape(s.shape)
	{
	}

	tRectShape::~tRectShape()
	{
	}

	void tRectShape::draw(sf::RenderTarget& target)
	{
		if (checkOption(option_mask.can_be_drawn))
		{
			target.draw(shape);
		}
	}

	void tRectShape::move(sf::Vector2f delta)
	{
		tObject::move(delta);
		shape.move(delta);
	}

	void tRectShape::updateTexture()
	{
		return;
	}

	void tRectShape::handleEvent(tEvent& e)
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
					case tEvent::codes.StopAndDoNotMove:
					{	// Сбросить флаг перетаскивания мышью
						setOneOption(option_mask.is_moving_by_mouse, false);
						// Не обнуляем событие
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
				switch (e.code)
				{
				case tEvent::codes.MouseButton:
				{
					if (mouse_inside[0])
					{	// Если мышь внутри, ...
						if (e.mouse.what_happened == sf::Event::MouseButtonReleased)
						{	// Если отпустили кнопку
							setOneOption(option_mask.is_moving_by_mouse, false);
						}
						else if (e.mouse.what_happened == sf::Event::MouseButtonPressed)
						{	// Если нажали кнопку
							if (e.mouse.button == sf::Mouse::Left)
							{	// Если тыкнули левой кнопкой мыши
								if (checkOption(option_mask.can_be_moved))
								{
									setOneOption(option_mask.is_moving_by_mouse, true);
								}
								message(getOwner(), tEvent::types.Broadcast, tEvent::codes.Activate, this);
							}
						}
						clearEvent(e);
					}
					break;
				}
				case tEvent::codes.MouseMoved:
				{
					if (checkOption(option_mask.is_moving_by_mouse))
					{
						move({ (float)e.mouse.dX, (float)e.mouse.dY });
						message(getOwner(), tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
					}
					if (mouse_inside[0] != mouse_inside[1])
					{
						message(nullptr, tEvent::types.Broadcast, tEvent::codes.ResetButtons, this);
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

	bool tRectShape::pointIsInsideMe(sf::Vector2i point) const
	{
		sf::FloatRect rect = getGlobalBounds();
		return (
			point.x >= rect.left && 
			point.x <= rect.left + rect.width && 
			point.y >= rect.top && 
			point.y <= rect.top + rect.height
		);
	}

	void tRectShape::setColor(const sf::Color& new_color)
	{
		shape.setFillColor(new_color);
	}

	void tRectShape::setPosition(const sf::Vector2f& new_position)
	{
		tObject::setPosition(new_position);
		shape.setPosition(new_position + getRelativeStartPosition());
	}

	void tRectShape::setSize(const sf::Vector2f& new_size)
	{
		shape.setSize(new_size);
	}

	sf::FloatRect tRectShape::getLocalBounds() const
	{
		return sf::FloatRect(
			shape.getPosition().x,
			shape.getPosition().y,
			shape.getSize().x * shape.getScale().x,
			shape.getSize().y * shape.getScale().y
		);
	}

	nlohmann::json tRectShape::getParamsInJson() const
	{
		nlohmann::json js = tObject::getParamsInJson();

		sf::Vector2f size = shape.getSize();
		sf::Color color = shape.getFillColor();

		js["what_is_it"] = objects_json_ids.tRectShape;
		js["what_is_it_string"] = "tRectShape";
		js["size"] = { size.x, size.y };
		js["color"] = { color.r, color.g, color.b, color.a };

		return js;
	}
}