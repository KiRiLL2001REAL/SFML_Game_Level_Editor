#include "stdafx.h"
#include "interfaceEngine.h"

namespace edt
{
	tGroup::tGroup(tAbstractBasicClass* _owner) :
		tAbstractBasicClass(_owner),
		elem({})
	{
	}

	tGroup::tGroup(tAbstractBasicClass* _owner, nlohmann::json& js) :
		tAbstractBasicClass(_owner),
		elem({})
	{
		makeObjectsFromJson(_owner, js);
	}

	tGroup::tGroup(const tGroup& g) :
		tAbstractBasicClass(g),
		elem(g.elem)
	{
	}

	tGroup::~tGroup()
	{
		int i = elem.size();
		while (i != 0)
		{	// Удаление всех элементов в контейнере
			delete elem.back();
			elem.pop_back();
			i--;
		}
	}

	void tGroup::forEach(unsigned int code, tAbstractBasicClass* from)
	{
		if (!from)
		{	// Если не сказано от кого событие, то отправителем становится сам "почтальон"
			from = this;
		}
		for (std::list<tAbstractBasicClass*>::reverse_iterator it = elem.rbegin(); it != elem.rend(); it++)
		{
			message(*it, tEvent::types.Broadcast, code, from);
		}
	}

	void tGroup::makeObjectsFromJson(tAbstractBasicClass* _owner, nlohmann::json& js)
	{
		for (nlohmann::json::iterator it = js["elem"].begin(); it != js["elem"].end(); it++)
		{
			unsigned int what_is_it = (*it)["what_is_it"].get<unsigned char>();
			switch (what_is_it) {
			case objects_json_ids.tText:
			{
				tText* el = new tText(_owner, *it);
				_insert(el);
				break;
			}
			case objects_json_ids.tButton:
			{
				tButton* el = new tButton(_owner, *it);
				_insert(el);
				break;
			}
			case objects_json_ids.tRectShape:
			{
				tRectShape* el = new tRectShape(_owner, *it);
				_insert(el);
				break;
			}
			case objects_json_ids.tWindow:
			{
				tWindow* el = new tWindow(_owner, *it);
				_insert(el);
				break;
			}
			case objects_json_ids.tDisplay:
			{
				tDisplay* el = new tDisplay(_owner, *it);
				_insert((tRenderRect*)el);
				break;
			}
			}
		}
	}

	void tGroup::draw(sf::RenderTarget& target)
	{
		for (std::list<tAbstractBasicClass*>::iterator it = elem.begin(); it != elem.end(); it++)
		{
			(*it)->draw(target);
		}
	}

	void tGroup::handleEvent(tEvent& e)
	{
		for (std::list<tAbstractBasicClass*>::reverse_iterator it = elem.rbegin(); it != elem.rend(); it++)
		{
			(*it)->handleEvent(e);
		}
	}

	nlohmann::json tGroup::getParamsInJson() const
	{
		nlohmann::json js;
		unsigned int id = 0;
		for (auto& it : elem)
		{
			std::string str = std::to_string(id);
			js["elem"][str] = it->getParamsInJson();
			id++;
		}

		return js;
	}
	
	void tGroup::_insert(tAbstractBasicClass* object)
	{
		if (elem.size() != 0)
		{
			((tObject*)elem.back())->setOneOption(tObject::option_mask.is_active, false);
			message(elem.back(), tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
		}
		elem.push_back(object);
		((tObject*)elem.back())->setOneOption(tObject::option_mask.is_active, true);
		message(elem.back(), tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
	}

	bool tGroup::_delete(tAbstractBasicClass* object)
	{
		bool success = false;
		for (std::list<tAbstractBasicClass*>::iterator it = elem.begin(); it != elem.end(); it++)
		{	// Поиск удаляемого элемента
			if (*it == object)
			{
				success = true;
				bool act = false;
				if (*it == elem.back())
				{
					act = true;
				}
				delete *it;		// Удаляем его	1) из памяти
				elem.erase(it);	//				2) из контейнера
				((tObject*)elem.back())->setOneOption(tObject::option_mask.is_active, true);
				message(elem.back(), tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
				break;	// Вываливаемся из перебора, чтобы не поймать аксес виолэйшн
			}
		}
		return success;
	}

	void tGroup::select(tAbstractBasicClass* object)
	{
		for (std::list<tAbstractBasicClass*>::iterator it = elem.begin(); it != elem.end(); it++)
		{	// Пока не нашли, перебираем список
			if (*it == object)
			{
				tAbstractBasicClass* obj = *it;		// Запоминаем объект
				elem.erase(it);			// Удаляем из списка
				((tObject*)elem.back())->setOneOption(tObject::option_mask.is_active, false);
				message(elem.back(), tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
				elem.push_back(obj);	// Кидаем в конец списка
				break;
			}
		}
	}
}