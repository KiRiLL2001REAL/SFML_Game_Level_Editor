#include "editor.h"
#include <iterator>

namespace edt {

	tObject::tObject() :
		x(0),
		y(0),
		owner(nullptr),
		options(option_mask.can_be_drawn)
	{
		mouse_inside[0] = false;
		mouse_inside[1] = false;
	};

	tObject::~tObject() {
	}

	sf::Vector2f tObject::getPosition() {
		return sf::Vector2f(x, y);
	}

	unsigned char tObject::getOptions() {
		return options;
	}

	bool tObject::checkOption(unsigned char option) {
		return (options & option) == option; // Если результат побитовой конъюнкции совпал с "option", то всё окей
	}

	void tObject::changeOneOption(unsigned char one_option, bool state) {
		if (state) {	// Если нужно взвести бит
			options |= one_option;	// Применяем результат побитового ИЛИ от "options" и параметра "one_option"
			return;
		}
		options &= ~one_option;	// Иначе применяем результат побитового И от "options" и инвертированного параметра "one_option"
	}

	void tObject::setOptions(unsigned char new_options) {
		options = new_options;
	}

	void tObject::move(sf::Vector2f deltaPos) {
		x += deltaPos.x;
		y += deltaPos.y;
	}

	void tObject::setPosition(sf::Vector2f new_position) {
		x = new_position.x;
		y = new_position.y;
	}

	void tObject::setOwner(tObject *new_owner) {
		owner = new_owner;
	}

	sf::Vector2i tObject::getCursorPos() {
		return sf::Vector2i(-1, -1);
	}

	void tObject::message(tObject *addr, int type, int code, tObject *from) {
		tEvent e;
		e.address = addr;
		e.from = from;
		e.type = type; // Из какой сферы событие
		e.code = code; // Код события
		if (addr != nullptr) addr->handleEvent(e);
		else putEvent(e);
	}

	void tObject::clearEvent(tEvent& e) {
		e.type = static_cast<int>(tEvent::types::Nothing);
	}

	void tObject::putEvent(tEvent e) {
		if (this->owner != nullptr)
			this->owner->putEvent(e);
	}

	void tObject::getEvent(tEvent& e) {
		return;
	}

	void tObject::handleEvent(tEvent& e) {
		return;
	}

	void tObject::draw(sf::RenderTarget& target) {
		return;
	}

	void tObject::setSize(sf::Vector2f new_size) {
		return;
	}

	sf::FloatRect tObject::getGlobalBounds() {
		if (owner != nullptr) {
			sf::FloatRect owner_rect = owner->getGlobalBounds();
			sf::FloatRect local_rect = getLocalBounds();
			return {
				owner_rect.left + local_rect.left,
				owner_rect.top + local_rect.top,
				local_rect.width,
				local_rect.height
			};
		}
		return sf::FloatRect( 0, 0, 0, 0 );
	}

	tGroup::tGroup() : 
		elem({})
	{
	}

	tGroup::~tGroup() {
		int i = elem.size();
		while (i != 0) { // Удаление всех элементов в контейнере
			delete elem.back();
			elem.pop_back();
			i--;
		}
	}

	void tGroup::_insert(tObject *object) {
		object->setOwner(this);
		if (elem.size() != 0) {
			elem.back()->changeOneOption(option_mask.is_active, false);
		}
		elem.push_back(object);
		elem.back()->changeOneOption(option_mask.is_active, true);
	}

	bool tGroup::_delete(tObject *object) {
		bool success = false;
		list<tObject*>::iterator it;

		for (it = elem.begin(); it != elem.end(); it++) {	// Поиск удаляемого элемента
			if (*it == object) {
				success = true;
				bool act = false;
				if (*it == elem.back()) {
					act = true;
				}
				delete* it;		// Удаляем его	1) из памяти
				elem.erase(it);	//				2) из контейнера
				elem.back()->changeOneOption(option_mask.is_active, true);
				break;	// Вываливаемся из перебора, чтобы не поймать аксес виолэйшн
			}
		}
		return success;
	}

	void tGroup::select(tObject *object) {
		list<tObject*>::iterator it;
		for (it = elem.begin(); it != elem.end(); it++) {	// Пока не нашли, перебираем список
			if (*it == object) {
				tObject* obj = *it;		// Запоминаем объект
				obj->changeOneOption(option_mask.is_active, true);
				elem.erase(it);			// Удаляем из списка
				elem.back()->changeOneOption(option_mask.is_active, false);
				elem.push_back(obj);	// Кидаем в конец списка
				break;
			}
		}
	}

	void tGroup::forEach(unsigned int code, tObject* from = nullptr) {
		list<tObject*>::iterator it;
		if (!from) {	// Если не сказано от кого событие, то отправителем становится сам почтальон
			from = this;
		}
		for (it = elem.begin(); it != elem.end(); it++) {
			switch (code) {
				case static_cast<int>(tEvent::codes::Deactivate) : {
					message(*it, static_cast<int>(tEvent::types::Broadcast), static_cast<int>(tEvent::codes::Deactivate), from);
					break;
				}
				case static_cast<int>(tEvent::codes::Show) : {
					message(*it, static_cast<int>(tEvent::types::Broadcast), static_cast<int>(tEvent::codes::Show), from);
					break;
				}
				case static_cast<int>(tEvent::codes::Hide) : {
					message(*it, static_cast<int>(tEvent::types::Broadcast), static_cast<int>(tEvent::codes::Hide), from);
					break;
				}
				case static_cast<int>(tEvent::codes::ResetButtons) : {
					message(*it, static_cast<int>(tEvent::types::Broadcast), static_cast<int>(tEvent::codes::ResetButtons), from);
					break;
				}
			}

		}
	}

