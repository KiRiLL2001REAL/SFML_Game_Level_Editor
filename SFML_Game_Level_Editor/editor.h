#pragma once

#define list std::list

namespace edt {

	static const struct sObjectsJsonIds {
		static const unsigned int tDesktop		= 0;
		static const unsigned int tWindow		= 1;
		static const unsigned int tButton		= 2;
		static const unsigned int tText			= 3;
		static const unsigned int tRectShape	= 4;
		static const unsigned int tDisplay		= 5;
		static const unsigned int tScrollbar	= 6;
	} objects_json_ids;

	class tAbstractBasicClass;
	class tObject;
	class tGroup;
	class tRenderRect;
	class tRectShape;
	class tDesktop;
	class tButton;
	class tWindow;
	class tText;
	class tDisplay;
	class tScrollbar;

	struct tEvent {
		static const struct sTypes { // ���� �������
			static const unsigned int Nothing		= 0;
			static const unsigned int Mouse			= 1;
			static const unsigned int Keyboard		= 2;
			static const unsigned int Broadcast		= 3;
			static const unsigned int Button		= 4;
		} types;

		static const struct sCodes { // ���� �������
			static const unsigned int Nothing			= 0;
			static const unsigned int Activate			= 1;
			static const unsigned int Deactivate		= 2;
			static const unsigned int Show				= 3;
			static const unsigned int Hide				= 4;
			static const unsigned int Move				= 5;
			static const unsigned int Adopt				= 6;
			static const unsigned int Delete			= 7;
			static const unsigned int Close				= 8;
			static const unsigned int CloseApplication	= 9;
			static const unsigned int MouseMoved		= 10;
			static const unsigned int MouseButton		= 11;
			static const unsigned int ResetButtons		= 12;
			static const unsigned int UpdateTexture		= 13;
			static const unsigned int FontRequest		= 14;
			static const unsigned int FontAnswer		= 15;
		} codes;

		unsigned int type = types.Nothing; // �� ����� ����� ������� (���)
		unsigned int code = codes.Nothing; // ��� �������
		tAbstractBasicClass* from = nullptr;
		tAbstractBasicClass* address = nullptr;
		
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

	class tAbstractBasicClass {
	protected:
		tAbstractBasicClass* owner;
	
	public:
		tAbstractBasicClass(tAbstractBasicClass* _owner);
		tAbstractBasicClass(const tAbstractBasicClass& a);
		~tAbstractBasicClass();

		void clearEvent(tEvent& e);
		void message(tAbstractBasicClass* addr, unsigned int type, unsigned int code, tAbstractBasicClass* from);
		void message(tEvent e);

		virtual void setOwner(tAbstractBasicClass* new_owner);
		virtual void putEvent(tEvent e);
		virtual void getEvent(tEvent& e);
		virtual void handleEvent(tEvent& e) = 0;
		virtual void draw(sf::RenderTarget& target) = 0;

		virtual tAbstractBasicClass* getOwner();
		virtual nlohmann::json saveParamsInJson();
		virtual sf::FloatRect getLocalBounds() = 0;
		virtual sf::FloatRect getGlobalBounds();
	};

	class tObject : public tAbstractBasicClass { // ����� �������
	public:
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
	
	protected:
		unsigned char anchor;	// ���� 0..2 �������� �� �������� �� X, � ���� 3..5 - �� �������� �� Y (�� tObject::anchors)

		bool mouse_inside[2];	// ����. ��������� �� ���� ������ ������?

		float x, y;				// �������� ������������ ��������� ����� (��. anchor)
		
		unsigned char options;	// ������� �����. ������ "tObject::option_mask"

	public:
		tObject(tAbstractBasicClass* _owner);
		tObject(tAbstractBasicClass* _owner, nlohmann::json& js);
		tObject(const tObject &o);
		virtual ~tObject();

		void setAnchor(unsigned char new_anchor);
		void setOptions(unsigned char new_options);
		void changeOneOption(unsigned char one_option, bool state);
		
		bool checkOption(unsigned char option);
		sf::Vector2f getPosition();
		unsigned char getOptions();
		unsigned char getAnchor();

		virtual void move(sf::Vector2f delta);
		virtual void draw(sf::RenderTarget& target);
		virtual void setPosition(sf::Vector2f new_position);
		virtual void setSize(sf::Vector2f new_size);
		virtual void updateTexture() = 0;

