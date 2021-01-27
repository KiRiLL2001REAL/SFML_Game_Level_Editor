#include "stdafx.h"
#include "interfaceEngine.h"

namespace edt
{
	tAbstractBasicClass::tAbstractBasicClass(tAbstractBasicClass* _owner) :
		owner(_owner)
	{
	}

	tAbstractBasicClass::tAbstractBasicClass(const tAbstractBasicClass& a) :
		owner(a.owner)
	{
	}

	tAbstractBasicClass::~tAbstractBasicClass()
	{
	}

	void tAbstractBasicClass::clearEvent(tEvent& e)
	{
		e.type = tEvent::types.Nothing;
	}

	void tAbstractBasicClass::message(tAbstractBasicClass* addr, unsigned int type, unsigned int code, tAbstractBasicClass* from)
	{
		tEvent e;
		e.address = addr;
		e.from = from;
		e.type = type; // Из какой сферы событие
		e.code = code; // Код события
		if (addr) addr->handleEvent(e);
		else putEvent(e);
	}

	void tAbstractBasicClass::message(tEvent e)
	{
		if (e.address) e.address->handleEvent(e);
		else putEvent(e);
	}

	void tAbstractBasicClass::putEvent(tEvent e)
	{
		if (owner != nullptr)
			owner->putEvent(e);
	}

	void tAbstractBasicClass::getEvent(tEvent& e)
	{
		return;
	}

	void tAbstractBasicClass::setOwner(tAbstractBasicClass* new_owner)
	{
		owner = new_owner;
	}

	tAbstractBasicClass* tAbstractBasicClass::getOwner() const
	{
		return owner;
	}

	nlohmann::json tAbstractBasicClass::getParamsInJson() const
	{
		return nlohmann::json();
	}

	const sf::FloatRect tAbstractBasicClass::getLocalBounds() const
	{
		return sf::FloatRect(
			0.f,
			0.f,
			1.f,
			1.f
		);
	}

	const sf::FloatRect tAbstractBasicClass::getGlobalBounds() const
	{
		if (owner != nullptr) {
			sf::FloatRect owner_rect = getOwner()->getGlobalBounds();
			sf::FloatRect local_rect = getLocalBounds();

			return {
				owner_rect.left + local_rect.left,
				owner_rect.top + local_rect.top,
				local_rect.width,
				local_rect.height
			};
		}
		return sf::FloatRect(0.f, 0.f, 0.f, 0.f);
	}
}