	void tGroup::draw(sf::RenderTarget& target) {
		list<tObject*>::iterator it;
		for (it = elem.begin(); it != elem.end(); it++) {
			(*it)->draw(target);
		}
	}

	void tGroup::handleEvent(tEvent& e) {
		list<tObject*>::reverse_iterator it;
		for (it = elem.rbegin(); it != elem.rend(); it++) {
			(*it)->handleEvent(e);
		}
	}

	tRenderRect::tRenderRect(sf::FloatRect rect) :
		render_squad(sf::VertexArray(sf::Quads, 4))
	{
		clear_color = sf::Color(0, 0, 0, 255);
		render_texture.create((unsigned int)rect.width, (unsigned int)rect.height);
		render_squad[0].position = sf::Vector2f{ rect.left, rect.top };
		render_squad[1].position = sf::Vector2f{ rect.left + rect.width, rect.top };
		render_squad[2].position = sf::Vector2f{ rect.left + rect.width, rect.top + rect.height };
		render_squad[3].position = sf::Vector2f{ rect.left, rect.top + rect.height };
		render_squad[0].texCoords = sf::Vector2f{ 0, 0 };
		render_squad[1].texCoords = sf::Vector2f{ rect.width - 1, 0 };
		render_squad[2].texCoords = sf::Vector2f{ rect.width - 1, rect.height - 1 };
		render_squad[3].texCoords = sf::Vector2f{ 0, rect.height - 1 };
	}

	tRenderRect::~tRenderRect() {
	}

	void tRenderRect::setTextureSize(sf::Vector2u new_size) {
		render_texture.create(new_size.x, new_size.y);
		render_squad[0].texCoords = sf::Vector2f{ 0.f, 0.f };
		render_squad[1].texCoords = sf::Vector2f{ (float)new_size.x - 1, 0.f };
		render_squad[2].texCoords = sf::Vector2f{ (float)new_size.x - 1, (float)new_size.y - 1 };
		render_squad[3].texCoords = sf::Vector2f{ 0.f, (float)new_size.y - 1 };
	}

	void tRenderRect::setClearColor(sf::Color color) {
		clear_color = color;
	}

	void tRenderRect::handleEvent(tEvent& e) {
		tGroup::handleEvent(e);		
		switch (e.type) {
			case static_cast<int>(tEvent::types::Broadcast) : {
				if (e.address == this) {
					switch (e.code) {
						case static_cast<int>(tEvent::codes::Delete) : {		// Удалить объект
							_delete(e.from);
							clearEvent(e);
							break;
						}
						case static_cast<int>(tEvent::codes::Activate) : {		// Установить фокус на объект
							select(e.from);
							clearEvent(e);
							break;
						}
						case static_cast<int>(tEvent::codes::Deactivate) : {	// Снять фокус с объекта
							e.from->changeOneOption(option_mask.is_active, false);
							clearEvent(e);
							break;
						}
						case static_cast<int>(tEvent::codes::Show) : {			// Показать объект (если он скрыт)
							e.from->changeOneOption(option_mask.can_be_drawn, true);
							clearEvent(e);
							break;
						}
						case static_cast<int>(tEvent::codes::Hide) : {			// Спрятать объект (если он не скрыт)
							e.from->changeOneOption(option_mask.can_be_drawn, false);
							clearEvent(e);
							break;
						}
						case static_cast<int>(tEvent::codes::Adopt) : {			// Стать владельцем объекта
							e.from->setOwner(this);
							clearEvent(e);
							break;
						}
						default: {				// Если не обработалось, то "проталкиваем" на уровень ниже
							e.address = owner;
							putEvent(e);
							clearEvent(e);
							break;
						}
					}
				}
				break;
			}
			case static_cast<int>(tEvent::types::Button) : {
				if (e.address == this) {
					switch (e.code) {
						case static_cast<int>(tEvent::codes::CloseApplication) : {
							e.type = static_cast<int>(tEvent::types::Broadcast);
							e.code = static_cast<int>(tEvent::codes::CloseApplication);
							e.address = owner;
							putEvent(e);
							clearEvent(e);
							break;
						}
						default: {				// Если не обработалось, то "проталкиваем" на уровень ниже
							e.address = owner;
							putEvent(e);
							clearEvent(e);
						}
					}
				}
				break;
			}
		}
		
	}

	void tRenderRect::setSize(sf::Vector2f new_size) {
		render_squad[0].position = sf::Vector2f{ render_squad[0].position.x, render_squad[0].position.y };
		render_squad[1].position = sf::Vector2f{ render_squad[0].position.x + new_size.x, render_squad[0].position.y };
		render_squad[2].position = sf::Vector2f{ render_squad[0].position.x + new_size.x, render_squad[0].position.y + new_size.y };
		render_squad[3].position = sf::Vector2f{ render_squad[0].position.x, render_squad[0].position.y + new_size.y };
	}

