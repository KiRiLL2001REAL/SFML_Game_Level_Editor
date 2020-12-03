#pragma once

namespace edt {
	class tObject : public tAbstractBasicClass { //  ласс объекта
	public:
		static const struct sOptionMask {	// ћаски операций
			static const unsigned char can_be_drawn = 1;	// ћожно ли выводить этот объект на экран
			static const unsigned char is_active = 2;		// јктивен ли объект
			static const unsigned char dummy_2 = 4;			// Ѕит не задействован
			static const unsigned char dummy_3 = 8;			// Ѕит не задействован
			static const unsigned char dummy_4 = 16;		// Ѕит не задействован
			static const unsigned char dummy_5 = 32;		// Ѕит не задействован
			static const unsigned char dummy_6 = 64;		// Ѕит не задействован
			static const unsigned char dummy_7 = 128;		// Ѕит не задействован
		} option_mask;

		static const struct sAnchors {		// ¬севозможные €кори
			static const unsigned char upper_left_corner = 0b00001001;		// якорь на верхний левый угол родител€
			static const unsigned char upper_side = 0b00001010;				// якорь на верхнюю сторону родител€
			static const unsigned char upper_right_corner = 0b00001100;		// якорь на верхний правый угол	родител€
			static const unsigned char left_side = 0b00010001;				// якорь на левую сторону родител€
			static const unsigned char center = 0b00010010;					// якорь на центр родител€
			static const unsigned char right_side = 0b00010100;				// якорь на правую сторону родител€
			static const unsigned char bottom_left_corner = 0b00100001;		// якорь на нижний левый угол родител€
			static const unsigned char bottom_side = 0b00100010;			// якорь на нижнюю сторону родител€
			static const unsigned char bottom_right_corner = 0b00100100;	// якорь на нижний правый угол родител€
		} anchors;

	protected:
		unsigned char anchor;	// якорь прив€зки к родительскому элементу (см. anchors)

		bool mouse_inside[2];	// ‘лаг. “екущее и предыдущее состо€ние. Ќаходитс€ ли мышь внутри объекта?

		float x, y;				// —мещение относительно начальной точки (см. anchor)

		unsigned char options;	// Ѕитовые флаги. —мотри "tObject::option_mask"

	public:
		tObject(tAbstractBasicClass* _owner);
		tObject(tAbstractBasicClass* _owner, nlohmann::json& js);
		tObject(const tObject& o);
		virtual ~tObject();

		virtual void move(sf::Vector2f delta);
		virtual void draw(sf::RenderTarget& target);
		virtual void updateTexture() = 0;
		virtual bool pointIsInsideMe(sf::Vector2i point) const;

		// Setters
		void setAnchor(const unsigned char& new_anchor);
		void setOptions(const unsigned char& new_options);
		void setOneOption(const unsigned char& one_option, const bool& state);
		virtual void setPosition(const sf::Vector2f& new_position);
		virtual void setSize(const sf::Vector2f& new_size);

		// Getters
		bool checkOption(unsigned char option) const;
		sf::Vector2f getPosition() const;
		unsigned char getOptions() const;
		unsigned char getAnchor() const;
		virtual sf::Vector2f getRelativeStartPosition() const;	// ¬озвращает начальную точку системы координат (ориентиру€сь на €корь)
		virtual nlohmann::json getParamsInJson() const;
	};
}