#pragma once

namespace edt {

	class tText;

	class tButton : public tRenderRect {
	public:
		static const struct sOptionMask {	// ����� ������ (�������������� ��� tButton)
			static const unsigned char can_be_drawn = 1;		// ����� �� �������� ���� ������ �� �����
			static const unsigned char is_active = 2;			// ������� �� ������
			static const unsigned char text_can_be_showed = 4;	// ����� ���� �������� �����
			static const unsigned char custom_skin_loaded = 8;	// �������� �� ��������� ����
			static const unsigned char dummy_4 = 16;			// ��� �� ������������
			static const unsigned char dummy_5 = 32;			// ��� �� ������������
			static const unsigned char dummy_6 = 64;			// ��� �� ������������
			static const unsigned char dummy_7 = 128;			// ��� �� ������������
		} option_mask;

		static const struct sTextOriginType {	// ��� �������� ������ � ������ (�� ����, �����)
			static const int Left = 0;		// �� ������ ����
			static const int Middle = 1;	// �� ������
			static const int Right = 2;		// �� ������� ����
		} text_origin_type;

	protected:
		const unsigned char side_offset;	// ������ ������ �� ���� ������

		int self_code;					// ���, ������� �������� ������ ��� ������� �� ��
		char text_origin;				// ������������ ������
		sf::Texture custom_skin[2];		// ���������������� ���� ������ (������� � ����������)
		sf::Vector2f text_offset;		// ��������� �������� ������, � ������, ���� �� ����� ��������� (��� �� ��-�� �������)
		std::string path_to_skin[2];	// ���� �� ������ ���������� �����
		tText* text;					// ����� ������ ������

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