#pragma once

#define list std::list

namespace edt {

	static const struct sObjectsJsonIds {
		static const unsigned char tDesktop = 0;
		static const unsigned char tWindow = 1;
		static const unsigned char tButton = 2;
		static const unsigned char tText = 3;
		static const unsigned char tRectShape = 4;
	} objects_json_ids;

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
			std::wstring string = L"";
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
		
		unsigned char anchor;	// ���� 0..2 �������� �� �������� �� X, � ���� 3..5 - �� �������� �� Y (�� tObject::anchors)

		bool mouse_inside[2];	// ����. ��������� �� ���� ������ ������?

		float x, y;				// �������� ������������ ��������� ����� (��. anchor)
		tObject *owner;			// ��������� �� ���������

		unsigned char options;	// ������� �����. ������ "tObject::option_mask"

	public:
		tObject(tObject* _owner);
		tObject(tObject* _owner, nlohmann::json& js);
		tObject(const tObject &o);
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
		virtual void updateTexture() = 0;

		virtual sf::FloatRect getLocalBounds() = 0;
		virtual sf::FloatRect getGlobalBounds();
		virtual sf::Vector2f getRelativeStartPosition();	// ���������� ��������� ����� ������� ��������� (������������ �� �����)
		virtual bool pointIsInsideMe(sf::Vector2i point) = 0;
		virtual nlohmann::json saveParamsInJson();
	};

	class tGroup : public tObject { // �����-���������
	protected:
		list<tObject*> elem;		// ��������� ���������, ���������� � ������ ������

	public:
		tGroup(tObject* _owner);
		tGroup(tObject* _owner, nlohmann::json& js);
		tGroup(const tGroup& g);
		virtual ~tGroup();

		void _insert(tObject *object);		// �������� �������� � ������ ������������
		void select(tObject *object);		// ��������� ����� "�������" � ��������
		void forEach(unsigned int code, tObject* from);	// ��������� ������� ��� ���� ������������
		
		bool _delete(tObject *object);		// �������� �������� �� ������

		virtual void draw(sf::RenderTarget& target);
		virtual void handleEvent(tEvent& e);
		void makeObjectsFromJson(tObject* _owner, nlohmann::json& js);

		virtual nlohmann::json saveParamsInJson();
	};

	class tRenderRect : public tGroup {
	protected:
		sf::VertexArray render_squad;		// ������, � ������� ����������� ���������
		sf::RenderTexture render_texture;	// ��������, � ������� �������������� �������
		sf::Color clear_color;				// ���� ������� ��������
		
	public:
		tRenderRect(tObject* _owner, sf::FloatRect rect = { 0, 0, 64, 64 });
		tRenderRect(tObject* _owner, nlohmann::json& js);
		tRenderRect(const tRenderRect& r);
		virtual ~tRenderRect();

		void setTextureSize(sf::Vector2u new_size);
		void setClearColor(sf::Color new_color);

		virtual void setSize(sf::Vector2f new_size);
		virtual void setPosition(sf::Vector2f new_position);
		virtual void draw(sf::RenderTarget& target);
		virtual void move(sf::Vector2f delta);
		virtual void setCameraOffset(sf::Vector2f new_offset);

		virtual sf::FloatRect getLocalBounds();
		virtual nlohmann::json saveParamsInJson();
	};

	class tRectShape : public tObject {
	protected:
		sf::RectangleShape shape;

	public:
		tRectShape(tObject* _owner, sf::FloatRect rect = {0, 0, 64, 64}, sf::Color fill_color = sf::Color(255, 255, 255, 255));
		tRectShape(tObject* _owner, nlohmann::json& js);
		tRectShape(const tRectShape& s);
		virtual ~tRectShape();

		void setColor(sf::Color new_color);

		virtual void draw(sf::RenderTarget& target);
		virtual void setPosition(sf::Vector2f new_position);
		virtual void setSize(sf::Vector2f new_size);
		virtual void updateTexture();
		
		virtual bool pointIsInsideMe(sf::Vector2i point);
		virtual sf::FloatRect getLocalBounds();
		virtual nlohmann::json saveParamsInJson();
	};

	class tDesktop : public tGroup {
	protected:
		std::string path_to_folder;			// ���� �� ������� �����
		sf::RenderWindow window;			// ������� ����
		sf::Font font_default;				// ����� �� ���������
		sf::Font custom_font;				// ���������������� �����
		bool custom_font_loaded;			// ����. �������� �� ���������������� �����?
		list<tEvent> events;				// ������ ������� � ���������
		sf::RectangleShape background;		// ������� �������������
		nlohmann::json json_configuration;	// ������-���� ���������, ���������� ����������� ��������� ������� ���������
		sf::Vector2i old_mouse_position;	// ���������� ��������� ���� (��������� ��� ������ tEvent::mouse.dX(.dY))
		char screen_code;					// ��� �������� ������
		sf::Vector2u window_size;			// ������ ����
		
	public:
		tDesktop(std::string path_to_folder, unsigned char _screen_code);
		virtual ~tDesktop();

		void run();								// ������� ����
		void saveData();						// ��������� ������ � ����
		void loadCustomFont(std::string path_to_font);	// ���������� ���������������� �����

		sf::Font& getFont();					// �������� �����
		bool windowIsOpen();					// ���������� ������ ����

		virtual void changeScreen(char new_screen_code);// �������� ����� (����, �������� �����, �������� NPC � �.�.)
		virtual void putEvent(tEvent e);
		virtual void getEvent(tEvent& e);
		virtual void handleEvent(tEvent& e);
		virtual void updateTexture();
		virtual void draw(sf::RenderTarget& target);

		virtual sf::FloatRect getLocalBounds();
		virtual bool pointIsInsideMe(sf::Vector2i point);
		virtual nlohmann::json saveParamsInJson();
	};

	class tText : public tObject {
	protected:
		sf::Text text_object;				// SFML �����
		sf::Font font;						// ����� ������
		bool font_loaded;					// ����. �������� �� �����?

	public:
		tText(tObject* _owner, sf::Vector2f position = {0, 0}, std::wstring string = L"Some text");
		tText(tObject* _owner, nlohmann::json& js);
		tText(const tText& t);
		virtual ~tText();

		void setString(std::wstring new_string);
		void setTextColor(sf::Color new_color);
		void setFont(sf::Font new_font);
		void setCharSize(unsigned int new_char_size);
		void setOutlineThickness(unsigned char new_thickness);

		bool getFontState();	// �������� ��� ���
		sf::Text& getTextObject();
		sf::Color getFillColor();

		virtual void draw(sf::RenderTarget& target);
		virtual void setPosition(sf::Vector2f new_position);
		virtual void handleEvent(tEvent& e);
		virtual void updateTexture();

		virtual sf::FloatRect getLocalBounds();
		virtual bool pointIsInsideMe(sf::Vector2i point);
		virtual nlohmann::json saveParamsInJson();
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
		
		tText text;							// ����� ������ ������

	public:
		enum class text_alignment_type { Left, Middle, Right };

		tButton(tObject* _owner, sf::FloatRect rect = { 0, 0, 128, 48 });
		tButton(tObject* _owner, nlohmann::json& js);
		tButton(const tButton& b);
		virtual ~tButton();

		void loadCustomSkin(std::string path_to_skin);
		void setTextAlignment(char new_alignment);
		void setTextOffset(sf::Vector2i new_offset);
		void setCode(int new_code);
		void setFont(sf::Font new_font);
		void setString(std::wstring new_string);
		void setTextColor(sf::Color new_color);
		void setCharSize(unsigned int new_char_size);
		void setOutlineThickness(unsigned char new_thickness);

		virtual void draw(sf::RenderTarget& target);
		virtual void handleEvent(tEvent& e);
		virtual void updateTexture();

		virtual bool pointIsInsideMe(sf::Vector2i point);
		virtual nlohmann::json saveParamsInJson();
	};

	class tWindow : public tRenderRect {
	private:
		static const int heap_height = 32;
		static const unsigned char caption_char_size = 24;

		void initWindow();
	
	protected:
		bool font_loaded;	// ����. �������� �� �����?
		sf::Font font;		// �����

		std::wstring caption;				// ��������� ����
		sf::Color color_heap;				// ���� �����
		sf::Color color_area;				// ���� �������� �����
		sf::Color color_caption_active;		// ���� ��������� ��������� ����
		sf::Color color_caption_inactive;	// ���� ��������� ����������� ����
		sf::Vector2f caption_offset;		// ��������� �������� ���������, � ������, ���� �� ����� ��������� (��� �� ��-�� �������)

		tButton button_close;				// ������ ��������
		tRectShape heap_shape;				// ������ �����
		tRectShape area_shape;				// ������ ������� �������

	public:
		tWindow(tObject* _owner, sf::FloatRect rect = { 0, 0, 300, 300 }, std::wstring caption = L"Default caption");
		tWindow(tObject* _owner, nlohmann::json& js);
		tWindow(const tWindow& w);
		virtual ~tWindow();

		void setCaption(std::wstring new_caption);
		void setHeapColor(sf::Color new_color);
		void setAreaColor(sf::Color new_color);
		void setActiveCaptionColor(sf::Color new_color);
		void setInactiveCaptionColor(sf::Color new_color);
		void setFont(sf::Font new_font);
		void setCaptionOffset(sf::Vector2f new_offset);

		std::wstring getCaption();
		bool pointIsInHeap(sf::Vector2i point);
		const int getHeapHeight();

		virtual void draw(sf::RenderTarget& target);
		virtual void handleEvent(tEvent& e);
		virtual void updateTexture();
		virtual void setCameraOffset(sf::Vector2f new_offset);
		
		virtual bool pointIsInsideMe(sf::Vector2i point);
		virtual nlohmann::json saveParamsInJson();
	};

	/*
	class tScrollbar : public tRenderRect {
	public:
		static const struct sOrientation {	// ������������ ���������� ��������
			static const unsigned char vertical = 0;	// ������������ ����������
			static const unsigned char horizontal = 1;	// �������������� ����������
		} orientation_types;

	protected:
		int minimum;				// ����������� �������� ��������
		int maximum;				// ������������ �������� ��������
		unsigned char orientation;	// ���������� ��������
		sf::Color color_space;		// ���� �������� ������� ������-����
		sf::Color color_slider;		// ���� ��������

	public:
		tScrollbar(tObject* _owner, int start_offset = 0, int max_offset = 20, sf::FloatRect rect = { 0, 0, 20, 100 }, unsigned char _orientation = orientation_types.vertical);
		tScrollbar(tObject* _owner, nlohmann::json& js);
		virtual ~tScrollbar();

		void setOrientation(unsigned char new_orientation);
		void setMinimum(int new_min);
		void setMaximum(int new_max);

		virtual void setSize(sf::Vector2f new_size);
		virtual void handleEvent(tEvent& e);

		virtual nlohmann::json saveParamsInJson();
	};
	*/
}