		virtual sf::Vector2f getRelativeStartPosition();	// ���������� ��������� ����� ������� ��������� (������������ �� �����)
		virtual bool pointIsInsideMe(sf::Vector2i point) = 0;
		virtual nlohmann::json saveParamsInJson();
	};

	class tGroup : public tAbstractBasicClass { // �����-���������
	protected:
		list<tAbstractBasicClass*> elem;		// ��������� ���������, ���������� � ������ ������

	public:
		tGroup(tAbstractBasicClass* _owner);
		tGroup(tAbstractBasicClass* _owner, nlohmann::json& js);
		tGroup(const tGroup& g);
		virtual ~tGroup();

		void forEach(unsigned int code, tAbstractBasicClass* from);	// ��������� ������� ��� ���� ������������
		void makeObjectsFromJson(tAbstractBasicClass* _owner, nlohmann::json& js);

		virtual void draw(sf::RenderTarget& target);
		virtual void handleEvent(tEvent& e);
		virtual void _insert(tAbstractBasicClass* object);		// �������� �������� � ������ ������������
		virtual void select(tAbstractBasicClass* object);		// ��������� ����� "�������" � ��������
		
		virtual bool _delete(tAbstractBasicClass* object);		// �������� �������� �� ������
		virtual nlohmann::json saveParamsInJson();
	};

	class tRenderRect : public tObject {
	protected:
		sf::VertexArray render_squad;		// ������, � ������� ����������� ���������
		sf::RenderTexture render_texture;	// ��������, � ������� �������������� �������
		sf::Color clear_color;				// ���� ������� ��������
		bool need_rerender;					// ����� �� �����������

	public:
		tRenderRect(tAbstractBasicClass* _owner, sf::FloatRect rect = { 0, 0, 64, 64 });
		tRenderRect(tAbstractBasicClass* _owner, nlohmann::json& js);
		tRenderRect(const tRenderRect& r);
		virtual ~tRenderRect();

		void setClearColor(sf::Color new_color);

		sf::Vector2u getTextureSize();

		virtual void setSize(sf::Vector2f new_size);
		virtual void setPosition(sf::Vector2f new_position);
		virtual void draw(sf::RenderTarget& target);
		virtual void move(sf::Vector2f delta);
		virtual void setTextureSize(sf::Vector2u new_size);

		virtual sf::FloatRect getLocalBounds();
		virtual nlohmann::json saveParamsInJson();
	};

	class tRectShape : public tObject {
	protected:
		sf::RectangleShape shape;

	public:
		tRectShape(tAbstractBasicClass* _owner, sf::FloatRect rect = {0, 0, 64, 64}, sf::Color fill_color = sf::Color(255, 255, 255, 255));
		tRectShape(tAbstractBasicClass* _owner, nlohmann::json& js);
		tRectShape(const tRectShape& s);
		virtual ~tRectShape();

		void setColor(sf::Color new_color);

		virtual void draw(sf::RenderTarget& target);
		virtual void setPosition(sf::Vector2f new_position);
		virtual void setSize(sf::Vector2f new_size);
		virtual void move(sf::Vector2f delta);
		virtual void updateTexture();
		virtual void handleEvent(tEvent& e);
		
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
		void completeEvents();					// ��������� �������

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
		tText(tAbstractBasicClass* _owner, sf::Vector2f position = {0, 0}, std::wstring string = L"Some text");
		tText(tAbstractBasicClass* _owner, nlohmann::json& js);
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
		virtual void move(sf::Vector2f delta);
		virtual void updateTexture();

		virtual sf::FloatRect getLocalBounds();
		virtual bool pointIsInsideMe(sf::Vector2i point);
		virtual nlohmann::json saveParamsInJson();
	};

	class tButton : public tRenderRect {
	protected:
		const unsigned char side_offset;

		int self_code;						// ���, ������� �������� ������ ��� ������� �� ��
		bool custom_skin_loaded;			// ����. �������� �� ���������������� ���� ������?
		char alignment;						// ��� ������������
		sf::Texture custom_skin;			// ���������������� ���� ������
		sf::Vector2i text_offset;			// ��������� �������� ������, � ������, ���� �� ����� ��������� (��� �� ��-�� �������)
		
		tText* text;							// ����� ������ ������

