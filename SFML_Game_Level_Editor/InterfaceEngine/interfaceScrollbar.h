#pragma once

namespace edt {
	class tScrollbar : public tRenderRect {
	public:
		static const int thickness = 24;	// ������ ���������
		static const int default_step = 32;	// ��� ��������� ����������� � ��������

		static const struct sOptionMask {	// ����� �������� (�������������� ��� scrollbar)
			static const unsigned char is_moving_by_mouse = 1;				// ������ ������������ ��� ������ ����
			static const unsigned char is_resizing_by_mouse = 2;				// ������ ������ ������ ��� ������ ����
			static const unsigned char can_be_drawn = 4;			// ����� �� �������� ���� ������ �� �����
			static const unsigned char is_active = 8;				// ������� �� ������
			static const unsigned char vectically_orientated = 16;	// ������������ �� scrollbar
			static const unsigned char dummy_2 = 32;				// ��� �� ������������
			static const unsigned char can_be_resized = 64;			// ����� �� ������ ������ ������� ��� ������ ����
			static const unsigned char can_be_moved = 128;			// ����� �� ���������� ������ ��� ������ ����
		} option_mask;

	private:
		void initScrollbar(bool vertical);

	protected:
		tButton* arrow_first, * arrow_second;	// �������
		tRectShape* slider;						// ��������
		sf::Vector2f old_position;				// ���������� ������� ��������
		sf::Color color_scroller_area;			// ���� ����, �� ������� ������ ��������
		sf::Vector2f target_size;				// ������ �������� ��������
		sf::Vector2u target_texture_size;		// ������ �������� �������� ��������

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