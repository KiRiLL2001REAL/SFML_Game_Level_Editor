#pragma once

namespace edt {
	class tAbstractBasicClass {
	protected:
		tAbstractBasicClass* owner;

	public:
		tAbstractBasicClass(tAbstractBasicClass* _owner);
		tAbstractBasicClass(const tAbstractBasicClass& a);
		virtual ~tAbstractBasicClass();

		void clearEvent(tEvent& e);
		void message(tAbstractBasicClass* addr, unsigned int type, unsigned int code, tAbstractBasicClass* from);
		void message(tEvent e);

		virtual void putEvent(tEvent e);
		virtual void getEvent(tEvent& e);
		virtual void handleEvent(tEvent& e) = 0;
		virtual void draw(sf::RenderTarget& target) = 0;

		// Setters
		virtual void setOwner(tAbstractBasicClass* new_owner);

		// Getters
		virtual tAbstractBasicClass* getOwner() const;
		virtual nlohmann::json getParamsInJson() const;
		virtual const sf::FloatRect getLocalBounds() const;
		virtual const sf::FloatRect getGlobalBounds() const;
	};
}