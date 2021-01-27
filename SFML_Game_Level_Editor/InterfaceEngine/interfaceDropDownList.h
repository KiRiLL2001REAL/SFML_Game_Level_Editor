#pragma once

namespace edt {

	class tDropDownVariant : public tButton {
	public:
		tDropDownVariant(tAbstractBasicClass* _owner, std::wstring text, sf::FloatRect rect = { 0, 0, 128, 48 });
		tDropDownVariant(tAbstractBasicClass* _owner, nlohmann::json& js);
		tDropDownVariant(const tDropDownVariant& d);
		virtual ~tDropDownVariant();

		virtual void updateTexture();

		//Getters
		const int& getCode() const;
		const std::wstring& getString() const;
		virtual nlohmann::json getParamsInJson() const;
	};

	class tDropDownWindow : public tDisplay {
	protected:
		tScrollbar* scrollbar_v;	// Вертикальный скроллбар

	public:
		tDropDownWindow(tAbstractBasicClass* _owner, sf::FloatRect rect = { 0, 0, 100, 100 });
		tDropDownWindow(tAbstractBasicClass* _owner, nlohmann::json& js);
		tDropDownWindow(const tDropDownWindow& d);
		virtual ~tDropDownWindow();

		virtual void handleEvent(tEvent& e);
		virtual void updateTexture();

		// Setters
		virtual void setSize(const sf::Vector2f& size);

		// Getters
		std::list<tDropDownVariant*>* getVariantList();
		virtual nlohmann::json getParamsInJson() const;
	};

	class tDropDownList : public tDropDownVariant {
	public:
		static const struct sDirectionTypes {	// В каком направлении будет развёртываться список
			static const unsigned int Down	= 0;	// Вниз
			static const unsigned int Up	= 1;	// Вверх
		} direction_types;
	
	protected:
		int direction;				// Направление развёртывания
		int selected_variant_code;	// Выбранный вариант. Если -1, то ничего не выбрано
		tDropDownWindow *ddwindow;	// Окно с вариантами выбора

	public:
		tDropDownList(tAbstractBasicClass* _owner, sf::FloatRect rect = { 0, 0, 128, 48 }, unsigned int _direction = direction_types.Down);
		tDropDownList(tAbstractBasicClass* _owner, nlohmann::json& js);
		tDropDownList(const tDropDownList &d);
		virtual ~tDropDownList();

		virtual void handleEvent(tEvent& e);
		virtual void updateTexture();
		virtual void draw(sf::RenderTarget& target);
		void insertVariant(const std::wstring& variant_text, const int& variant_code, const float& height, const sf::Vector2f& text_offset = { 0.f, 0.f });

		bool deleteVariant(const std::wstring& variant_text);
		bool deleteVariant(const int& variant_code);

		// Setters
		void setDropDownWindowSize(const sf::Vector2f& size);
		void setDirection(const unsigned int &new_direction);

		// Getters
		unsigned int getSelectedVariantCode() const;
		virtual nlohmann::json getParamsInJson() const;

	protected:
		//Setters
		void setSelectedVariantCode(const int& new_selected_variant_code);
	};
}