#include "stdafx.h"
#include "interfaceEngine.h"

namespace edt
{
	tDesktop::tDesktop(std::string path_to_folder, unsigned char _screen_code) :
		tGroup(nullptr),
		custom_font_loaded(false),
		json_configuration(),
		screen_code(_screen_code),
		window_size({ 1280, 720 })
	{
		background.setPosition({ 0, 0 });
		background.setFillColor({ 40, 40, 40, 255 });

		old_mouse_position = sf::Mouse::getPosition();

		this->path_to_folder = path_to_folder;
		std::string config_file_name = "\\Content\\Config\\forms.conf";

		std::ifstream file(path_to_folder + config_file_name);
		try
		{
			if (!file.is_open())
			{
				throw(-1);
			}
			else
			{
				file >> json_configuration;
				file.close();

				std::map<std::string, unsigned char> styles;
				styles["Close"] = sf::Style::Close;
				styles["Default"] = sf::Style::Default;
				styles["Fullscreen"] = sf::Style::Fullscreen;
				styles["None"] = sf::Style::None;
				styles["Resize"] = sf::Style::Resize;
				styles["Titlebar"] = sf::Style::Titlebar;

				std::string style_str = json_configuration["sfml_window"]["style"].get<std::string>();
				unsigned char style = sf::Style::Close;
				if (styles.find(style_str) != styles.end()) {	// Если такой стиль есть, то запоминаем его номер
					style = styles[style_str];
				}

				vec<unsigned int> w_size = json_configuration["sfml_window"]["size"].get<vec<unsigned int>>();;

				background.setSize({ (float)w_size[0], (float)w_size[1] });
				window_size = { w_size[0], w_size[1] };

				window.create(
					sf::VideoMode(w_size[0], w_size[1]),
					json_configuration["sfml_window"]["caption"].get<std::string>(),
					style
				);
				font_default.loadFromFile(path_to_folder + json_configuration["sfml_window"]["font_default"].get<std::string>());

				window.setKeyRepeatEnabled(false);
			}
		}
		catch (int error)
		{
			switch (error)
			{
			case -1:
			{
				std::cout << "Can't open file '" << path_to_folder << config_file_name << "'\n";
				break;
			}
			}
		}
	}

	tDesktop::~tDesktop()
	{
	}

	void tDesktop::run()
	{
		changeScreen(screen_code);

		while (window.isOpen())
		{
			completeEvents();

			window.clear();
			draw(window);
			window.display();

			sf::sleep(sf::milliseconds(5));
		};
		/*
		sf::Vector2u size = window.getSize();
		nlohmann::json js;
		js["menu"] = getParamsInJson();
		js["sfml_window"]["caption"] = "SFML_Game environment editor";
		js["sfml_window"]["size"] = { size.x, size.y };
		js["sfml_window"]["style"] = "Default";
		js["sfml_window"]["font_default"] = "\\Content\\Fonts\\PT Sans.ttf";
		print_json(js, path_to_folder + "\\Content\\Config\\forms.conf");
		/*
		std::fstream file(path_to_folder + "\\Content\\Config\\forms.conf", std::fstream::out);
		file << js;
		file.close();
		*/
	}

	void tDesktop::saveData() const
	{
	}

	void tDesktop::loadCustomFont(std::string path_to_font)
	{
		if (custom_font.loadFromFile(path_to_font))
		{
			custom_font_loaded = true;
		}
		else
		{
			std::cout << "tDesktop.loadcustomFont error: Invalid path_to_font.\n";
		};
	}

	void tDesktop::completeEvents()
	{
		tEvent e;
		getEvent(e);
		while (e.type != tEvent::types.Nothing)
		{
			handleEvent(e);
			getEvent(e);
		}
	}

	bool tDesktop::windowIsOpen() const
	{
		return window.isOpen();
	}

	void tDesktop::changeScreen(char new_screen_code)
	{
		screen_code = new_screen_code;

		saveData();

		// Очистить все подэлементы
		int i = elem.size();
		while (i > 0)
		{
			_delete(elem.back());
			elem.pop_back();
			i--;
		};
		// Очистить список событий
		i = events.size();
		while (i > 0)
		{
			events.pop_back();
			i--;
		};
	}

	void tDesktop::putEvent(tEvent e)
	{
		events.push_back(e);
	}

