#pragma once

namespace edt {
	class tObject : public tAbstractBasicClass { // ����� �������
	public:
		static const struct sOptionMask {	// ����� ��������
			static const unsigned char is_moving_by_mouse = 1;		// ������ ������������ ��� ������ ����
			static const unsigned char is_resizing_by_mouse = 2;		// ������ ������ ������ ��� ������ ����
			static const unsigned char can_be_drawn = 4;	// ����� �� �������� ���� ������ �� �����
			static const unsigned char is_active = 8;		// ������� �� ������
			static const unsigned char dummy_1 = 16;		// ��� �� ������������
			static const unsigned char dummy_2 = 32;		// ��� �� ������������
			static const unsigned char can_be_resized = 64;	// ����� �� ������ ������ ������� ��� ������ ����
			static const unsigned char can_be_moved = 128;	// ����� �� ���������� ������ ��� ������ ����
		} option_mask;

		static const struct sAnchors {		// ������������ �����
			static const unsigned char upper_left_corner = 0b00001001;	// ����� �� ������� ����� ���� ��������
			static const unsigned char upper_side = 0b00001010; // ����� �� ������� ������� ��������
			static const unsigned char upper_right_corner = 0b00001100; // ����� �� ������� ������ ����	��������
			static const unsigned char left_side = 0b00010001; // ����� �� ����� ������� ��������
			static const unsigned char center = 0b00010010; // ����� �� ����� ��������
			static const unsigned char right_side = 0b00010100; // ����� �� ������ ������� ��������
			static const unsigned char bottom_left_corner = 0b00100001; // ����� �� ������ ����� ���� ��������
			static const unsigned char bottom_side = 0b00100010; // ����� �� ������ ������� ��������
			static const unsigned char bottom_right_corner = 0b00100100; // ����� �� ������ ������ ���� ��������
		} anchors;

	protected:
		unsigned char anchor;	// ���� 0..2 �������� �� �������� �� X, � ���� 3..5 - �� �������� �� Y (�� tObject::anchors)

		bool mouse_inside[2];	// ����. ��������� �� ���� ������ ������?

		float x, y;				// �������� ������������ ��������� ����� (��. anchor)

		unsigned char options;	// ������� �����. ������ "tObject::option_mask"

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
		virtual sf::Vector2f getRelativeStartPosition() const;	// ���������� ��������� ����� ������� ��������� (������������ �� �����)
		virtual nlohmann::json getParamsInJson() const;
	};
}