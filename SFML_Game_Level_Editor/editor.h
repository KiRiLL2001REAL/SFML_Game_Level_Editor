#pragma once

#define list std::list

namespace edt {

	enum class objects_json_ids {
		tRectShape, tRenderRect, tText, tWindow, tButton
	};

	class tObject;

	struct tEvent {

		enum class types { Nothing, Mouse, Keyboard, Broadcast, Button };

		enum class codes {
			Nothing, Activate, Deactivate, Show, Hide,
			Move, Adopt, Delete, Close, CloseApplication,
			MouseMoved, MouseButton, ResetButtons, UpdateTexture,
			FontRequest, FontAnswer
		};

		unsigned int type = static_cast<int>(types::Nothing); // �� ����� ����� ������� (���)
		unsigned int code = static_cast<int>(codes::Nothing); // ��� �������
		tObject *from = nullptr;
		tObject *address = nullptr;

		struct sMouse { // ������� �� ����
			char button = 0;
			char what_happened = 0;
			int x = 0;
			int y = 0;
			int dX = 0;
			int dY = 0;
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
		struct sFont {	// ������ ������
			sf::Font* font = nullptr;
		} font;
	};

	class tObject { // ������� �����
	protected:
		static const struct sOptionMask {	// ����� ��������
			static const unsigned char is_moving = 1;		// ������ ������������ ��� ������ ����
			static const unsigned char is_resizing = 2;		// ������ ������ ������ ��� ������ ����
			static const unsigned char can_be_drawn = 4;	// ����� �� �������� ���� ������ �� �����
			static const unsigned char is_active = 8;		// ������� �� ������
			static const unsigned char dummy_1 = 16;		// ��� �� ������������
			static const unsigned char dummy_2 = 32;		// ��� �� ������������
			static const unsigned char can_be_resized = 64;	// ����� �� ������ ������ ������� ��� ������ ����
			static const unsigned char can_be_moved	= 128;	// ����� �� ���������� ������ ��� ������ ����
		} option_mask;

		static const struct sAnchors {		// ������������ �����
			static const unsigned char upper_left_corner =		0b00001001;	// ����� �� ������� ����� ���� ��������
			static const unsigned char upper_side =				0b00001010; // ����� �� ������� ������� ��������
			static const unsigned char upper_right_corner =		0b00001100; // ����� �� ������� ������ ����	��������
			static const unsigned char left_side =				0b00010001; // ����� �� ����� ������� ��������
			static const unsigned char center =					0b00010010; // ����� �� ����� ��������
			static const unsigned char right_side =				0b00010100; // ����� �� ������ ������� ��������
			static const unsigned char bottom_left_corner =		0b00100001; // ����� �� ������ ����� ���� ��������
			static const unsigned char bottom_side =			0b00100010; // ����� �� ������ ������� ��������
			static const unsigned char bottom_right_corner =	0b00100100; // ����� �� ������ ������ ���� ��������
		} anchors;
		
		unsigned char anchor;	// ���� 0..2 �������� �� �������� �� X, � ���� 3..5 - �� �������� �� Y

		bool mouse_inside[2];	// ����. ��������� �� ���� ������ ������?

		float x, y;				// �������� ������������ ��������� ����� (��. anchor)
		tObject *owner;			// ��������� �� ���������

		unsigned char options;	// ������� �����. ������ "tObject::option_mask"

	public:
		tObject(tObject* _owner);
		virtual ~tObject();

		void setAnchor(unsigned char new_anchor);
		void setOptions(unsigned char new_options);
		void changeOneOption(unsigned char one_option, bool state);
		void message(tObject* addr, int type, int code, tObject* from);
		void message(tEvent e);
		void setOwner(tObject *new_owner);
		void clearEvent(tEvent& e);

		bool checkOption(unsigned char option);
		sf::Vector2f getPosition();
		unsigned char getOptions();
		unsigned char getAnchor();

		virtual void move(sf::Vector2f delta);
		virtual void putEvent(tEvent e);
		virtual void getEvent(tEvent& e);
		virtual void handleEvent(tEvent& e);
		virtual void draw(sf::RenderTarget& target);
		virtual void setPosition(sf::Vector2f new_position);
		virtual void setSize(sf::Vector2f new_size);

		virtual sf::FloatRect getLocalBounds() = 0;
		virtual sf::FloatRect getGlobalBounds();
		virtual sf::Vector2f getRelativeStartPosition();	// ���������� ��������� ����� ������� ��������� (������������ �� �����)
		virtual bool pointIsInsideMe(sf::Vector2i point) = 0;
	};

	class tGroup : public tObject { // �����-���������
	protected:
		list<tObject*> elem;		// ��������� ���������, ���������� � ������ ������

	public:
		tGroup(tObject* _owner);
		virtual ~tGroup();

		void _insert(tObject *object);		// �������� �������� � ������ ������������
		void select(tObject *object);		// ��������� ����� "�������" � ��������
		void forEach(unsigned int code, tObject* from);	// ��������� ������� ��� ���� ������������
		
		bool _delete(tObject *object);		// �������� �������� �� ������

		virtual void draw(sf::RenderTarget& target);
		virtual void handleEvent(tEvent& e);
	};

	class tRenderRect : public tGroup {
	protected:
		sf::VertexArray render_squad;		// ������, � ������� ����������� ���������
		sf::RenderTexture render_texture;	// ��������, � ������� �������������� �������
		sf::Color clear_color;				// ���� ������� ��������

	public:
		tRenderRect(tObject* _owner, sf::FloatRect rect = { 0, 0, 64, 64 });
		virtual ~tRenderRect();