	void tDesktop::getEvent(tEvent& e)
	{
		if (events.size() != 0)
		{
			e = events.back();
			events.pop_back();
		}
		else
		{
			if (windowIsOpen())
			{
				sf::Event event;
				if (window.pollEvent(event))
				{
					switch (event.type)
					{
					case sf::Event::Closed:
					{	// Окно просит закрыться
						e.type = tEvent::types.Button;
						e.code = tEvent::codes.CloseApplication;
						e.address = this;
						break;
					}
					case sf::Event::KeyPressed:
					case sf::Event::KeyReleased:
					{	// Нажата или отпущена какая-либо кнопка на клавиатуре
						e.type = tEvent::types.Keyboard;
						event.type == sf::Event::KeyPressed ?
							e.key.what_happened = sf::Event::KeyPressed : e.key.what_happened = sf::Event::KeyReleased;
						e.key.button = event.key.code;
						e.key.control = event.key.control;
						e.key.alt = event.key.alt;
						e.key.shift = event.key.shift;
						e.address = this;
						break;
					}
					case sf::Event::MouseButtonPressed:
					case sf::Event::MouseButtonReleased:
					{	// Нажата или отпущена какая-либо кнопка мыши
						e.type = tEvent::types.Mouse;
						e.code = tEvent::codes.MouseButton;
						event.type == sf::Event::MouseButtonPressed ?
							e.mouse.what_happened = sf::Event::MouseButtonPressed : e.mouse.what_happened = sf::Event::MouseButtonReleased;
						e.mouse.button = event.mouseButton.button;
						e.mouse.x = event.mouseButton.x;
						e.mouse.y = event.mouseButton.y;
						e.address = this;
						break;
					}
					case sf::Event::MouseMoved:
					{	// Мышь двинулась куда-то
						e.type = tEvent::types.Mouse;
						e.code = tEvent::codes.MouseMoved;
						e.mouse.x = event.mouseMove.x;
						e.mouse.y = event.mouseMove.y;
						e.mouse.dX = e.mouse.x - old_mouse_position.x;
						e.mouse.dY = e.mouse.y - old_mouse_position.y;
						old_mouse_position = { e.mouse.x , e.mouse.y };
						e.address = this;
						break;
					}
					default:
					{
						e.type = tEvent::types.Nothing;
						break;
					}
					}
				}
				else
				{
					e.type = tEvent::types.Nothing;
				}
			}
		}
	}

	void tDesktop::handleEvent(tEvent& e)
	{
		if (e.type == tEvent::types.Mouse &&
			e.code == tEvent::codes.MouseButton &&
			e.mouse.what_happened == sf::Event::MouseButtonReleased &&
			e.mouse.button == sf::Mouse::Left)
		{	// Если отжата левая кнопка мыши, то останавливаем движение всех элементов
			forEach(tEvent::codes.StopAndDoNotMove);
		}
		tGroup::handleEvent(e);
		switch (e.type)
		{
		case tEvent::types.Broadcast:
		{	// Общего типа
			if (e.address == this)
			{	// Обработка событий для этого объекта
				switch (e.code) {
				case tEvent::codes.Delete:
				{	// Удалить объект
					_delete(e.from);
					clearEvent(e);
					break;
				}
				case tEvent::codes.Activate:
				{	// Установить фокус на объект
					select(e.from);
					((tObject*)e.from)->setOneOption(tObject::option_mask.is_active, true);
					clearEvent(e);
					break;
				}
				case tEvent::codes.Deactivate:
				{	// Снять фокус с объекта
					((tObject*)e.from)->setOneOption(tObject::option_mask.is_active, false);
					clearEvent(e);
					break;
				}
				case tEvent::codes.Show:
				{	// Показать объект (если он скрыт)
					((tObject*)e.from)->setOneOption(tObject::option_mask.can_be_drawn, true);
					clearEvent(e);
					break;
				}
				case tEvent::codes.Hide:
				{	// Спрятать объект (если он не скрыт)
					((tObject*)e.from)->setOneOption(tObject::option_mask.can_be_drawn, false);
					clearEvent(e);
					break;
				}
				case tEvent::codes.Adopt:
				{	// Стать владельцем объекта
					e.from->setOwner(this);
					_insert(e.from);
					clearEvent(e);
					break;
				}
				}
			}
			switch (e.code)
			{	// Обработка событий от "дальних" объектов
			case tEvent::codes.FontRequest:
			{
				e.type = tEvent::types.Broadcast;
				e.code = tEvent::codes.FontAnswer;
				if (!custom_font_loaded)
				{
					e.font.font = &font_default;
				}
				else
				{
					e.font.font = &custom_font;
				}
				e.address = e.from;
				e.from = this;
				message(e);
				break;
			}
			}
			break;
		}
		case tEvent::types.Button:
		{	// От кнопки
			switch (e.code)
			{
			case tEvent::codes.ResetButtons:
			{	// Сбросить состояние кнопок (выделение текста)
				forEach(tEvent::codes.ResetButtons, e.from);
				clearEvent(e);
				break;
			}
			case tEvent::codes.CloseApplication:
			{	// Закрыть приложение
				window.close();
				clearEvent(e);
				break;
			}
			}
			break;
		}
		}
	}

	void tDesktop::updateTexture()
	{
		return;
	}

	void tDesktop::draw(sf::RenderTarget& target)
	{
		target.draw(background);
		tGroup::draw(target);
	}

	const sf::Font& tDesktop::getFont() const
	{
		if (custom_font_loaded)
		{
			return (sf::Font&)custom_font;
		}
		else
		{
			return (sf::Font&)font_default;
		}
	}

	const sf::FloatRect tDesktop::getLocalBounds() const
	{
		return {
			0,
			0,
			(float)window_size.x,
			(float)window_size.y
		};
	}

	const bool tDesktop::pointIsInsideMe(sf::Vector2i point) const
	{
		sf::Vector2i size = (sf::Vector2i)window.getSize();
		return (
			point.x >= 0 && 
			point.x <= size.x && 
			point.y >= 0 && 
			point.y <= size.y
		);
	}

	nlohmann::json tDesktop::getParamsInJson() const
	{
		nlohmann::json js;

		js = tGroup::getParamsInJson();

		js["what_is_it"] = objects_json_ids.tDesktop;
		js["what_is_it_string"] = "tDesktop";

		return js;
	}
}