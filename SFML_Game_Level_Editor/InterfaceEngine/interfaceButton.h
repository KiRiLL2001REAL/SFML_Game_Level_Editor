#pragma once

namespace edt {

	class tText;

	class tButton : public tRenderRect {
	public:
		static const struct sOptionMask {	// Маски операций (переопределено для tButton)
			static const unsigned char is_moving_by_mouse = 1;		// Объект перемещается при помощи мыши
			static const unsigned char is_resizing_by_mouse = 2;	// Объект меняет размер при помощи мыши
			static const unsigned char can_be_drawn = 4;			// Можно ли выводить этот объект на экран
			static const unsigned char is_active = 8;				// Активен ли объект
			static const unsigned char text_can_be_showed = 16;		// Может быть отображён текст
			static const unsigned char custom_skin_loaded = 32;		// Загружен ли кастомный скин
			static const unsigned char can_be_resized = 64;			// Можно ли менять размер объекта при помощи мыши
			static const unsigned char can_be_moved = 128;			// Можно ли перемещать объект при помощи мыши
		} option_mask;

		static const struct sTextAlignmentType {	// Тип привязки текста к кнопке (по сути, якорь)
			static const int Left = 0;		// По левому краю
			static const int Middle = 1;	// По центру
			static const int Right = 2;		// По правому краю
		} text_alignment_type;

	protected:
		const unsigned char side_offset;

		int self_code;					// Код, который посылает кнопка при нажатии на неё
		char alignment;					// Тип выравнивания
		sf::Texture custom_skin[2];		// Пользовательский скин кпопки (обычный и выделенный)
		sf::Vector2i text_offset;		// Настройка смещения текста, в случае, если он криво выводится (это всё из-за шрифтов)
		std::string path_to_skin[2];	// Путь до частей кастомного скина
		tText* text;					// Текст внутри кнопки

	public:
		tButton(tAbstractBasicClass* _owner, sf::FloatRect rect = { 0, 0, 128, 48 });
		tButton(tAbstractBasicClass* _owner, nlohmann::json& js);
		tButton(const tButton& b);
		virtual ~tButton();

		virtual void handleEvent(tEvent& e);
		virtual void updateTexture();
		virtual bool pointIsInsideMe(sf::Vector2i point) const;

		// Setters
		void loadCustomSkin(const std::string& path_to_skin, const int& cell);
		void setTextAlignment(const char& new_alignment);
		void setTextOffset(const sf::Vector2i& new_offset);
		void setCode(const int& new_code);
		void setFont(const sf::Font& new_font);
		void setString(const std::wstring& new_string);
		void setTextColor(const sf::Color& new_color);
		void setCharSize(const unsigned int& new_char_size);
		void setOutlineThickness(const unsigned char& new_thickness);

		// Getters
		virtual nlohmann::json getParamsInJson() const;
	};


}