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

		unsigned int type = static_cast<int>(types::Nothing); // Из какой сферы событие (тип)
		unsigned int code = static_cast<int>(codes::Nothing); // Код события
		tObject *from = nullptr;
		tObject *address = nullptr;

		struct sMouse { // Событие от мыши
			char button = 0;
			char what_happened = 0;
			int x = 0;
			int y = 0;
			int dX = 0;
			int dY = 0;
		} mouse;
		struct sKey { // Событие от клавиатуры
			char button = 0;
			int what_happened = 0;
			bool control = false;
			bool alt = false;
			bool shift = false;
		} key;
		struct sText { // Вывод текста
			std::wstring string = L"";
			char char_size = 12;
			int x = 0;
			int y = 0;
			sf::Color color = sf::Color(255, 255, 255, 255);
		} text;
		struct sFont {	// Запрос шрифта
			sf::Font* font = nullptr;
		} font;
	};

	class tObject { // Базовый класс
	protected:
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
		
		unsigned char anchor;	// Биты 0..2 отвечают за привязку по X, а биты 3..5 - за привязку по Y (см tObject::anchors)

		bool mouse_inside[2];	// Флаг. Находится ли мышь внутри кнопки?

		float x, y;				// Смещение относительно начальной точки (см. anchor)
		tObject *owner;			// Указатель на владельца

		unsigned char options;	// Битовая штука. Смотри "tObject::option_mask"

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
		virtual sf::Vector2f getRelativeStartPosition();	// Возвращает начальную точку системы координат (ориентируясь на якорь)
		virtual bool pointIsInsideMe(sf::Vector2i point) = 0;
		virtual nlohmann::json saveParamsInJson();
	};

	class tGroup : public tObject { // Класс-контейнер
	protected:
		list<tObject*> elem;		// Контейнер элементов, хранящихся в данном классе

	public:
		tGroup(tObject* _owner);
		tGroup(tObject* _owner, nlohmann::json& js);
		tGroup(const tGroup& g);
		virtual ~tGroup();

		void _insert(tObject *object);		// Внесение элемента в список подэлементов
		void select(tObject *object);		// Установка флага "активен" у элемента
		void forEach(unsigned int code, tObject* from);	// Выполнить команду для всех подэлементов
		
		bool _delete(tObject *object);		// Удаление элемента из списка

		virtual void draw(sf::RenderTarget& target);
		virtual void handleEvent(tEvent& e);
		void makeObjectsFromJson(tObject* _owner, nlohmann::json& js);

		virtual nlohmann::json saveParamsInJson();
	};

	class tRenderRect : public tGroup {
	protected:
		sf::VertexArray render_squad;		// Фигура, в которой выполняется отрисовка
		sf::RenderTexture render_texture;	// Текстура, в которой отрисовываются объекты
		sf::Color clear_color;				// Цвет очистки текстуры
		
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
		void saveData();						// Сохранить данные в файл
		void loadCustomFont(std::string path_to_font);	// Установить пользовательский шрифт

		sf::Font& getFont();					// Получить шрифт
		bool windowIsOpen();					// Возвращает статус окна

		virtual void changeScreen(char new_screen_code);// Изменить экран (меню, редактор карты, редактор NPC и т.д.)
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
		sf::Text text_object;				// SFML текст
		sf::Font font;						// Шрифт текста
		bool font_loaded;					// Флаг. Загружен ли шрифт?

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

		bool getFontState();	// Загружен или нет
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

		int self_code;						// Код, который посылает кнопка при нажатии на неё
		bool custom_skin_loaded;			// Флаг. Загружен ли пользовательский скин кнопки?
		char alignment;						// Тип выравнивания
		sf::Texture custom_skin;			// Пользовательский скин кпопки
		sf::Vector2i text_offset;			// Настройка смещения текста, в случае, если он криво выводится (это всё из-за шрифтов)
		
		tText text;							// Текст внутри кнопки

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
		bool font_loaded;	// Флаг. Загружен ли шрифт?
		sf::Font font;		// Шрифт

		std::wstring caption;				// Заголовок окна
		sf::Color color_heap;				// Цвет шапки
		sf::Color color_area;				// Цвет основной части
		sf::Color color_caption_active;		// Цвет заголовка активного окна
		sf::Color color_caption_inactive;	// Цвет заголовка неактивного окна
		sf::Vector2f caption_offset;		// Настройка смещения заголовка, в случае, если он криво выводится (это всё из-за шрифтов)

		tButton button_close;				// Кнопка закрытия
		tRectShape heap_shape;				// Фигура шапки
		tRectShape area_shape;				// Фигура рабочей области

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
		static const struct sOrientation {	// Определитель ориентации ползунка
			static const unsigned char vertical = 0;	// Вертикальная ориентация
			static const unsigned char horizontal = 1;	// Горизонтальная ориентация
		} orientation_types;

	protected:
		int minimum;				// Минимальное значение смещения
		int maximum;				// Максимальное значение смещения
		unsigned char orientation;	// Ориентация ползунка
		sf::Color color_space;		// Цвет закраски области скролл-бара
		sf::Color color_slider;		// Цвет ползунка

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