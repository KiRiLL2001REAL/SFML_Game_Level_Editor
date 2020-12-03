#pragma once

namespace edt {
	class tRectShape : public tObject {
	public:
		static const struct sOptionMask {	// Маски операций (переопределено для tRectShape)
			static const unsigned char can_be_drawn = 1;		// Можно ли выводить этот объект на экран
			static const unsigned char is_active = 2;			// Активен ли объект
			static const unsigned char can_be_moved = 4;		// Можно ли перемещать объект при помощи мыши
			static const unsigned char is_moving_by_mouse = 8;	// Объект перемещается при помощи мыши
			static const unsigned char dummy_4 = 16;			// Бит не задействован
			static const unsigned char dummy_5 = 32;			// Бит не задействован
			static const unsigned char dummy_6 = 64;			// Бит не задействован
			static const unsigned char dummy_7 = 128;			// Бит не задействован
		} option_mask;

	protected:
		sf::RectangleShape shape;

	public:
		tRectShape(tAbstractBasicClass* _owner, sf::FloatRect rect = { 0, 0, 64, 64 }, sf::Color fill_color = sf::Color(255, 255, 255, 255));
		tRectShape(tAbstractBasicClass* _owner, nlohmann::json& js);
		tRectShape(const tRectShape& s);
		virtual ~tRectShape();

		virtual void draw(sf::RenderTarget& target);
		virtual void move(sf::Vector2f delta);
		virtual void updateTexture();
		virtual void handleEvent(tEvent& e);
		virtual bool pointIsInsideMe(sf::Vector2i point) const;

		// Setters
		void setColor(const sf::Color& new_color);
		virtual void setPosition(const sf::Vector2f& new_position);
		virtual void setSize(const sf::Vector2f& new_size);

		// Getters
		virtual sf::FloatRect getLocalBounds() const;
		virtual nlohmann::json getParamsInJson() const;
	};
}