#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>
#include "json.hpp"

#define list std::list

namespace edt {

	class tObject;

	struct sMovement { // ������������ ��� ����������� ������� �����
		bool active;		// �������� �� "����������" ������� ������� � ������� ����
		int mX, mY;			// �������� ���� ������������ �������� ������ ���� �������
	};

	struct tEvent {
		enum class types { Nothing, Mouse, Keyboard, Broadcast, Button };

		enum class codes {
			Activate, Deactivate, Show, Hide,
			Move, Adopt, Delete, Close, CloseApplication,
			MouseMoved, MouseButton, ResetButtons
		};

		unsigned int type = static_cast<int>(types::Nothing); // �� ����� ����� ������� (���)
		unsigned int code = 0; // ��� ������� (�����������)
		tObject *from = nullptr;
		tObject *address = nullptr;

		struct sMouse { // ������� �� ����
			char button = 0;
			char what_happened = 0;
			int x = 0;
			int y = 0;
		} mouse;
		struct sKey { // ������� �� ����������
			char button = 0;
			int what_happened = 0;
			bool control = false;
			bool alt = false;
			bool shift = false;
		} key;
		struct sText { // ����� ������
			std::string string = "";
			char char_size = 12;
			int x = 0;
			int y = 0;
			sf::Color color = sf::Color(255, 255, 255, 255);
		} text;
	};

	class tObject { // ������� �����
	protected:
		bool mouse_inside[2];	// ����. ��������� �� ���� ������ ������?

		float x, y;				// ���������� �������
		tObject *owner;			// ��������� �� ���������

		bool can_be_drawn;		// ����. ����� �� ������ ������ ���� ������������?
		bool is_active;			// ����. ������� �� ������ �������?
		bool can_be_moved;		// ����. ����� �� ������ ���� ���������?

		sMovement movement;		// ���������-�������� ��� ������� �������������� �����

	public:
		tObject();
		virtual ~tObject();

		sf::Vector2f getPosition();
		bool isActive();
		bool canMove();

		void message(tObject* addr, int type, int code, tObject* from);
		void setMovementStates(sMovement new_movement);
		void setOwner(tObject *new_owner);
		void clearEvent(tEvent& e);
		void setMoveAbility(bool can_move);

		virtual sMovement getMovementStates();
		virtual void move(sf::Vector2f delta);
		virtual void makeVisible(bool flag);
		virtual void makeActive(bool flag);
		virtual sf::Vector2i getCursorPos();
		virtual void putEvent(tEvent e);
		virtual void getEvent(tEvent& e);
		virtual void handleEvent(tEvent& e);
		virtual void draw(sf::RenderTarget& target);
		virtual void setPosition(sf::Vector2f new_position);
		virtual void setSize(sf::Vector2f new_size);

		virtual sf::FloatRect getLocalBounds() = 0;
		virtual sf::FloatRect getGlobalBounds();
	};

	class tGroup : public tObject { // �����-���������
	protected:
		list<tObject*> elem;		// ��������� ���������, ���������� � ������ ������

	public:
		tGroup();
		virtual ~tGroup();

		void _insert(tObject *object);		// �������� �������� � ������ ������������
		bool _delete(tObject *object);		// �������� �������� �� ������
		void select(tObject *object);		// ��������� ����� "�������" � ��������
		void forEach(unsigned int code, tObject* from);	// ��������� ������� ��� ���� ������������

		virtual void draw(sf::RenderTarget& target);
		virtual void handleEvent(tEvent& e);
		virtual void makeVisible(bool flag);
	};

	class tRenderRect : public tGroup {
	protected:
		sf::VertexArray render_squad;		// ������, � ������� ����������� ���������
		sf::RenderTexture render_texture;	// ��������, � ������� �������������� �������
		sf::Color clear_color;				// ���� ������� ��������

	public:
		tRenderRect(sf::FloatRect rect = { 0, 0, 64, 64 });
		virtual ~tRenderRect();

		void setTextureSize(sf::Vector2u new_size);
		void setClearColor(sf::Color new_color);

		virtual void handleEvent(tEvent& e);
		virtual void setSize(sf::Vector2f new_size);
		virtual void setPosition(sf::Vector2f new_position);
		virtual void draw(sf::RenderTarget& target);

		virtual sf::FloatRect getLocalBounds();
	};

	class tRectShape : public tObject {
	protected:
		sf::RectangleShape shape;

	public:
		tRectShape(sf::FloatRect rect = {0, 0, 64, 64}, sf::Color fill_color = sf::Color(255, 255, 255, 255));
		virtual ~tRectShape();

		void setColor(sf::Color new_color);

		virtual void draw(sf::RenderTarget& target);
		virtual void setPosition(sf::Vector2f new_position);
		virtual void setSize(sf::Vector2f new_size);

		virtual sf::FloatRect getLocalBounds();
	};

