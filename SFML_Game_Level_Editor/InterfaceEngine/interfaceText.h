#pragma once

namespace edt {
	class tText : public tObject {
	public:
		static const struct sOptionMask {	// Маски флагов (переопределено для tText)
			static const unsigned char can_be_drawn = 1;	// Можно ли выводить этот объект на экран
			static const unsigned char is_active = 2;		// Активен ли объект
			static const unsigned char is_font_loaded = 4;	// Загружен ли шрифт?
			static const unsigned char dummy_3 = 8;			// Бит не задействован
			static const unsigned char dummy_4 = 16;		// Бит не задействован
			static const unsigned char dummy_5 = 32;		// Бит не задействован
			static const unsigned char dummy_6 = 64;		// Бит не задействован
			static const unsigned char dummy_7 = 128;		// Бит не задействован
		} option_mask;

	protected:
		bool need_update_anchor;			// необходимо при загрузке из json (см. конструктор)
		sf::Text text_object;				// SFML текст
		sf::Font font;						// Шрифт текста

	public:
		tText(tAbstractBasicClass* _owner, sf::Vector2f position = { 0, 0 }, std::wstring string = L"Some text");
		tText(tAbstractBasicClass* _owner, nlohmann::json& js);
		tText(const tText& t);
		virtual ~tText();

		virtual void draw(sf::RenderTarget& target);
		virtual void handleEvent(tEvent& e);
		virtual void move(sf::Vector2f delta);
		virtual void updateTexture();

		// Setters
		void setStyle(const sf::Text::Style& new_style);
		void setAnchor(const unsigned char& new_anchor);
		void setString(const std::wstring& new_string);
		void setTextColor(const sf::Color& new_color);
		void setFont(const sf::Font& new_font);
		void setCharSize(const unsigned int& new_char_size);
		void setOutlineThickness(const unsigned char& new_thickness);
		virtual void setPosition(const sf::Vector2f& new_position);

		// Getters
		std::wstring& getString() const;
		bool getFontState() const;	// Загружен или нет
		sf::Text& getTextObject() const;
		sf::Color getFillColor() const;
		virtual sf::FloatRect getLocalBounds() const;
		virtual bool pointIsInsideMe(sf::Vector2i point) const;
		virtual nlohmann::json getParamsInJson() const;
	};
}