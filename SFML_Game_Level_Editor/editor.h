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

	struct sMovement { // Используется при перемещении объекта мышью
		bool active;		// Отвечает за "прилипание" данного объекта к курсору мыши
		int mX, mY;			// Смещение мыши относительно верхнего левого угла объекта
	};

	struct tEvent {
		enum class types { Nothing, Mouse, Keyboard, Broadcast, Button };

		enum class codes {
			Activate, Deactivate, Show, Hide,
			Move, Adopt, Delete, Close, CloseApplication,
			MouseMoved, MouseButton, ResetButtons
		};

		unsigned int type = static_cast<int>(types::Nothing); // Из какой сферы событие (тип)
		unsigned int code = 0; // Код события (опционально)
		tObject *from = nullptr;
		tObject *address = nullptr;

		struct sMouse { // Событие от мыши
			char button = 0;
			char what_happened = 0;
			int x = 0;
			int y = 0;
		} mouse;
		struct sKey { // Событие от клавиатуры
			char button = 0;
			int what_happened = 0;
			bool control = false;
			bool alt = false;
			bool shift = false;
		} key;
		struct sText { // Вывод текста
			std::string string = "";
			char char_size = 12;
			int x = 0;
			int y = 0;
			sf::Color color = sf::Color(255, 255, 255, 255);
		} text;
	};

	class tObject { // Базовый класс
	protected:
		bool mouse_inside[2];	// Флаг. Находится ли мышь внутри кнопки?

		float x, y;				// Координаты объекта
		tObject *owner;			// Указатель на владельца

		bool can_be_drawn;		// Флаг. Может ли данный объект быть нарисованным?
		bool is_active;			// Флаг. Активен ли данный элемент?
		bool can_be_moved;		// Флаг. Может ли объект быть сдвинутым?

		sMovement movement;		// Структура-помощник при событии перетаскивания мышью

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

	class tGroup : public tObject { // Класс-контейнер
	protected:
		list<tObject*> elem;		// Контейнер элементов, хранящихся в данном классе

	public:
		tGroup();
		virtual ~tGroup();

		void _insert(tObject *object);		// Внесение элемента в список подэлементов
		bool _delete(tObject *object);		// Удаление элемента из списка
		void select(tObject *object);		// Установка флага "активен" у элемента
		void forEach(unsigned int code, tObject* from);	// Выполнить команду для всех подэлементов

		virtual void draw(sf::RenderTarget& target);
		virtual void handleEvent(tEvent& e);
		virtual void makeVisible(bool flag);
	};

	class tRenderRect : public tGroup {
	protected:
		sf::VertexArray render_squad;		// Фигура, в которой выполняется отрисовка
		sf::RenderTexture render_texture;	// Текстура, в которой отрисовываются объекты
		sf::Color clear_color;				// Цвет очистки текстуры

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
		std::string path_to_folder;	// Путь до рабочей папки
		sf::RenderWindow window;	// Главное окно
		sf::Font font_default;		// Шрифт по умолчанию
		sf::Font custom_font;		// Пользовательский шрифт
		bool custom_font_loaded;	// Флаг. Загружен ли пользовательский шрифт?
		list<tEvent> events;		// Список событий к обработке

		char screen_code;			// Код текущего экрана
	
	public:
		tDesktop(std::string path_to_folder);
		~tDesktop();

		void run();								// Главный цикл
		bool windowIsOpen();					// Возвращает статус окна
		void saveData();						// Сохранить данные в файл
		void loadCustomFont(std::string path_to_font);	// Установить пользовательский шрифт
		sf::Font& getFont();					// Получить шрифт

		virtual void changeScreen(char new_screen_code);// Изменить экран (меню, редактор карты, редактор NPC)
		virtual void putEvent(tEvent e);
		virtual void getEvent(tEvent& e);
		virtual void handleEvent(tEvent& e);
	};

	class tText : public tObject {
	protected:
		sf::Text text_object;				// SFML текст
		std::string string;					// Текст сообщения
		sf::Color text_color;				// Цвет теста
		sf::Font font;						// Шрифт текста
		bool font_loaded;					// Флаг. Загружен ли шрифт?
		unsigned int char_size;				// Размер шрифта
		unsigned char outline_thickness;	// Толщина линии обводки текста

	public:
		tText(sf::Vector2f position = {0, 0}, std::string string = "Some text");
		virtual ~tText();

		void setString(std::string new_string);
		void setTextColor(sf::Color new_color);
		void setFont(sf::Font new_font);
		void setCharSize(unsigned int new_char_size);
		void setOutlineThickness(unsigned char new_thickness);
		void updateTextObject();	// Не использовать в классе tButton

		virtual sf::FloatRect getLocalBounds();

		virtual void draw(sf::RenderTarget& target);
		virtual void setPosition(sf::Vector2f new_position);
	};

	class tButton : public tText {		
	protected:
		const unsigned char side_offset;

		int self_code;						// Код, который посылает кнопка при нажатии на неё

		sf::RenderTexture render_texture;	// Текстура
		sf::VertexArray render_squad;		// Прямоугольник отрисовки
		bool custom_skin_loaded;			// Флаг. Загружен ли пользовательский скин кнопки?
		sf::Texture custom_skin;			// Пользовательский скин кпопки
		char alignment;						// Тип выравнивания
		sf::Vector2i text_offset;			// Настройка смещения текста, в случае, если он криво выводится (это всё из-за шрифтов)
		
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
		bool font_loaded;	// Флаг. Загружен ли шрифт?
		sf::Font font;		// Шрифт

		std::string caption;				// Заголовок окна
		sf::Color color_heap;				// Цвет шапки
		sf::Color color_space;				// Цвет основной части
		sf::Color color_caption_active;		// Цвет заголовка активного окна
		sf::Color color_caption_inactive;	// Цвет заголовка неактивного окна
		sf::Vector2f caption_offset;		// Настройка смещения заголовка, в случае, если он криво выводится (это всё из-за шрифтов)

	public:
		tWindow(sf::FloatRect rect = { 0, 0, 300, 300 }, std::string caption = "Default caption" );	// Необходимо так же вызвать метод initWindow()
		virtual ~tWindow();

		void initWindow();	// Обязательно к выполнению после вызова конструктора

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