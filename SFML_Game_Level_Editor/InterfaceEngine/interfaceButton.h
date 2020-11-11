#pragma once

namespace edt {

	class tText;

	class tButton : public tRenderRect {
	public:
		static const struct sOptionMask {	// ����� �������� (�������������� ��� tButton)
			static const unsigned char is_moving_by_mouse = 1;		// ������ ������������ ��� ������ ����
			static const unsigned char is_resizing_by_mouse = 2;	// ������ ������ ������ ��� ������ ����
			static const unsigned char can_be_drawn = 4;			// ����� �� �������� ���� ������ �� �����
			static const unsigned char is_active = 8;				// ������� �� ������
			static const unsigned char text_can_be_showed = 16;		// ����� ���� �������� �����
			static const unsigned char custom_skin_loaded = 32;		// �������� �� ��������� ����
			static const unsigned char can_be_resized = 64;			// ����� �� ������ ������ ������� ��� ������ ����
			static const unsigned char can_be_moved = 128;			// ����� �� ���������� ������ ��� ������ ����
		} option_mask;

		static const struct sTextAlignmentType {	// ��� �������� ������ � ������ (�� ����, �����)
			static const int Left = 0;		// �� ������ ����
			static const int Middle = 1;	// �� ������
			static const int Right = 2;		// �� ������� ����
		} text_alignment_type;

	protected:
		const unsigned char side_offset;

		int self_code;					// ���, ������� �������� ������ ��� ������� �� ��
		char alignment;					// ��� ������������
		sf::Texture custom_skin[2];		// ���������������� ���� ������ (������� � ����������)
		sf::Vector2i text_offset;		// ��������� �������� ������, � ������, ���� �� ����� ��������� (��� �� ��-�� �������)
		std::string path_to_skin[2];	// ���� �� ������ ���������� �����
		tText* text;					// ����� ������ ������

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