#include "stdafx.h"
#include "interfaceEngine.h"

namespace edt {
	tDisplay::tDisplay(tAbstractBasicClass* _owner, sf::FloatRect rect) :
		owner(_owner),
		tGroup((tRenderRect*)this),
		tRenderRect(_owner, rect)
	{
	}

	tDisplay::tDisplay(tAbstractBasicClass* _owner, nlohmann::json& js) :
		owner(_owner),
		tGroup((tRenderRect*)this, js),
		tRenderRect(_owner, js)
	{
		vec<float> vf = js["camera_offset"].get<vec<float>>();
		setCameraOffset({ vf[0], vf[1] });

		setAnchor(anchor);
	}

	tDisplay::tDisplay(const tDisplay& d) :
		owner(d.owner),
		tGroup(d),
		tRenderRect(d)
	{
	}

	tDisplay::~tDisplay()
	{
	}

	void tDisplay::draw(sf::RenderTarget& target) {
		tRenderRect::draw(target);
	}

	void tDisplay::handleEvent(tEvent& e) {
		if (checkOption(option_mask.can_be_drawn)) {
			tGroup::handleEvent(e);
			switch (e.type) {
			case tEvent::types.Broadcast: {
				if (e.address == (tRenderRect*)this) {
					switch (e.code) {
					case tEvent::codes.Deactivate: {	// Снять фокус с объекта
						((tObject*)e.from)->setOneOption(tObject::option_mask.is_active, false);
						tAbstractBasicClass::clearEvent(e);
						break;
					}
					case tEvent::codes.Show: {			// Показать объект (если он скрыт)
						((tObject*)e.from)->setOneOption(tObject::option_mask.can_be_drawn, true);
						tAbstractBasicClass::clearEvent(e);
						break;
					}
					case tEvent::codes.Hide: {			// Спрятать объект (если он не скрыт)
						((tObject*)e.from)->setOneOption(tObject::option_mask.can_be_drawn, false);
						tAbstractBasicClass::clearEvent(e);
						break;
					}
					case tEvent::codes.Adopt: {			// Стать владельцем объекта
						e.from->setOwner((tRenderRect*)this);
						tAbstractBasicClass::clearEvent(e);
						break;
					}
					case tEvent::codes.UpdateTexture: {	// Обновить текстуру
						tAbstractBasicClass::message(getOwner(), tEvent::types.Broadcast, tEvent::codes.UpdateTexture, (tRenderRect*)this);
						need_rerender = true;
						tAbstractBasicClass::clearEvent(e);
						break;
					}
					case tEvent::codes.StopAndDoNotMove: {	// Сбросить флаг перетаскивания мышью
						forEach(tEvent::codes.StopAndDoNotMove);
						// Не обнуляем событие
						break;
					}
					default: {				// Если не обработалось, то "проталкиваем" на уровень ниже
						e.address = getOwner();
						tAbstractBasicClass::message(e);
						tAbstractBasicClass::clearEvent(e);
						break;
					}
					}
				}
				break;
			}
			case tEvent::types.Button: {
				if (e.address == (tRenderRect*)this) {
					e.address = getOwner();
					tAbstractBasicClass::putEvent(e);
					tAbstractBasicClass::clearEvent(e);
				}
				break;
			}
			case tEvent::types.Mouse: {
				mouse_inside[1] = mouse_inside[0];
				mouse_inside[0] = pointIsInsideMe({ e.mouse.x, e.mouse.y });
			}
			}
		}
	}

	void tDisplay::updateTexture() {
		render_texture.clear(clear_color);	// Очиститься
		tGroup::draw(render_texture);		// Нарисовать подэлементы
		render_texture.display();			// Обновить "лицевую" текстуру
	}

	bool tDisplay::pointIsInsideMe(sf::Vector2i point) const {
		sf::FloatRect rect = tRenderRect::getGlobalBounds();
		return (point.x >= rect.left && point.x <= rect.left + rect.width && point.y >= rect.top && point.y <= rect.top + rect.height);
	}

	void tDisplay::setCameraOffset(const sf::Vector2f& new_offset) {
		render_squad[1].texCoords += new_offset - render_squad[0].texCoords;
		render_squad[2].texCoords += new_offset - render_squad[0].texCoords;
		render_squad[3].texCoords += new_offset - render_squad[0].texCoords;
		render_squad[0].texCoords = new_offset;
	}

	void tDisplay::setOwner(tAbstractBasicClass* new_owner) {
		owner = new_owner;
	}

	void tDisplay::setTextureSize(const sf::Vector2u& new_size) {
		render_texture.create(new_size.x, new_size.y);
		need_rerender = true;
	}

	tAbstractBasicClass* tDisplay::getOwner() const {
		return owner;
	}

	sf::FloatRect tDisplay::getLocalBounds() const {
		return tRenderRect::getLocalBounds();
	}

	nlohmann::json tDisplay::getParamsInJson() const {
		nlohmann::json js = tGroup::getParamsInJson();
		nlohmann::json js1 = tRenderRect::getParamsInJson();

		for (nlohmann::json::iterator it = js1.begin(); it != js1.end(); it++) {	// Прикручиваем содержимое js1 к js
			js[it.key()] = it.value();
		}

		js["what_is_it"] = objects_json_ids.tDisplay;
		js["what_is_it_string"] = "tDisplay";
		js["camera_offset"] = { render_squad[0].texCoords.x, render_squad[0].texCoords.y };

		return js;
	}
}