	class tDesktop : public tGroup {
	protected:		
		std::string path_to_folder;	// ���� �� ������� �����
		sf::RenderWindow window;	// ������� ����
		sf::Font font_default;		// ����� �� ���������
		sf::Font custom_font;		// ���������������� �����
		bool custom_font_loaded;	// ����. �������� �� ���������������� �����?
		list<tEvent> events;		// ������ ������� � ���������

		char screen_code;			// ��� �������� ������
	
	public:
		tDesktop(std::string path_to_folder);
		~tDesktop();

		void run();								// ������� ����
		bool windowIsOpen();					// ���������� ������ ����
		void saveData();						// ��������� ������ � ����
		void loadCustomFont(std::string path_to_font);	// ���������� ���������������� �����
		sf::Font& getFont();					// �������� �����

		virtual void changeScreen(char new_screen_code);// �������� ����� (����, �������� �����, �������� NPC)
		virtual void putEvent(tEvent e);
		virtual void getEvent(tEvent& e);
		virtual void handleEvent(tEvent& e);
	};

	class tText : public tObject {
	protected:
		sf::Text text_object;				// SFML �����
		std::string string;					// ����� ���������
		sf::Color text_color;				// ���� �����
		sf::Font font;						// ����� ������
		bool font_loaded;					// ����. �������� �� �����?
		unsigned int char_size;				// ������ ������
		unsigned char outline_thickness;	// ������� ����� ������� ������

	public:
		tText(sf::Vector2f position = {0, 0}, std::string string = "Some text");
		virtual ~tText();

		void setString(std::string new_string);
		void setTextColor(sf::Color new_color);
		void setFont(sf::Font new_font);
		void setCharSize(unsigned int new_char_size);
		void setOutlineThickness(unsigned char new_thickness);
		void updateTextObject();	// �� ������������ � ������ tButton

		virtual sf::FloatRect getLocalBounds();

		virtual void draw(sf::RenderTarget& target);
		virtual void setPosition(sf::Vector2f new_position);
	};

	class tButton : public tText {		
	protected:
		const unsigned char side_offset;

		int self_code;						// ���, ������� �������� ������ ��� ������� �� ��

		sf::RenderTexture render_texture;	// ��������
		sf::VertexArray render_squad;		// ������������� ���������
		bool custom_skin_loaded;			// ����. �������� �� ���������������� ���� ������?
		sf::Texture custom_skin;			// ���������������� ���� ������
		char alignment;						// ��� ������������
		sf::Vector2i text_offset;			// ��������� �������� ������, � ������, ���� �� ����� ��������� (��� �� ��-�� �������)
		
	public:
		enum class alignment_type { Left, Middle, Right };

		tButton(sf::FloatRect rect = { 0, 0, 128, 48 }, std::string text = "button");
		virtual ~tButton();

		void setSize(sf::Vector2f new_size);
		void setTextureSize(sf::Vector2u new_size);
		void updateTexture();
		void loadCustomSkin(std::string path_to_skin);
		void setAlignment(char new_alignment);
		void setTextOffset(sf::Vector2i new_offset);
		void setCode(int new_code);

		bool pointIsInsideMe(sf::Vector2i point);
		virtual sf::FloatRect getLocalBounds();

		virtual void draw(sf::RenderTarget& target);
		virtual void setPosition(sf::Vector2f new_position);
		virtual void handleEvent(tEvent& e);
	};

	class tWindow : public tRenderRect {
	private:
		static const int heap_height = 32;
		static const unsigned char caption_char_size = 24;

	protected:
		bool font_loaded;	// ����. �������� �� �����?
		sf::Font font;		// �����

		std::string caption;				// ��������� ����
		sf::Color color_heap;				// ���� �����
		sf::Color color_space;				// ���� �������� �����
		sf::Color color_caption_active;		// ���� ��������� ��������� ����
		sf::Color color_caption_inactive;	// ���� ��������� ����������� ����
		sf::Vector2f caption_offset;		// ��������� �������� ���������, � ������, ���� �� ����� ��������� (��� �� ��-�� �������)

	public:
		tWindow(sf::FloatRect rect = { 0, 0, 300, 300 }, std::string caption = "Default caption" );	// ���������� ��� �� ������� ����� initWindow()
		virtual ~tWindow();

		void initWindow();	// ����������� � ���������� ����� ������ ������������

		void setCaption(std::string new_caption);
		std::string getCaption();
		void setColorHeap(sf::Color new_color);
		void setColorSpace(sf::Color new_color);
		void setColorCaptionActive(sf::Color new_color);
		void setColorCaptionInactive(sf::Color new_color);
		void setFont(sf::Font new_font);
		void setCaptionOffset(sf::Vector2f new_offset);

		virtual sf::FloatRect getLocalBounds();
		const int getHeapHeight();

		virtual void draw(sf::RenderTarget& target);
		virtual void handleEvent(tEvent& e);
	};
}