	public:
		enum class text_alignment_type { Left, Middle, Right };

		tButton(tAbstractBasicClass* _owner, sf::FloatRect rect = { 0, 0, 128, 48 });
		tButton(tAbstractBasicClass* _owner, nlohmann::json& js);
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

		virtual void handleEvent(tEvent& e);
		virtual void updateTexture();

		virtual bool pointIsInsideMe(sf::Vector2i point);
		virtual nlohmann::json saveParamsInJson();
	};

	class tWindow : public tRenderRect {
	public:
		static const int heap_height = 32;

	private:
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

		tButton* button_close;				// ������ ��������
		tRectShape* heap_shape;				// ������ �����
		tDisplay* display;					// ������, � ������� ���������� ��������� ���� ������������ ������������ ����
		tScrollbar* scrollbar_v, * scrollbar_h;	// ��������

	public:
		tWindow(tAbstractBasicClass* _owner, sf::FloatRect rect = { 0, 0, 300, 300 }, std::wstring caption = L"Default caption");
		tWindow(tAbstractBasicClass* _owner, nlohmann::json& js);
		tWindow(const tWindow& w);
		virtual ~tWindow();

		void setCaption(std::wstring new_caption);
		void setHeapColor(sf::Color new_color);
		void setAreaColor(sf::Color new_color);
		void setActiveCaptionColor(sf::Color new_color);
		void setInactiveCaptionColor(sf::Color new_color);
		void setFont(sf::Font new_font);
		void setCaptionOffset(sf::Vector2f new_offset);
		void setCameraOffset(sf::Vector2f new_offset);
		void setDisplayTextureSize(sf::Vector2u new_size);

		tDisplay* getDisplayPointer();
		std::wstring getCaption();
		bool pointIsInHeap(sf::Vector2i point);
		const int getHeapHeight();

		virtual void handleEvent(tEvent& e);
		virtual void updateTexture();
		
		virtual bool pointIsInsideMe(sf::Vector2i point);
		virtual nlohmann::json saveParamsInJson();
	};

	class tDisplay : public tRenderRect, public tGroup {
	protected:
		tAbstractBasicClass* owner;

	public:
		tDisplay(tAbstractBasicClass* _owner, sf::FloatRect rect = { 0, 0, 100, 100 });
		tDisplay(tAbstractBasicClass* _owner, nlohmann::json& js);
		tDisplay(const tDisplay& d);
		virtual ~tDisplay();

		void setCameraOffset(sf::Vector2f new_offset);

		virtual void draw(sf::RenderTarget& target);
		virtual void handleEvent(tEvent& e);
		virtual void updateTexture();
		virtual void setOwner(tAbstractBasicClass* new_owner);
		virtual void setTextureSize(sf::Vector2u new_size);

		virtual tAbstractBasicClass* getOwner();
		virtual bool pointIsInsideMe(sf::Vector2i point);
		virtual sf::FloatRect getLocalBounds();
		virtual nlohmann::json saveParamsInJson();
	};

	class tScrollbar : public tRenderRect {
	public:
		static const int thickness = 24;	// ������ ���������

		static const struct sOptionMask {	// ����� �������� (�������������� ��� scrollbar)
			static const unsigned char is_moving = 1;				// ������ ������������ ��� ������ ����
			static const unsigned char is_resizing = 2;				// ������ ������ ������ ��� ������ ����
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
		tRectShape* scroller;					// ��������
		sf::Vector2f old_position;				// ���������� ������� ��������
		sf::Color color_scroller_area;	// ���� ����, �� ������� ������ ��������

	public:
		tScrollbar(tAbstractBasicClass* _owner, bool vertical = true, sf::FloatRect rect = { 0, 0, thickness, thickness * 8});
		tScrollbar(tAbstractBasicClass* _owner, nlohmann::json& js);
		tScrollbar(const tScrollbar& s);
		virtual ~tScrollbar();

		void updateScrollerSize();

		virtual void updateTexture();	// tWindow ������ ������� �� ���, ��� ������ ��� updateTextureEvent, ����� ������ tDisplay'��
		virtual void handleEvent(tEvent& e);
		virtual void setTextureSize(sf::Vector2u new_size);

		virtual bool pointIsInsideMe(sf::Vector2i point);
		virtual nlohmann::json saveParamsInJson();
	};

}