	void tRenderRect::setPosition(sf::Vector2f new_position) {
		render_squad[0].position = sf::Vector2f{ new_position.x, new_position.y };
		render_squad[1].position = sf::Vector2f{ new_position.x + render_squad[1].position.x - render_squad[0].position.x, new_position.y + render_squad[1].position.y - render_squad[0].position.y };
		render_squad[2].position = sf::Vector2f{ new_position.x + render_squad[2].position.x - render_squad[0].position.x, new_position.y + render_squad[2].position.y - render_squad[0].position.y };
		render_squad[3].position = sf::Vector2f{ new_position.x + render_squad[3].position.x - render_squad[0].position.x, new_position.y + render_squad[3].position.y - render_squad[0].position.y };
	}

	void tRenderRect::draw(sf::RenderTarget& target) {
		if (checkOption(option_mask.can_be_drawn)) {
			render_texture.clear(clear_color);	// Очиститься
			tGroup::draw(render_texture);		// Нарисовать на себе все подэлементы
			render_texture.display();			// Обновить "лицевую" текстуру
			target.draw(render_squad, &render_texture.getTexture());	// Отобразиться
		}
	}

	void tRenderRect::move(sf::Vector2f delta) {
		sf::Vector2f pos;
		for (char i = 0; i < 4; i++) {
			pos = render_squad[i].position;
			render_squad[i].position = sf::Vector2f{ pos.x + delta.x, pos.y + delta.y };
		}
	}

	bool tRenderRect::pointIsInsideMe(sf::Vector2i point) {
		return false;
	}

	sf::FloatRect tRenderRect::getLocalBounds() {
		return sf::FloatRect(
			render_squad[0].position.x,
			render_squad[0].position.y,
			render_squad[1].position.x - render_squad[0].position.x,
			render_squad[3].position.y - render_squad[0].position.y
		);
	}

	tDesktop::tDesktop(std::string path_to_folder) : 
		custom_font_loaded(false), 
		screen_code(0) 
	{
		old_mouse_position = sf::Mouse::getPosition();

		this->path_to_folder = path_to_folder;
		std::string config_file_name = "\\config.conf";

		std::ifstream file(path_to_folder + config_file_name);
		if (!file.is_open()) {
			std::cout << "Can't open file '" << path_to_folder << config_file_name << "\n";
		}
		else {
			nlohmann::json config;
			file >> config;			
			file.close();

			bool style_close = config.at("window").at("style").get<std::string>() == "close";

			window.create(
				sf::VideoMode(
					config.at("window").at("size").at("width").get<unsigned int>(), 
					config.at("window").at("size").at("height").get<unsigned int>()
				),
				config.at("window").at("caption").get<std::string>(),
				style_close ? sf::Style::Close : sf::Style::Default
			);
			
			font_default.loadFromFile(path_to_folder + config.at("window").at("font_default").get<std::string>());

			window.setKeyRepeatEnabled(false);
		}

	}

	tDesktop::~tDesktop() {
	}

	void tDesktop::run() {
		tEvent e;
		changeScreen(0);
		
		while (window.isOpen()) {
			getEvent(e);
			while (e.type != static_cast<int>(tEvent::types::Nothing)) {
				handleEvent(e);
				getEvent(e);
			}

			window.clear();
			draw(window);
			window.display();

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		};
	}

	bool tDesktop::windowIsOpen() {
		return window.isOpen();
	}

	void tDesktop::changeScreen(char new_screen_code) {
		screen_code = new_screen_code;

		saveData();

		int i = elem.size();	// Очистить все подэлементы
		while (i > 0) {
			_delete(elem.back());
			elem.pop_back();
			i--;
		};
		i = events.size();	// Очистить список событий
		while (i > 0) {
			events.pop_back();
			i--;
		};
	}

	void tDesktop::saveData() {
	}

	void tDesktop::loadCustomFont(std::string path_to_font) {
		if (custom_font.loadFromFile(path_to_font)) {
			custom_font_loaded = true;
		}
		else {
			std::cout << "tDesktop.loadcustomFont error: Invalid path_to_font.\n";
		};
	}

	sf::Font& tDesktop::getFont() {
		if (custom_font_loaded)
			return custom_font;
		else 
			return font_default;
	}

	void tDesktop::putEvent(tEvent e) {
		events.push_back(e);
	}

