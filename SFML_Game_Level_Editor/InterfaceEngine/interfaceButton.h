#pragma once

namespace edt {

	class tText;

	class tButton : public tRenderRect {
	public:
		static const struct sOptionMask {	// Маски флагов (переопределено для tButton)
			static const unsigned char can_be_drawn = 1;		// Можно ли выводить этот объект на экран
			static const unsigned char is_active = 2;			// Активен ли объект
			static const unsigned char text_can_be_showed = 4;	// Может быть отображён текст
			static const unsigned char custom_skin_loaded = 8;	// Загружен ли кастомный скин
			static const unsigned char dummy_4 = 16;			// Бит не задействован
			static const unsigned char dummy_5 = 32;			// Бит не задействован
			static const unsigned char dummy_6 = 64;			// Бит не задействован
			static const unsigned char dummy_7 = 128;			// Бит не задействован
		} option_mask;

		static const struct sTextOriginType {	// Тип привязки текста к кнопке (по сути, якорь)
			static const int Left = 0;		// По левому краю
			static const int Middle = 1;	// По центру
			static const int Right = 2;		// По правому краю
		} text_origin_type;

	protected:
		const unsigned char side_offset;	// Отступ текста от края кнопки

		int self_code;					// Код, который посылает кнопка при нажатии на неё
		char text_origin;				// Выравнивание текста
		sf::Texture custom_skin[2];		// Пользовательский скин кпопки (обычный и выделенный)
		sf::Vector2f text_offset;		// Настройка смещения текста, в случае, если он криво выводится (это всё из-за шрифтов)
		std::string path_to_skin[2];	// Путь до частей кастомного скина
		tText* text;					// Текст внутри кнопки

	public:
		tButton(tAbstractBasicClass* _owner, sf::FloatRect rect = { 0, 0, 128, 48 });
		tButton(tAbstractBasicClass* _owner, nlohmann::json& js);
		tButton(const tButton& b);
		virtual ~tButton();

		virtual void handleEvent(tEvent& e);
		virtual void updateTexture();

		// Setters
		void loadCustomSkin(const std::string& path_to_skin, const int& cell);
		void setTextOrigin(const char& new_origin);
		void setTextOffset(const sf::Vector2f& new_offset);
		void setCode(const int& new_code);
		void setFont(const sf::Font& new_font);
		void setString(const std::wstring& new_string);
		void setTextColor(const sf::Color& new_color);
		void setCharSize(const unsigned int& new_char_size);
		void setOutlineThickness(const unsigned char& new_thickness);

		// Getters
		virtual const bool pointIsInsideMe(sf::Vector2i point) const;
		virtual nlohmann::json getParamsInJson() const;
	};


}