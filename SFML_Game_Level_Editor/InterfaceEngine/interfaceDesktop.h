#pragma once

namespace edt {
	class tDesktop : public tGroup {
	protected:
		std::string path_to_folder;			// Путь до рабочей папки
		sf::RenderWindow window;			// Главное окно
		sf::Font font_default;				// Шрифт по умолчанию
		sf::Font custom_font;				// Пользовательский шрифт
		bool custom_font_loaded;			// Флаг. Загружен ли пользовательский шрифт?
		std::list<tEvent> events;			// Список событий к обработке
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
}