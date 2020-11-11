#pragma once

namespace edt {
	class tGroup : public tAbstractBasicClass { // Класс-контейнер
	protected:
		std::list<tAbstractBasicClass*> elem;		// Контейнер элементов, хранящихся в данном классе

	public:
		tGroup(tAbstractBasicClass* _owner);
		tGroup(tAbstractBasicClass* _owner, nlohmann::json& js);
		tGroup(const tGroup& g);
		virtual ~tGroup();

		void forEach(unsigned int code, tAbstractBasicClass* from = nullptr);	// Выполнить команду для всех подэлементов
		void makeObjectsFromJson(tAbstractBasicClass* _owner, nlohmann::json& js);

		virtual void draw(sf::RenderTarget& target);
		virtual void handleEvent(tEvent& e);
		virtual void _insert(tAbstractBasicClass* object);		// Внесение элемента в список подэлементов
		virtual bool _delete(tAbstractBasicClass* object);		// Удаление элемента из списка
		virtual void select(tAbstractBasicClass* object);		// Установка флага "активен" у элемента

		// Getters
		virtual nlohmann::json getParamsInJson() const;
	};
}