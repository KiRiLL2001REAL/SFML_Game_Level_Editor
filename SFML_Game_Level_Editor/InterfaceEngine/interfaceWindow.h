#pragma once

namespace edt {

	class tDisplay;
	class tScrollbar;

	class tWindow : public tRenderRect {
	public:
		static const int heap_height = 32;
		static const struct sOptionMask {	// ����� ������ (�������������� ��� tWindow)
			static const unsigned char can_be_drawn = 1;			// ����� �� �������� ���� ������ �� �����
			static const unsigned char is_active = 2;				// ������� �� ������
			static const unsigned char can_be_moved = 4;			// ����� �� ���������� ������ ��� ������ ����
			static const unsigned char is_moving_by_mouse = 8;		// ������ ������������ ��� ������ ����
			static const unsigned char can_be_resized = 16;			// ����� �� ������ ������ ������� ��� ������ ����
			static const unsigned char is_resizing_by_mouse = 32;	// ������ ������ ������ ��� ������ ����
			static const unsigned char dummy_6 = 64;				// ��� �� ������������
			static const unsigned char dummy_7 = 128;				// ��� �� ������������
		} option_mask;

	private:
		static const unsigned char caption_char_size = 24;

		void initWindow();

	protected:
		bool font_loaded;	// ����. �������� �� �����?
		sf::Font font;		// �����

		std::wstring caption;					// ��������� ����
		sf::Color color_heap;					// ���� �����
		sf::Color color_area;					// ���� �������� �����
		sf::Color color_caption_active;			// ���� ��������� ��������� ����
		sf::Color color_caption_inactive;		// ���� ��������� ����������� ����
		sf::Vector2f caption_offset;			// ��������� �������� ���������, � ������, ���� �� ����� ��������� (��� �� ��-�� �������)

		tButton* button_close;					// ������ ��������
		tRectShape* heap_shape;					// ������ �����
		tDisplay* display;						// ������, � ������� ���������� ��������� ���� ������������ ������������ ����
		tScrollbar* scrollbar_v, * scrollbar_h;	// ��������

		sf::Vector2f last_scrollbar_offset;		// ���������� �������� ���������

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