	void tDesktop::getEvent(tEvent& e) {
		if (events.size() != 0) {
			e = events.back();
			events.pop_back();
		}
		else {
			if (windowIsOpen()) {
				sf::Event event;
				if (window.pollEvent(event)) {
					switch (event.type) {
					case sf::Event::Closed: {				// Окно просит закрыться
						e.type = static_cast<int>(tEvent::types::Broadcast);
						e.code = static_cast<int>(tEvent::codes::CloseApplication);
						e.address = this;
						break;
					}
					case sf::Event::KeyPressed:			// Нажата или отпущена какая-либо кнопка на клавиатуре
					case sf::Event::KeyReleased: {
						e.type = static_cast<int>(tEvent::types::Keyboard);
						event.type == sf::Event::KeyPressed ?
							e.key.what_happened = sf::Event::KeyPressed : e.key.what_happened = sf::Event::KeyReleased;
						e.key.button = event.key.code;
						e.key.control = event.key.control;
						e.key.alt = event.key.alt;
						e.key.shift = event.key.shift;
						e.address = this;
						break;
					}
					case sf::Event::MouseButtonPressed:	// Нажата или отпущена какая-либо кнопка мыши
					case sf::Event::MouseButtonReleased: {
						e.type = static_cast<int>(tEvent::types::Mouse);
						e.code = static_cast<int>(tEvent::codes::MouseButton);
						event.type == sf::Event::MouseButtonPressed ?
							e.mouse.what_happened = sf::Event::MouseButtonPressed : e.mouse.what_happened = sf::Event::MouseButtonReleased;
						e.mouse.button = event.mouseButton.button;
						e.mouse.x = event.mouseButton.x;
						e.mouse.y = event.mouseButton.y;
						e.address = this;
						break;
					}
					case sf::Event::MouseMoved: {			// Мышь двинулась куда-то
						e.type = static_cast<int>(tEvent::types::Mouse);
						e.code = static_cast<int>(tEvent::codes::MouseMoved);
						e.mouse.x = event.mouseMove.x;
						e.mouse.y = event.mouseMove.y;
						e.mouse.dX = e.mouse.x - old_mouse_position.x;
						e.mouse.dY = e.mouse.y - old_mouse_position.y;
						old_mouse_position = { e.mouse.x , e.mouse.y };
						e.address = this;
						break;
					}
					default: {
						e.type = static_cast<int>(tEvent::types::Nothing);
						break;
					}
					}
				}
				else {
					e.type = static_cast<int>(tEvent::types::Nothing);
				}
			}
		}
	}

	void tDesktop::handleEvent(tEvent& e) {
		tGroup::handleEvent(e);
		switch (e.type) {
			case static_cast<int>(tEvent::types::Broadcast) : {	// Общего типа
				if (e.address == this) {			// Обработка событий для этого объекта
					switch (e.code) {
						case static_cast<int>(tEvent::codes::Delete) : {		// Удалить объект
							_delete(e.from);
							clearEvent(e);
							break;
						}
						case static_cast<int>(tEvent::codes::Activate) : {		// Установить фокус на объект
							select(e.from);
							clearEvent(e);
							break;
						}
						case static_cast<int>(tEvent::codes::Deactivate) : {	// Снять фокус с объекта
							e.from->changeOneOption(option_mask.is_active, false);
							clearEvent(e);
							break;
						}
						case static_cast<int>(tEvent::codes::Show) : {			// Показать объект (если он скрыт)
							e.from->changeOneOption(option_mask.can_be_drawn, true);
							clearEvent(e);
							break;
						}
						case static_cast<int>(tEvent::codes::Hide) : {			// Спрятать объект (если он не скрыт)
							e.from->changeOneOption(option_mask.can_be_drawn, false);
							clearEvent(e);
							break;
						}
						case static_cast<int>(tEvent::codes::Adopt) : {			// Стать владельцем объекта
							e.from->setOwner(this);
							_insert(e.from);
							clearEvent(e);
							break;
						}
						case static_cast<int>(tEvent::codes::CloseApplication) : {	// Закрыть приложение
							window.close();
							clearEvent(e);
							break;
						}
					}
				}
				break;
			}
			case static_cast<int>(tEvent::types::Button) : {	// От кнопки
				switch (e.code) {
					case static_cast<int>(tEvent::codes::ResetButtons) : {
						forEach(static_cast<int>(tEvent::codes::ResetButtons), e.from);
						clearEvent(e);
						break;
					}
				}
				break;
			}
		}
	}

	tRectShape::tRectShape(sf::FloatRect rect, sf::Color fill_color) {
		shape.setPosition({rect.left, rect.top});
		shape.setSize({rect.width, rect.height});
		shape.setFillColor(fill_color);
	}
	
	tRectShape::~tRectShape() {
	}

	void tRectShape::setColor(sf::Color new_color) {
		shape.setFillColor(new_color);
	}

	void tRectShape::draw(sf::RenderTarget& target) {
		if (checkOption(option_mask.can_be_drawn)) {
			target.draw(shape);
		}
	}

	void tRectShape::setPosition(sf::Vector2f new_position) {
		tObject::setPosition(new_position);
		shape.setPosition(new_position);
	}

	void tRectShape::setSize(sf::Vector2f new_size) {
		shape.setSize(new_size);
	}

	bool tRectShape::pointIsInsideMe(sf::Vector2i point) {
		return false;
	}

	sf::FloatRect tRectShape::getLocalBounds() {
		return sf::FloatRect(
			shape.getPosition().x,
			shape.getPosition().y,
			shape.getSize().x * shape.getScale().x,
			shape.getSize().y * shape.getScale().y
		);
	}

	tText::tText(sf::Vector2f position, std::string _string) :
		string(_string),
		text_color(sf::Color(255, 255, 255, 255)),
		font_loaded(false),
		char_size(24),
		outline_thickness(1)
	{
		tObject::setPosition(position);
		text_object.setPosition(position);
	}

	tText::~tText() {
	}

	void tText::setString(std::string new_string) {
		string = new_string;
		updateTextObject();
	}