		void setTextureSize(sf::Vector2u new_size);
		void setClearColor(sf::Color new_color);

		virtual void handleEvent(tEvent& e);
		virtual void setSize(sf::Vector2f new_size);
		virtual void setPosition(sf::Vector2f new_position);
		virtual void draw(sf::RenderTarget& target);
		virtual void move(sf::Vector2f delta);

		virtual bool pointIsInsideMe(sf::Vector2i point);
		virtual sf::FloatRect getLocalBounds();
	};

	class tRectShape : public tObject {
	protected:
		sf::RectangleShape shape;

	public:
		tRectShape(tObject* _owner, sf::FloatRect rect = {0, 0, 64, 64}, sf::Color fill_color = sf::Color(255, 255, 255, 255));
		virtual ~tRectShape();

		void setColor(sf::Color new_color);

		virtual void draw(sf::RenderTarget& target);
		virtual void setPosition(sf::Vector2f new_position);
		virtual void setSize(sf::Vector2f new_size);
		
		virtual bool pointIsInsideMe(sf::Vector2i point);
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

		sf::Vector2i old_mouse_position;

		char screen_code;			// ��� �������� ������
		
	public:
		tDesktop(std::string path_to_folder);
		~tDesktop();

		void run();								// ������� ����
		void saveData();						// ��������� ������ � ����
		void loadCustomFont(std::string path_to_font);	// ���������� ���������������� �����
		sf::Font& getFont();					// �������� �����
		
		bool windowIsOpen();					// ���������� ������ ����

		virtual void changeScreen(char new_screen_code);// �������� ����� (����, �������� �����, �������� NPC)
		virtual void putEvent(tEvent e);
		virtual void getEvent(tEvent& e);
		virtual void handleEvent(tEvent& e);
	};

	class tText : public tObject {
	protected:
		sf::Text text_object;				// SFML �����
		sf::Font font;						// ����� ������
		bool font_loaded;					// ����. �������� �� �����?

	public:
		tText(tObject* _owner, sf::Vector2f position = {0, 0}, std::string string = "Some text");
		virtual ~tText();

		void setString(std::string new_string);
		void setTextColor(sf::Color new_color);
		void setFont(sf::Font new_font);
		void setCharSize(unsigned int new_char_size);
		void setOutlineThickness(unsigned char new_thickness);

		bool getFontState();	// �������� ��� ���
		sf::Text getTextObject();
		sf::Color getFillColor();

		virtual void draw(sf::RenderTarget& target);
		virtual void setPosition(sf::Vector2f new_position);

		virtual sf::FloatRect getLocalBounds();
		virtual bool pointIsInsideMe(sf::Vector2i point);
	};

	class tButton : public tRenderRect {
	private:
		void initButton();

	protected:
		const unsigned char side_offset;

		int self_code;						// ���, ������� �������� ������ ��� ������� �� ��
		bool custom_skin_loaded;			// ����. �������� �� ���������������� ���� ������?
		char alignment;						// ��� ������������
		sf::Texture custom_skin;			// ���������������� ���� ������
		sf::Vector2i text_offset;			// ��������� �������� ������, � ������, ���� �� ����� ��������� (��� �� ��-�� �������)
		
	public:
		enum class alignment_type { Left, Middle, Right };

		tButton(tObject* _owner, sf::FloatRect rect = { 0, 0, 128, 48 });
		virtual ~tButton();

		void updateTexture();
		void loadCustomSkin(std::string path_to_skin);
		void setAlignment(char new_alignment);
		void setTextOffset(sf::Vector2i new_offset);
		void setCode(int new_code);
		void setFont(sf::Font new_font);
		void setString(std::string new_string);
		void setTextColor(sf::Color new_color);
		void setCharSize(unsigned int new_char_size);
		void setOutlineThickness(unsigned char new_thickness);

		virtual void draw(sf::RenderTarget& target);
		virtual void handleEvent(tEvent& e);

		virtual bool pointIsInsideMe(sf::Vector2i point);
		virtual sf::FloatRect getLocalBounds();
	};

	class tWindow : public tRenderRect {
	private:
		static const int heap_height = 32;
		static const unsigned char caption_char_size = 24;

		void initWindow();
	
	protected:
		bool need_rerender;	// ����� �����������?

		bool font_loaded;	// ����. �������� �� �����?
		sf::Font font;		// �����

		std::string caption;				// ��������� ����
		sf::Color color_heap;				// ���� �����
		sf::Color color_space;				// ���� �������� �����
		sf::Color color_caption_active;		// ���� ��������� ��������� ����
		sf::Color color_caption_inactive;	// ���� ��������� ����������� ����
		sf::Vector2f caption_offset;		// ��������� �������� ���������, � ������, ���� �� ����� ��������� (��� �� ��-�� �������)

	public:
		tWindow(tObject* _owner, sf::FloatRect rect = { 0, 0, 300, 300 }, std::string caption = "Default caption");
		virtual ~tWindow();

		void setCaption(std::string new_caption);
		void setColorHeap(sf::Color new_color);
		void setColorSpace(sf::Color new_color);
		void setColorCaptionActive(sf::Color new_color);
		void setColorCaptionInactive(sf::Color new_color);
		void setFont(sf::Font new_font);
		void setCaptionOffset(sf::Vector2f new_offset);

		std::string getCaption();
		bool pointIsInHeap(sf::Vector2i point);
		const int getHeapHeight();

		virtual void draw(sf::RenderTarget& target);
		virtual void handleEvent(tEvent& e);
		
		virtual bool pointIsInsideMe(sf::Vector2i point);
		virtual sf::FloatRect getLocalBounds();
	};
}