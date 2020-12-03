#include "stdafx.h"
#include "interfaceEngine.h"

namespace edt {
	tObject::tObject(tAbstractBasicClass* _owner) :
		tAbstractBasicClass(_owner),
		anchor(anchors.upper_left_corner),
		x(0),
		y(0),
		options(0)
	{
		setOneOption(option_mask.can_be_drawn, true);
		mouse_inside[0] = false;
		mouse_inside[1] = false;
	}

	tObject::tObject(tAbstractBasicClass* _owner, nlohmann::json& js) :
		tObject(_owner)
	{
		anchor = js["anchor"].get<unsigned char>();

		options = js["options"].get<unsigned char>();

		vec<float> vf = js["position"].get<vec<float>>();

		setPosition({ vf[0], vf[1] });
	}

	tObject::tObject(const tObject& o) :
		tAbstractBasicClass(o),
		anchor(o.anchor),
		x(o.x),
		y(o.y),
		options(o.options)
	{
		mouse_inside[0] = false;
		mouse_inside[1] = false;
	}

	tObject::~tObject() {
	}

	void tObject::move(sf::Vector2f deltaPos) {
		x += deltaPos.x;
		y += deltaPos.y;
	}

	void tObject::draw(sf::RenderTarget& target) {
		return;
	}

	bool tObject::pointIsInsideMe(sf::Vector2i point) const {
		return false;
	}

	void tObject::setAnchor(const unsigned char& new_anchor) {
		anchor = new_anchor;
	}

	void tObject::setOptions(const unsigned char& new_options) {
		options = new_options;
	}

	void tObject::setOneOption(const unsigned char& one_option, const bool& state) {
		if (state) {	// Если нужно взвести бит
			options |= one_option;	// Применяем результат побитового ИЛИ от "options" и параметра "one_option"
			return;
		}
		options &= ~one_option;	// Иначе применяем результат побитового И от "options" и инвертированного параметра "one_option"
	}

	void tObject::setPosition(const sf::Vector2f& new_position) {
		x = new_position.x;
		y = new_position.y;
	}

	void tObject::setSize(const sf::Vector2f& new_size) {
		return;
	}

	bool tObject::checkOption(unsigned char option) const {
		return (options & option) == option; // Если результат побитовой конъюнкции совпал с "option", то всё окей
	}

	sf::Vector2f tObject::getPosition() const {
		return sf::Vector2f(x, y);
	}

	unsigned char tObject::getOptions() const {
		return options;
	}

	unsigned char tObject::getAnchor() const {
		return anchor;
	}

	sf::Vector2f tObject::getRelativeStartPosition() const {
		unsigned char i = 0;
		sf::FloatRect owner_rect;
		sf::Vector2f offset = { 0, 0 };

		owner_rect = getOwner()->getLocalBounds();

		unsigned char anchor = this->anchor;
		for (i = 0; i < 3; i++) {
			if (anchor & 1) {
				offset.x = (owner_rect.width / 2) * i;
				break;
			}
			else anchor = anchor >> 1;
		}
		anchor = anchor >> (3 - i);
		for (i = 0; i < 3; i++) {
			if (anchor & 1) {
				offset.y = (owner_rect.height / 2) * i;
				break;
			}
			else anchor = anchor >> 1;
		}

		return offset;
	}

	nlohmann::json tObject::getParamsInJson() const {
		nlohmann::json js;

		js["position"] = { x, y };
		js["anchor"] = anchor;
		js["options"] = options;

		return js;
	}
}