	void tText::setTextColor(sf::Color new_color) {
		text_color = new_color;
		updateTextObject();
	}

	void tText::setFont(sf::Font new_font) {
		font_loaded = true;
		font = new_font;
		updateTextObject();
	}

	void tText::setCharSize(unsigned int new_char_size) {
		char_size = new_char_size;
		updateTextObject();
	}

	void tText::setOutlineThickness(unsigned char new_thickness) {
		outline_thickness = new_thickness;
		updateTextObject();
	}

	void tText::updateTextObject() {
		text_object.setString(string);
		if (font_loaded)
			text_object.setFont(font);
		text_object.setOutlineThickness(outline_thickness);
		text_object.setCharacterSize(char_size);
		text_object.setFillColor(text_color);
	}

	sf::FloatRect tText::getLocalBounds() {
		return text_object.getLocalBounds();
	}

	bool tText::pointIsInsideMe(sf::Vector2i point) {
		return false;
	}

	void tText::draw(sf::RenderTarget& target) {
		if (font_loaded && checkOption(option_mask.can_be_drawn)) {
			target.draw(text_object);
		}
	}

	void tText::setPosition(sf::Vector2f new_position) {
		tObject::setPosition(new_position);
		text_object.setPosition(new_position);
	}

	tButton::tButton(sf::FloatRect rect, std::string text) :
		render_squad(sf::VertexArray(sf::Quads, 4)),
		custom_skin_loaded(false),
		alignment(static_cast<int>(alignment_type::Left)),
		side_offset(10),
		text_offset(sf::Vector2u(0, 0)),
		self_code(static_cast<int>(tEvent::codes::CloseApplication))
	{
		setString(text);
		render_texture.create((unsigned int)rect.width, (unsigned int)rect.height);
		render_squad[0].position = sf::Vector2f{ rect.left, rect.top };
		render_squad[1].position = sf::Vector2f{ rect.left + rect.width, rect.top };
		render_squad[2].position = sf::Vector2f{ rect.left + rect.width, rect.top + rect.height };
		render_squad[3].position = sf::Vector2f{ rect.left, rect.top + rect.height };
		render_squad[0].texCoords = sf::Vector2f{ 0, 0 };
		render_squad[1].texCoords = sf::Vector2f{ rect.width - 1, 0 };
		render_squad[2].texCoords = sf::Vector2f{ rect.width - 1, rect.height - 1 };
		render_squad[3].texCoords = sf::Vector2f{ 0, rect.height - 1 };
		updateTexture();
	}

	tButton::~tButton() {
	}

	void tButton::setSize(sf::Vector2f new_size) {
		render_squad[0].position = sf::Vector2f{ render_squad[0].position.x, render_squad[0].position.y };
		render_squad[1].position = sf::Vector2f{ render_squad[0].position.x + new_size.x, render_squad[0].position.y };
		render_squad[2].position = sf::Vector2f{ render_squad[0].position.x + new_size.x, render_squad[0].position.y + new_size.y };
		render_squad[3].position = sf::Vector2f{ render_squad[0].position.x, render_squad[0].position.y + new_size.y };
	}

	void tButton::setTextureSize(sf::Vector2u new_size) {
		render_texture.create(new_size.x, new_size.y);
		render_squad[0].texCoords = sf::Vector2f{ 0, 0 };
		render_squad[1].texCoords = sf::Vector2f{ (float)new_size.x - 1, 0 };
		render_squad[2].texCoords = sf::Vector2f{ (float)new_size.x - 1, (float)new_size.y - 1 };
		render_squad[3].texCoords = sf::Vector2f{ 0, (float)new_size.y - 1 };
	}

