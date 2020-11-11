#pragma once

namespace edt {
	class tText : public tObject {
	protected:
		sf::Text text_object;				// SFML �����
		sf::Font font;						// ����� ������
		bool font_loaded;					// ����. �������� �� �����?

	public:
		tText(tAbstractBasicClass* _owner, sf::Vector2f position = { 0, 0 }, std::wstring string = L"Some text");
		tText(tAbstractBasicClass* _owner, nlohmann::json& js);
		tText(const tText& t);
		virtual ~tText();

		virtual void draw(sf::RenderTarget& target);
		virtual void handleEvent(tEvent& e);
		virtual void move(sf::Vector2f delta);
		virtual void updateTexture();

		// Setters
		void setString(const std::wstring& new_string);
		void setTextColor(const sf::Color& new_color);
		void setFont(const sf::Font& new_font);
		void setCharSize(const unsigned int& new_char_size);
		void setOutlineThickness(const unsigned char& new_thickness);
		virtual void setPosition(const sf::Vector2f& new_position);

		// Getters
		bool getFontState() const;	// �������� ��� ���
		sf::Text& getTextObject() const;
		sf::Color getFillColor() const;
		virtual sf::FloatRect getLocalBounds() const;
		virtual bool pointIsInsideMe(sf::Vector2i point) const;
		virtual nlohmann::json getParamsInJson() const;
	};
}