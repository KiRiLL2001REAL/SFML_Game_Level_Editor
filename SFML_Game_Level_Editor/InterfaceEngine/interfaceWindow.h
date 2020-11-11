#pragma once

namespace edt {

	class tDisplay;
	class tScrollbar;

	class tWindow : public tRenderRect {
	public:
		static const int heap_height = 32;

	private:
		static const unsigned char caption_char_size = 24;

		void initWindow();

	protected:
		bool font_loaded;	// Флаг. Загружен ли шрифт?
		sf::Font font;		// Шрифт

		std::wstring caption;					// Заголовок окна
		sf::Color color_heap;					// Цвет шапки
		sf::Color color_area;					// Цвет основной части
		sf::Color color_caption_active;			// Цвет заголовка активного окна
		sf::Color color_caption_inactive;		// Цвет заголовка неактивного окна
		sf::Vector2f caption_offset;			// Настройка смещения заголовка, в случае, если он криво выводится (это всё из-за шрифтов)

		tButton* button_close;					// Кнопка закрытия
		tRectShape* heap_shape;					// Фигура шапки
		tDisplay* display;						// Объект, в котором происходит отрисовка всех динамических подэлементов окна
		tScrollbar* scrollbar_v, * scrollbar_h;	// Ползунки

		sf::Vector2f last_scrollbar_offset;		// Предыдущее смещение ползунков

	public:
		tWindow(tAbstractBasicClass* _owner, sf::FloatRect rect = { 0, 0, 300, 300 }, std::wstring caption = L"Default caption");
		tWindow(tAbstractBasicClass* _owner, nlohmann::json& js);
		tWindow(const tWindow& w);
		virtual ~tWindow();

		bool pointIsInHeap(sf::Vector2i point) const;
		virtual void handleEvent(tEvent& e);
		virtual void updateTexture();
		virtual bool pointIsInsideMe(sf::Vector2i point) const;

		// Setters
		void setCaption(const std::wstring& new_caption);
		void setHeapColor(const sf::Color& new_color);
		void setAreaColor(const sf::Color& new_color);
		void setActiveCaptionColor(const sf::Color& new_color);
		void setInactiveCaptionColor(const sf::Color& new_color);
		void setFont(const sf::Font& new_font);
		void setCaptionOffset(const sf::Vector2f& new_offset);
		void setCameraOffset(const sf::Vector2f& new_offset);
		void setDisplaySize(const sf::Vector2f& new_size);
		void setDisplayTextureSize(const sf::Vector2u& new_size);

		// Getters
		tDisplay* getDisplayPointer() const;
		std::wstring getCaption() const;
		const int getHeapHeight() const;
		virtual nlohmann::json getParamsInJson() const;
	};
}