	void tButton::updateTexture() {
		render_texture.clear(sf::Color(0, 0, 0, 255));
		if (custom_skin_loaded) {	// Если загружен пользовательский скин кнопки, то выводим его
			sf::Sprite spr;
			spr.setTexture(custom_skin);
			spr.setPosition(sf::Vector2f(0, 0));
			render_texture.draw(spr);
		}
		else {						// Иначе - рисуем стандартную кнопку
			sf::Vector2f tex_size = (sf::Vector2f)render_texture.getSize();
			float offset = (float)side_offset / 2;
			sf::VertexArray arr(sf::Quads, 4);
			sf::Color front_color = sf::Color(150, 150, 150, 255);
			sf::Color top_color = sf::Color(120, 120, 120, 255);
			sf::Color side_color = sf::Color(70, 70, 70, 255);
			sf::Color bottom_color = sf::Color(20, 20, 20, 255);
			for (int i = 0; i < 4; i++)						// Лицевая сторона
				arr[i].color = front_color;
			arr[0].position = sf::Vector2f(offset, offset);
			arr[1].position = sf::Vector2f(tex_size.x - offset, offset);
			arr[2].position = sf::Vector2f(tex_size.x - offset, tex_size.y - offset);
			arr[3].position = sf::Vector2f(offset, tex_size.y - offset);
			render_texture.draw(arr);
			for (int i = 0; i < 4; i++)						// Верхняя сторона
				arr[i].color = top_color;
			arr[0].position = sf::Vector2f(0, 0);
			arr[1].position = sf::Vector2f(tex_size.x, 0);
			arr[2].position = sf::Vector2f(tex_size.x - offset, offset);
			arr[3].position = sf::Vector2f(offset, offset);
			render_texture.draw(arr);
			for (int i = 0; i < 4; i++)						// Боковые стороны
				arr[i].color = side_color;
			arr[0].position = sf::Vector2f(0, 0);
			arr[1].position = sf::Vector2f(offset, offset);
			arr[2].position = sf::Vector2f(offset, tex_size.y - offset);
			arr[3].position = sf::Vector2f(0, tex_size.y);
			render_texture.draw(arr);
			arr[0].position = sf::Vector2f(tex_size.x - offset, offset);
			arr[1].position = sf::Vector2f(tex_size.x, 0);
			arr[2].position = sf::Vector2f(tex_size.x, tex_size.y);
			arr[3].position = sf::Vector2f(tex_size.x - offset, tex_size.y - offset);
			render_texture.draw(arr);
			for (int i = 0; i < 4; i++)						// Нижжняя сторона
				arr[i].color = bottom_color;
			arr[0].position = sf::Vector2f(offset, tex_size.y - offset);
			arr[1].position = sf::Vector2f(tex_size.x - offset, tex_size.y - offset);
			arr[2].position = sf::Vector2f(tex_size.x, tex_size.y);
			arr[3].position = sf::Vector2f(0, tex_size.y);
			render_texture.draw(arr);
		}
		if (font_loaded) {				// Если подгружен шрифт, то выводим текст
			updateTextObject();
			sf::Text text_to_display = text_object;
			mouse_inside[0] ? text_to_display.setStyle(sf::Text::Style::Bold) : text_to_display.setStyle(sf::Text::Style::Regular);	// При наведении на кнопку мышью, текст подчёркивается
			text_to_display.setOutlineThickness(outline_thickness);
			switch (alignment) {			// Настройка выравнивания
				case static_cast<int>(tButton::alignment_type::Right) : {
					text_to_display.setOrigin(sf::Vector2f((float)text_to_display.getLocalBounds().width, (float)text_to_display.getLocalBounds().height));
					text_to_display.setPosition(sf::Vector2f((float)render_texture.getSize().x - side_offset - text_offset.x, (float)render_texture.getSize().y / 2 + text_offset.y));
					break;
				}
				case static_cast<int>(tButton::alignment_type::Middle) : {
					text_to_display.setOrigin(sf::Vector2f(text_to_display.getLocalBounds().width / 2, (float)text_to_display.getLocalBounds().height));
					text_to_display.setPosition(sf::Vector2f((float)render_texture.getSize().x / 2 + text_offset.x, (float)render_texture.getSize().y / 2 + text_offset.y));
					break;
				}
				case static_cast<int>(tButton::alignment_type::Left) :
				default: {
					text_to_display.setOrigin(sf::Vector2f(0, (float)text_to_display.getLocalBounds().height));
					text_to_display.setPosition(sf::Vector2f((float)side_offset + text_offset.x, (float)render_texture.getSize().y / 2 + text_offset.x));
					break;
				}
			}
			text_to_display.setFillColor(sf::Color::Black);	// Немного контраста
			text_to_display.move(sf::Vector2f(1, 1));
			render_texture.draw(text_to_display);
			text_to_display.setFillColor(text_color);		// А это уже сам вывод текста
			text_to_display.move(sf::Vector2f(-1, -1));
			render_texture.draw(text_to_display);
		}
		render_texture.display();
	}

	void tButton::loadCustomSkin(std::string path_to_skin) {
		if (custom_skin.loadFromFile(path_to_skin)) {
			custom_skin_loaded = true;
			setSize(sf::Vector2f((float)custom_skin.getSize().x, (float)custom_skin.getSize().y));
			setTextureSize(sf::Vector2u(custom_skin.getSize().x, custom_skin.getSize().y));
			updateTexture();
		}
		else {
			std::cout << "tButton.loadCustomSkin error: Invalid path_to_skin.\n";
		}
	}

	void tButton::setAlignment(char new_alignment) {
		switch (new_alignment) {
			case static_cast<int>(tButton::alignment_type::Middle) :
				case static_cast<int>(tButton::alignment_type::Right) : {
				alignment = new_alignment;
				break;
			}
			case static_cast<int>(tButton::alignment_type::Left) : {
			default:
				alignment = static_cast<int>(tButton::alignment_type::Left);
				break;
			}
		}
		updateTexture();
	}

	void tButton::setTextOffset(sf::Vector2i new_offset) {
		text_offset = new_offset;
		updateTexture();
	}

	void tButton::setCode(int new_code) {
		self_code = new_code;
	}

	bool tButton::pointIsInsideMe(sf::Vector2i point) {
		sf::FloatRect rect = getGlobalBounds();
		return (point.x >= rect.left && point.x <= rect.left + rect.width && point.y >= rect.top && point.y <= rect.top + rect.height);
	}

	sf::FloatRect tButton::getLocalBounds() {
		return
			sf::FloatRect(
				render_squad[0].position.x,
				render_squad[0].position.y,
				render_squad[1].position.x - render_squad[0].position.x,
				render_squad[3].position.y - render_squad[0].position.y
			);
	}

