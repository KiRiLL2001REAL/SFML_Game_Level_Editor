#pragma once

namespace edt {
	class tDesktop : public tGroup {
	protected:
		std::string path_to_folder;			// ���� �� ������� �����
		sf::RenderWindow window;			// ������� ����
		sf::Font font_default;				// ����� �� ���������
		sf::Font custom_font;				// ���������������� �����
		bool custom_font_loaded;			// ����. �������� �� ���������������� �����?
		std::list<tEvent> events;			// ������ ������� � ���������
		sf::RectangleShape background;		// ������� �������������
		nlohmann::json json_configuration;	// ������-���� ���������, ���������� ����������� ��������� ������� ���������
		sf::Vector2i old_mouse_position;	// ���������� ��������� ���� (��������� ��� ������ tEvent::mouse.dX(.dY))
		char screen_code;					// ��� �������� ������
		sf::Vector2u window_size;			// ������ ����

	public:
		tDesktop(std::string path_to_folder, unsigned char _screen_code);
		virtual ~tDesktop();

		void run();								// ������� ����
		void saveData() const;						// ��������� ������ � ����
		void loadCustomFont(std::string path_to_font);	// ���������� ���������������� �����
		void completeEvents();					// ��������� �������
		bool windowIsOpen() const;					// ���������� ������ ����
		virtual void changeScreen(char new_screen_code);// �������� ����� (����, �������� �����, �������� NPC � �.�.)
		virtual void putEvent(tEvent e);
		virtual void getEvent(tEvent& e);
		virtual void handleEvent(tEvent& e);
		virtual void updateTexture();
		virtual void draw(sf::RenderTarget& target);

		// Getters
		sf::Font& getFont() const;					// �������� �����
		virtual sf::FloatRect getLocalBounds() const;
		virtual bool pointIsInsideMe(sf::Vector2i point) const;
		virtual nlohmann::json getParamsInJson() const;
	};
}