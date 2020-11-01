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
	private:
		struct sMouse {
			char button = 0;
			char what_happened = 0;
			int x = 0;
			int y = 0;
			int dX = 0;
			int dY = 0;
		};
		struct sKey {
			char button = 0;
			int what_happened = 0;
			bool control = false;
			bool alt = false;
			bool shift = false;
		};
		struct sText {
			std::wstring string = L"";
			char char_size = 12;
			int x = 0;
			int y = 0;
			sf::Color color = sf::Color(255, 255, 255, 255);
		};
		struct sFont {
			sf::Font* font = nullptr;
		};
	public:

		static const struct sTypes {	// Типы событий
			static const unsigned int Nothing = 0;
			static const unsigned int Mouse = 1;
			static const unsigned int Keyboard = 2;
			static const unsigned int Broadcast = 3;
			static const unsigned int Button = 4;
		} types;
		static const struct sCodes {	// Коды событий
			static const unsigned int Nothing = 0;
			static const unsigned int Activate = 1;
			static const unsigned int Deactivate = 2;
			static const unsigned int Show = 3;
			static const unsigned int Hide = 4;
			static const unsigned int Move = 5;
			static const unsigned int Adopt = 6;
			static const unsigned int Delete = 7;
			static const unsigned int Close = 8;
			static const unsigned int CloseApplication = 9;
			static const unsigned int MouseMoved = 10;
			static const unsigned int MouseButton = 11;
			static const unsigned int ResetButtons = 12;
			static const unsigned int UpdateTexture = 13;
			static const unsigned int FontRequest = 14;
			static const unsigned int FontAnswer = 15;
		} codes;

		unsigned int type = types.Nothing; // Из какой сферы событие (тип)
		unsigned int code = codes.Nothing; // Код события
		tAbstractBasicClass* from = nullptr;
		tAbstractBasicClass* address = nullptr;
		
		sMouse mouse;	// Событие от мыши
		sKey key;	// Событие от клавиатуры
		sText text;	// Вывод текста
		sFont font;	// Запрос шрифта
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

		virtual void putEvent(tEvent e);
		virtual void getEvent(tEvent& e);
		virtual void handleEvent(tEvent& e) = 0;
		virtual void draw(sf::RenderTarget& target) = 0;

		// Setters
		virtual void setOwner(tAbstractBasicClass* new_owner);

		// Getters
		virtual tAbstractBasicClass* getOwner() const;
		virtual nlohmann::json getParamsInJson() const;
		virtual sf::FloatRect getLocalBounds() const;
		virtual sf::FloatRect getGlobalBounds() const;
	};

	class tObject : public tAbstractBasicClass { // Класс объекта
	public:
		static const struct sOptionMask {	// Маски операций
			static const unsigned char is_moving = 1;		// Объект перемещается при помощи мыши
			static const unsigned char is_resizing = 2;		// Объект меняет размер при помощи мыши
			static const unsigned char can_be_drawn = 4;	// Можно ли выводить этот объект на экран
			static const unsigned char is_active = 8;		// Активен ли объект
			static const unsigned char dummy_1 = 16;		// Бит не задействован
			static const unsigned char dummy_2 = 32;		// Бит не задействован
			static const unsigned char can_be_resized = 64;	// Можно ли менять размер объекта при помощи мыши
			static const unsigned char can_be_moved	= 128;	// Можно ли перемещать объект при помощи мыши
		} option_mask;

		static const struct sAnchors {		// Всевозможные якори
			static const unsigned char upper_left_corner =		0b00001001;	// Якорь на верхний левый угол родителя
			static const unsigned char upper_side =				0b00001010; // Якорь на верхнюю сторону родителя
			static const unsigned char upper_right_corner =		0b00001100; // Якорь на верхний правый угол	родителя
			static const unsigned char left_side =				0b00010001; // Якорь на левую сторону родителя
			static const unsigned char center =					0b00010010; // Якорь на центр родителя
			static const unsigned char right_side =				0b00010100; // Якорь на правую сторону родителя
			static const unsigned char bottom_left_corner =		0b00100001; // Якорь на нижний левый угол родителя
			static const unsigned char bottom_side =			0b00100010; // Якорь на нижнюю сторону родителя
			static const unsigned char bottom_right_corner =	0b00100100; // Якорь на нижний правый угол родителя
		} anchors;
	
	protected:
		unsigned char anchor;	// Биты 0..2 отвечают за привязку по X, а биты 3..5 - за привязку по Y (см tObject::anchors)

		bool mouse_inside[2];	// Флаг. Находится ли мышь внутри кнопки?

		float x, y;				// Смещение относительно начальной точки (см. anchor)
		
		unsigned char options;	// Битовая штука. Смотри "tObject::option_mask"

	public:
		tObject(tAbstractBasicClass* _owner);
		tObject(tAbstractBasicClass* _owner, nlohmann::json& js);
		tObject(const tObject &o);
		virtual ~tObject();

		virtual void move(sf::Vector2f delta);
		virtual void draw(sf::RenderTarget& target);
		virtual void updateTexture() = 0;
		virtual bool pointIsInsideMe(sf::Vector2i point) const;

		// Setters
		void setAnchor(const unsigned char &new_anchor);
		void setOptions(const unsigned char &new_options);
		void changeOneOption(const unsigned char &one_option, const bool &state);
		virtual void setPosition(const sf::Vector2f &new_position);
		virtual void setSize(const sf::Vector2f &new_size);
		
		// Getters
		bool checkOption(unsigned char option) const;
		sf::Vector2f getPosition() const;
		unsigned char getOptions() const;
		unsigned char getAnchor() const;
		virtual sf::Vector2f getRelativeStartPosition() const;	// Возвращает начальную точку системы координат (ориентируясь на якорь)
		virtual nlohmann::json getParamsInJson() const;
	};

	class tGroup : public tAbstractBasicClass { // Класс-контейнер
	protected:
		list<tAbstractBasicClass*> elem;		// Контейнер элементов, хранящихся в данном классе

	public:
		tGroup(tAbstractBasicClass* _owner);
		tGroup(tAbstractBasicClass* _owner, nlohmann::json& js);
		tGroup(const tGroup& g);
		virtual ~tGroup();

		void forEach(unsigned int code, tAbstractBasicClass* from);	// Выполнить команду для всех подэлементов
		void makeObjectsFromJson(tAbstractBasicClass* _owner, nlohmann::json& js);

		virtual void draw(sf::RenderTarget& target);
		virtual void handleEvent(tEvent& e);
		virtual void _insert(tAbstractBasicClass* object);		// Внесение элемента в список подэлементов
		virtual bool _delete(tAbstractBasicClass* object);		// Удаление элемента из списка
		virtual void select(tAbstractBasicClass* object);		// Установка флага "активен" у элемента

		// Getters
		virtual nlohmann::json getParamsInJson() const;
	};

	class tRenderRect : public tObject {
	protected:
		sf::VertexArray render_squad;		// Фигура, в которой выполняется отрисовка
		sf::RenderTexture render_texture;	// Текстура, в которой отрисовываются объекты
		sf::Color clear_color;				// Цвет очистки текстуры
		bool need_rerender;					// Нужна ли перерисовка

	public:
		tRenderRect(tAbstractBasicClass* _owner, sf::FloatRect rect = { 0, 0, 64, 64 });
		tRenderRect(tAbstractBasicClass* _owner, nlohmann::json& js);
		tRenderRect(const tRenderRect& r);
		virtual ~tRenderRect();

		virtual void draw(sf::RenderTarget& target);
		virtual void move(sf::Vector2f delta);

		// Setters
		void setClearColor(const sf::Color &new_color);
		virtual void setSize(const sf::Vector2f &new_size);
		virtual void setPosition(const sf::Vector2f &new_position);
		virtual void setTextureSize(const sf::Vector2u &new_size);

		// Getters
		sf::Vector2u getTextureSize() const;
		virtual sf::FloatRect getLocalBounds() const;
		virtual nlohmann::json getParamsInJson() const;
	};

	class tRectShape : public tObject {
	protected:
		sf::RectangleShape shape;

	public:
		tRectShape(tAbstractBasicClass* _owner, sf::FloatRect rect = {0, 0, 64, 64}, sf::Color fill_color = sf::Color(255, 255, 255, 255));
		tRectShape(tAbstractBasicClass* _owner, nlohmann::json& js);
		tRectShape(const tRectShape& s);
		virtual ~tRectShape();

		virtual void draw(sf::RenderTarget& target);
		virtual void move(sf::Vector2f delta);
		virtual void updateTexture();
		virtual void handleEvent(tEvent& e);
		virtual bool pointIsInsideMe(sf::Vector2i point) const;

		// Setters
		void setColor(const sf::Color &new_color);
		virtual void setPosition(const sf::Vector2f &new_position);
		virtual void setSize(const sf::Vector2f &new_size);

		// Getters
		virtual sf::FloatRect getLocalBounds() const;
		virtual nlohmann::json getParamsInJson() const;
	};

	class tDesktop : public tGroup {
	protected:
		std::string path_to_folder;			// Путь до рабочей папки
		sf::RenderWindow window;			// Главное окно
		sf::Font font_default;				// Шрифт по умолчанию
		sf::Font custom_font;				// Пользовательский шрифт
		bool custom_font_loaded;			// Флаг. Загружен ли пользовательский шрифт?
		list<tEvent> events;				// Список событий к обработке
		sf::RectangleShape background;		// Фоновый прямоугольник
		nlohmann::json json_configuration;	// Конфиг-файл программы, содержащий изначальное состояние экранов редактора
		sf::Vector2i old_mouse_position;	// Предыдущее положение мыши (требуется для работы tEvent::mouse.dX(.dY))
		char screen_code;					// Код текущего экрана
		sf::Vector2u window_size;			// Размер окна
		
	public:
		tDesktop(std::string path_to_folder, unsigned char _screen_code);
		virtual ~tDesktop();

		void run();								// Главный цикл
		void saveData() const;						// Сохранить данные в файл
		void loadCustomFont(std::string path_to_font);	// Установить пользовательский шрифт
		void completeEvents();					// Выполнить события
		bool windowIsOpen() const;					// Возвращает статус окна
		virtual void changeScreen(char new_screen_code);// Изменить экран (меню, редактор карты, редактор NPC и т.д.)
		virtual void putEvent(tEvent e);
		virtual void getEvent(tEvent& e);
		virtual void handleEvent(tEvent& e);
		virtual void updateTexture();
		virtual void draw(sf::RenderTarget& target);

		// Getters
		sf::Font& getFont() const;					// Получить шрифт
		virtual sf::FloatRect getLocalBounds() const;
		virtual bool pointIsInsideMe(sf::Vector2i point) const;
		virtual nlohmann::json getParamsInJson() const;
	};

	class tText : public tObject {
	protected:
		sf::Text text_object;				// SFML текст
		sf::Font font;						// Шрифт текста
		bool font_loaded;					// Флаг. Загружен ли шрифт?

	public:
		tText(tAbstractBasicClass* _owner, sf::Vector2f position = {0, 0}, std::wstring string = L"Some text");
		tText(tAbstractBasicClass* _owner, nlohmann::json& js);
		tText(const tText& t);
		virtual ~tText();

		virtual void draw(sf::RenderTarget& target);
		virtual void handleEvent(tEvent& e);
		virtual void move(sf::Vector2f delta);
		virtual void updateTexture();

		// Setters
		void setString(const std::wstring &new_string);
		void setTextColor(const sf::Color &new_color);
		void setFont(const sf::Font &new_font);
		void setCharSize(const unsigned int &new_char_size);
		void setOutlineThickness(const unsigned char &new_thickness);
		virtual void setPosition(const sf::Vector2f &new_position);

		// Getters
		bool getFontState() const;	// Загружен или нет
		sf::Text& getTextObject() const;
		sf::Color getFillColor() const;
		virtual sf::FloatRect getLocalBounds() const;
		virtual bool pointIsInsideMe(sf::Vector2i point) const;
		virtual nlohmann::json getParamsInJson() const;
	};

	class tButton : public tRenderRect {
	protected:
		const unsigned char side_offset;

		int self_code;					// Код, который посылает кнопка при нажатии на неё
		bool custom_skin_loaded;		// Флаг. Загружен ли пользовательский скин кнопки?
		char alignment;					// Тип выравнивания
		sf::Texture custom_skin;		// Пользовательский скин кпопки
		sf::Vector2i text_offset;		// Настройка смещения текста, в случае, если он криво выводится (это всё из-за шрифтов)
		
		tText* text;					// Текст внутри кнопки

	public:
		enum class text_alignment_type { Left, Middle, Right };

		tButton(tAbstractBasicClass* _owner, sf::FloatRect rect = { 0, 0, 128, 48 });
		tButton(tAbstractBasicClass* _owner, nlohmann::json& js);
		tButton(const tButton& b);
		virtual ~tButton();

		void loadCustomSkin(std::string path_to_skin);
		virtual void handleEvent(tEvent& e);
		virtual void updateTexture();
		virtual bool pointIsInsideMe(sf::Vector2i point) const;

		// Setters
		void setTextAlignment(const char &new_alignment);
		void setTextOffset(const sf::Vector2i &new_offset);
		void setCode(const int &new_code);
		void setFont(const sf::Font &new_font);
		void setString(const std::wstring &new_string);
		void setTextColor(const sf::Color &new_color);
		void setCharSize(const unsigned int &new_char_size);
		void setOutlineThickness(const unsigned char &new_thickness);

		// Getters
		virtual nlohmann::json getParamsInJson() const;
	};

	class tWindow : public tRenderRect {
	public:
		static const int heap_height = 32;

	private:
		static const unsigned char caption_char_size = 24;

		void initWindow();
	
	protected:
		bool font_loaded;	// Флаг. Загружен ли шрифт?
		sf::Font font;		// Шрифт

		std::wstring caption;					// Заголовок окна
		sf::Color color_heap;					// Цвет шапки
		sf::Color color_area;					// Цвет основной части
		sf::Color color_caption_active;			// Цвет заголовка активного окна
		sf::Color color_caption_inactive;		// Цвет заголовка неактивного окна
		sf::Vector2f caption_offset;			// Настройка смещения заголовка, в случае, если он криво выводится (это всё из-за шрифтов)

		tButton* button_close;					// Кнопка закрытия
		tRectShape* heap_shape;					// Фигура шапки
		tDisplay* display;						// Объект, в котором происходит отрисовка всех динамических подэлементов окна
		tScrollbar* scrollbar_v, * scrollbar_h;	// Ползунки

		sf::Vector2f last_scrollbar_offset;		// Предыдущее смещение ползунков

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
		void setCaption(const std::wstring &new_caption);
		void setHeapColor(const sf::Color &new_color);
		void setAreaColor(const sf::Color &new_color);
		void setActiveCaptionColor(const sf::Color &new_color);
		void setInactiveCaptionColor(const sf::Color &new_color);
		void setFont(const sf::Font &new_font);
		void setCaptionOffset(const sf::Vector2f &new_offset);
		void setCameraOffset(const sf::Vector2f &new_offset);
		void setDisplaySize(const sf::Vector2f &new_size);
		void setDisplayTextureSize(const sf::Vector2u &new_size);

		// Getters
		tDisplay* getDisplayPointer() const;
		std::wstring getCaption() const;
		const int getHeapHeight() const;
		virtual nlohmann::json getParamsInJson() const;
	};

	class tDisplay : public tRenderRect, public tGroup {
	protected:
		tAbstractBasicClass* owner;

	public:
		tDisplay(tAbstractBasicClass* _owner, sf::FloatRect rect = { 0, 0, 100, 100 });
		tDisplay(tAbstractBasicClass* _owner, nlohmann::json& js);
		tDisplay(const tDisplay& d);
		virtual ~tDisplay();

		virtual void draw(sf::RenderTarget& target);
		virtual void handleEvent(tEvent& e);
		virtual void updateTexture();
		virtual bool pointIsInsideMe(sf::Vector2i point) const;

		// Setters
		void setCameraOffset(const sf::Vector2f &new_offset);
		virtual void setOwner(tAbstractBasicClass* new_owner);
		virtual void setTextureSize(const sf::Vector2u &new_size);

		// Getters
		virtual tAbstractBasicClass* getOwner() const;
		virtual sf::FloatRect getLocalBounds() const;
		virtual nlohmann::json getParamsInJson() const;
	};

	class tScrollbar : public tRenderRect {
	public:
		static const int thickness = 24;	// Ширина скролбара
		static const int default_step = 32;	// Шаг прокрутки стрелочками в пикселях

		static const struct sOptionMask {	// Маски операций (переопределено для scrollbar)
			static const unsigned char is_moving = 1;				// Объект перемещается при помощи мыши
			static const unsigned char is_resizing = 2;				// Объект меняет размер при помощи мыши
			static const unsigned char can_be_drawn = 4;			// Можно ли выводить этот объект на экран
			static const unsigned char is_active = 8;				// Активен ли объект
			static const unsigned char vectically_orientated = 16;	// Вертикальный ли scrollbar
			static const unsigned char dummy_2 = 32;				// Бит не задействован
			static const unsigned char can_be_resized = 64;			// Можно ли менять размер объекта при помощи мыши
			static const unsigned char can_be_moved = 128;			// Можно ли перемещать объект при помощи мыши
		} option_mask;

	private:
		void initScrollbar(bool vertical);

	protected:
		tButton* arrow_first, * arrow_second;	// Стрелки
		tRectShape* slider;						// Ползунок
		sf::Vector2f old_position;				// Предыдущая позиция ползунка
		sf::Color color_scroller_area;			// Цвет зоны, по которой бегает ползунок
		sf::Vector2f target_size;				// Размер целевого элемента
		sf::Vector2u target_texture_size;		// Размер текстуры целевого элемента

	public:
		tScrollbar(tAbstractBasicClass* _owner, bool vertical = true, sf::FloatRect rect = { 0, 0, thickness, thickness * 8});
		tScrollbar(tAbstractBasicClass* _owner, nlohmann::json& js);
		tScrollbar(const tScrollbar& s);
		virtual ~tScrollbar();

		void updateScrollerSize();
		virtual void updateTexture();
		virtual void handleEvent(tEvent& e);
		virtual bool pointIsInsideMe(sf::Vector2i point) const;

		// Setters
		void moveSlider(const int &step);
		void setTargetSize(const sf::Vector2f &new_size);
		void setTargetTextureSize(const sf::Vector2u &new_size);
		virtual void setTextureSize(const sf::Vector2u &new_size);

		// Getters
		float getPixelOffset() const;
		virtual nlohmann::json getParamsInJson() const;
	};

}