	void tButton::draw(sf::RenderTarget& target) {
		if (checkOption(option_mask.can_be_drawn)) {
			target.draw(render_squad, &render_texture.getTexture());
		}
	}

	void tButton::setPosition(sf::Vector2f new_position) {
		tObject::setPosition(new_position);
		render_squad[1].position = sf::Vector2f{ new_position.x + render_squad[1].position.x - render_squad[0].position.x, new_position.y + render_squad[1].position.y - render_squad[0].position.y };
		render_squad[2].position = sf::Vector2f{ new_position.x + render_squad[2].position.x - render_squad[0].position.x, new_position.y + render_squad[2].position.y - render_squad[0].position.y };
		render_squad[3].position = sf::Vector2f{ new_position.x + render_squad[3].position.x - render_squad[0].position.x, new_position.y + render_squad[3].position.y - render_squad[0].position.y };
		render_squad[0].position = sf::Vector2f{ new_position.x, new_position.y };
	}

	void tButton::handleEvent(tEvent& e) {
		switch (e.type) {
			case static_cast<int>(tEvent::types::Broadcast) : {
				switch (e.code) {
					case static_cast<int>(tEvent::codes::ResetButtons) : {
						if (e.from != this) {
							mouse_inside[0] = false;
							updateTexture();
						}
						break;
					}
				}
				break;
			}
			case static_cast<int>(tEvent::types::Mouse) : {
				switch (e.code) {
					case static_cast<int>(tEvent::codes::MouseMoved) : {
						mouse_inside[1] = mouse_inside[0];	// Предыдущее и текущее состояние флага
						mouse_inside[0] = pointIsInsideMe({ e.mouse.x, e.mouse.y });
						if (mouse_inside[0] != mouse_inside[1]) {	// Если произошло изменение, то генерируем текстуру заново с подчёркнутым текстом
							message(nullptr, static_cast<int>(tEvent::types::Button), static_cast<int>(tEvent::codes::ResetButtons), this);
							updateTexture();
							clearEvent(e);
						}
						break;
					}
					case static_cast<int>(edt::tEvent::codes::MouseButton) : {
						if (e.mouse.what_happened == sf::Event::MouseButtonReleased && e.mouse.button == sf::Mouse::Left &&
							pointIsInsideMe({ e.mouse.x, e.mouse.y }))	// Если левая кнопка мыши отпущена, и мышь находится внутри кнопки, то передаём послание
						{
							message(owner, static_cast<int>(edt::tEvent::types::Button), self_code, this);
							clearEvent(e);
						}
						break;
					}
				}
				break;
			}
		}
	}


	tWindow::tWindow(sf::FloatRect rect, std::string _caption) :
		font_loaded(false),
		caption(_caption),
		color_heap(sf::Color(100, 100, 100, 255)),
		color_space(sf::Color(80, 80, 80, 255)),
		color_caption_active(sf::Color(255, 255, 255, 255)),
		color_caption_inactive(sf::Color(150, 150, 150, 255)),
		caption_offset({2, 2})
	{
		changeOneOption(option_mask.can_be_moved, true);
		changeOneOption(option_mask.can_be_resized, true);

		setPosition({rect.left, rect.top});
		setSize({rect.width, rect.height});
		setTextureSize({(unsigned int)rect.width, (unsigned int)rect.height});
	}

	tWindow::~tWindow() {
	}

	void tWindow::initWindow() {
		sf::FloatRect rect = getLocalBounds();
		tRectShape *r = new tRectShape({ 0, 0, rect.width, heap_height }, color_heap);	// Шапка
		_insert(r);

		r = new tRectShape({ 0, heap_height, rect.width, rect.height - heap_height }, color_space);	// Рабочее пространство
		_insert(r);

		tButton *b = new tButton({ rect.width - heap_height + 2, 2, heap_height - 4, heap_height - 4 }, "");
		b->setCode(static_cast<int>(edt::tEvent::codes::Close));
		b->setString("x");
		b->setFont(font);
		b->setTextColor({255, 255, 255, 255});
		b->setCharSize(20);
		b->setOutlineThickness(1);
		b->setAlignment(static_cast<int>(tButton::alignment_type::Middle));
		b->setTextOffset({0, -2});
		b->updateTexture();
		_insert(b);
	}

	void tWindow::setCaption(std::string new_caption) {
		caption = new_caption;
	}

	std::string tWindow::getCaption() {
		return caption;
	}

	void tWindow::setColorHeap(sf::Color new_color) {
		color_heap = new_color;
	}

	void tWindow::setColorSpace(sf::Color new_color) {
		color_space = new_color;
	}

	void tWindow::setColorCaptionActive(sf::Color new_color) {
		color_caption_active = new_color;
	}

	void tWindow::setColorCaptionInactive(sf::Color new_color) {
		color_caption_inactive = new_color;
	}

	void tWindow::setFont(sf::Font new_font) {
		font_loaded = true;
		font = new_font;
	}

	void tWindow::setCaptionOffset(sf::Vector2f new_offset) {
		caption_offset = new_offset;
	}

	sf::FloatRect tWindow::getLocalBounds() {
		return sf::FloatRect(
				render_squad[0].position.x,
				render_squad[0].position.y,
				render_squad[1].position.x - render_squad[0].position.x,
				render_squad[3].position.y - render_squad[0].position.y
			);
	}

