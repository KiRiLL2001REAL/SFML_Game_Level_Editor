#pragma once

namespace edt {
	class tScrollbar : public tRenderRect {
	public:
		static const int thickness = 24;	// Ширина скролбара
		static const int default_step = 32;	// Шаг прокрутки стрелочками в пикселях

		static const struct sOptionMask {	// Маски операций (переопределено для scrollbar)
			static const unsigned char is_moving_by_mouse = 1;				// Объект перемещается при помощи мыши
			static const unsigned char is_resizing_by_mouse = 2;				// Объект меняет размер при помощи мыши
			static const unsigned char can_be_drawn = 4;			// Можно ли выводить этот объект на экран
			static const unsigned char is_active = 8;				// Активен ли объект
			static const unsigned char vectically_orientated = 16;	// Вертикальный ли scrollbar
			static const unsigned char dummy_2 = 32;				// Бит не задействован
			static const unsigned char can_be_resized = 64;			// Можно ли менять размер объекта при помощи мыши
			static const unsigned char can_be_moved = 128;			// Можно ли перемещать объект при помощи мыши
		} option_mask;

	private:
		void initScrollbar(bool vertical);

	protected:
		tButton* arrow_first, * arrow_second;	// Стрелки
		tRectShape* slider;						// Ползунок
		sf::Vector2f old_position;				// Предыдущая позиция ползунка
		sf::Color color_scroller_area;			// Цвет зоны, по которой бегает ползунок
		sf::Vector2f target_size;				// Размер целевого элемента
		sf::Vector2u target_texture_size;		// Размер текстуры целевого элемента

	public:
		tScrollbar(tAbstractBasicClass* _owner, bool vertical = true, sf::FloatRect rect = { 0, 0, thickness, thickness * 8 });
		tScrollbar(tAbstractBasicClass* _owner, nlohmann::json& js);
		tScrollbar(const tScrollbar& s);
		virtual ~tScrollbar();

		void updateScrollerSize();
		virtual void updateTexture();
		virtual void handleEvent(tEvent& e);
		virtual bool pointIsInsideMe(sf::Vector2i point) const;

		// Setters
		void moveSlider(const int& step);
		void setTargetSize(const sf::Vector2f& new_size);
		void setTargetTextureSize(const sf::Vector2u& new_size);
		virtual void setTextureSize(const sf::Vector2u& new_size);

		// Getters
		float getPixelOffset() const;
		virtual nlohmann::json getParamsInJson() const;
	};
}