	const int tWindow::getHeapHeight() {
		return heap_height;
	}

	bool tWindow::pointIsInHeap(sf::Vector2i point) {
		sf::FloatRect rect = getGlobalBounds();
		return (point.x >= rect.left && point.x <= rect.left + rect.width && point.y >= rect.top && point.y <= rect.top + heap_height);
	}

	bool tWindow::pointIsInsideMe(sf::Vector2i point) {
		sf::FloatRect rect = getGlobalBounds();
		return (point.x >= rect.left && point.x <= rect.left + rect.width && point.y >= rect.top && point.y <= rect.top + rect.height);
	}

	void tWindow::draw(sf::RenderTarget& target) {
		if (checkOption(option_mask.can_be_drawn)) {
			render_texture.clear();
			tGroup::draw(render_texture);

			if (font_loaded) {
				sf::Text text;
				text.setString(caption);
				text.setCharacterSize(caption_char_size);
				text.setFont(font);
				text.setFillColor(checkOption(option_mask.is_active) ? color_caption_active : color_caption_inactive);
				text.setPosition(caption_offset);
				text.setOutlineThickness(1);
				render_texture.draw(text);
			}

			render_texture.display();
			target.draw(render_squad, &render_texture.getTexture());
		}
	}

	void tWindow::handleEvent(tEvent& e) {
		tGroup::handleEvent(e);
		switch (e.type) {
			case static_cast<int>(tEvent::types::Broadcast) : {
				if (e.address == this) {
					switch (e.code) {
						case static_cast<int>(tEvent::codes::Delete) : {		// Удалить объект
							_delete(e.from);
							clearEvent(e);
							break;
						}
						case static_cast<int>(tEvent::codes::Activate) : {		// Установить фокус на объект
							select(e.from);
							clearEvent(e);
							break;
						}
						case static_cast<int>(tEvent::codes::Deactivate) : {	// Снять фокус с объекта
							e.from->changeOneOption(option_mask.is_active, false);
							clearEvent(e);
							break;
						}
						case static_cast<int>(tEvent::codes::Show) : {			// Показать объект (если он скрыт)
							e.from->changeOneOption(option_mask.can_be_drawn, true);
							clearEvent(e);
							break;
						}
						case static_cast<int>(tEvent::codes::Hide) : {			// Спрятать объект (если он не скрыт)
							e.from->changeOneOption(option_mask.can_be_drawn, false);
							clearEvent(e);
							break;
						}
						case static_cast<int>(tEvent::codes::Adopt) : {			// Стать владельцем объекта
							e.from->setOwner(this);
							clearEvent(e);
							break;
						}
						default: {				// Если не обработалось, то "проталкиваем" на уровень ниже
							e.address = owner;
							putEvent(e);
							clearEvent(e);
							break;
						}
					}
				}
				break;
			}
			case static_cast<int>(tEvent::types::Button) : {
				if (e.address == this) {
					switch (e.code) {
						case static_cast<int>(tEvent::codes::Close) : {			// Закрыть окно
							e.type = static_cast<int>(tEvent::types::Broadcast);
							e.code = static_cast<int>(tEvent::codes::Delete);
							e.address = owner;
							e.from = this;
							putEvent(e);
							clearEvent(e);
							break;
						}
						default: {				// Если не обработалось, то "проталкиваем" на уровень ниже
							e.address = owner;
							putEvent(e);
							clearEvent(e);
						}
					}
				}
				break;
			}
			case static_cast<int>(tEvent::types::Mouse) : {
				switch (e.code) {
					case static_cast<int>(tEvent::codes::MouseButton) : {
						if (e.mouse.what_happened == sf::Event::MouseButtonPressed && e.mouse.button == sf::Mouse::Left && pointIsInsideMe({ e.mouse.x, e.mouse.y })) {
							if (pointIsInHeap({ e.mouse.x, e.mouse.y }) && checkOption(option_mask.can_be_moved)) {
								changeOneOption(option_mask.is_moving, true);
							}
							message(owner, static_cast<int>(tEvent::types::Broadcast), static_cast<int>(tEvent::codes::Activate), this);
							clearEvent(e);
						}
						else if (e.mouse.what_happened == sf::Event::MouseButtonReleased) {
							changeOneOption(option_mask.is_moving, false);
						}
						break;
					}
					case static_cast<int>(tEvent::codes::MouseMoved) : {
						if (pointIsInsideMe({ e.mouse.x, e.mouse.y })) {
							message(nullptr, static_cast<int>(tEvent::types::Broadcast), static_cast<int>(tEvent::codes::ResetButtons), this);
							clearEvent(e);
						}
						if (checkOption(option_mask.is_moving)) {
							move({ (float)e.mouse.dX, (float)e.mouse.dY });
						}
						break;
					}
				}
				break;
			}
		}
	}

	tMoveable::tMoveable() {
		//movement.active = false;
		movement.mX = 0;
		movement.mY = 0;
	}

	tMoveable::~tMoveable() {
	}

	void tMoveable::setMovementStates(sMovement new_movement) {
		movement = new_movement;
	}

	tMoveable::sMovement tMoveable::getMovementStates() {
		return movement;
	}

}