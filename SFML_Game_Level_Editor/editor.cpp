#include "stdafx.h"
#include "editor.h"

namespace edt {

	tObject::tObject(tObject* _owner) :
		anchor(0b00001001),
		x(0),
		y(0),
		owner(_owner),
		options(option_mask.can_be_drawn)
	{
		mouse_inside[0] = false;
		mouse_inside[1] = false;
	}

	tObject::tObject(tObject* _owner, nlohmann::json& js) :
		tObject(_owner)
	{
		anchor = js["anchor"].get<unsigned char>();

		options = js["options"].get<unsigned char>();
		
		std::vector<float> vf = js["position"].get<std::vector<float>>();
		x = vf[0];
		y = vf[1];
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

	void tObject::setAnchor(unsigned char new_anchor) {
		anchor = new_anchor;
	}

	unsigned char tObject::getAnchor() {
		return anchor;
	}

	void tObject::move(sf::Vector2f deltaPos) {
		x += deltaPos.x;
		y += deltaPos.y;
	}

	void tObject::setPosition(sf::Vector2f new_position) {
		sf::Vector2f offset = getRelativeStartPosition();
		x = offset.x + new_position.x;
		y = offset.y + new_position.y;
	}

	void tObject::setOwner(tObject *new_owner) {
		owner = new_owner;
	}

	void tObject::message(tObject *addr, int type, int code, tObject *from) {
		tEvent e;
		e.address = addr;
		e.from = from;
		e.type = type; // Из какой сферы событие
		e.code = code; // Код события
		if (addr) addr->handleEvent(e);
		else putEvent(e);
	}

	void tObject::message(tEvent e) {
		if (e.address) e.address->handleEvent(e);
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

	sf::Vector2f tObject::getRelativeStartPosition() {
		unsigned char i = 0;
		sf::FloatRect owner_rect;
		sf::Vector2f offset = { 0, 0 };

		//if (owner) 
		owner_rect = owner->getLocalBounds();
		//else return {0, 0};

		unsigned char anchor = this->anchor;
		for (i = 0; i < 3; i++) {
			if (anchor & 1) {
				offset.x = (owner_rect.width / 2) * i;
				break;
			}
			else anchor = anchor >> 1;
		}
		anchor = anchor >> (3 - i);
		for (i = 0; i < 3; i++) {
			if (anchor & 1) {
				offset.y = (owner_rect.height / 2) * i;
				break;
			}
			else anchor = anchor >> 1;
		}

		return offset;
	}

	nlohmann::json tObject::saveParamsInJson() {
		nlohmann::json js;

		js["position"] = { x, y };
		js["anchor"] = anchor;
		js["options"] = options;

		return js;
	}

	tGroup::tGroup(tObject* _owner) : 
		tObject(_owner),
		elem({})
	{
	}

	tGroup::tGroup(tObject* _owner, nlohmann::json& js) :
		tObject(_owner, js),
		elem({})
	{
		makeObjectsFromJson(js);
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
		if (elem.size() != 0) {
			elem.back()->changeOneOption(option_mask.is_active, false);
			elem.back()->updateTexture();
		}
		elem.push_back(object);
		elem.back()->changeOneOption(option_mask.is_active, true);
		elem.back()->updateTexture();
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
				elem.back()->updateTexture();
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
				elem.back()->updateTexture();
				elem.push_back(obj);	// Кидаем в конец списка
				break;
			}
		}
	}

	void tGroup::forEach(unsigned int code, tObject* from = nullptr) {
		list<tObject*>::reverse_iterator it;
		if (!from) {	// Если не сказано от кого событие, то отправителем становится сам почтальон
			from = this;
		}
		for (it = elem.rbegin(); it != elem.rend(); it++) {
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

	void tGroup::makeObjectsFromJson(nlohmann::json& js) {
		for (nlohmann::json::iterator it = js["elem"].begin(); it != js["elem"].end(); it++) {
			unsigned char what_is_it = (*it)["what_is_it"].get<unsigned char>();
			switch (what_is_it) {
			case (objects_json_ids.tText): {
				tText* el = new tText(this, *it);
				_insert(el);
				break;
			}
			case (objects_json_ids.tButton): {
				tButton* el = new tButton(this, *it);
				_insert(el);
				break;
			}
			case (objects_json_ids.tRectShape): {
				tRectShape* el = new tRectShape(this, *it);
				_insert(el);
				break;
			}
			case (objects_json_ids.tWindow): {
				tWindow* el = new tWindow(this, *it);
				_insert(el);
				break;
			}
			}
		}
	}

	nlohmann::json tGroup::saveParamsInJson() {
		nlohmann::json js = tObject::saveParamsInJson();

		unsigned int id = 0;
		for (list<tObject*>::iterator it = elem.begin(); it != elem.end(); it++) {
			std::string str = std::to_string(id);
			js["elem"][str] = (*it)->saveParamsInJson();
			id++;
		}

		return js;
	}

	tRenderRect::tRenderRect(tObject* _owner, sf::FloatRect rect) :
		tGroup(_owner),
		render_squad(sf::VertexArray(sf::Quads, 4)),
		clear_color({ 0, 0, 0, 255 })
	{
		tObject::setPosition({ rect.left, rect.top });
		render_texture.create((unsigned int)rect.width, (unsigned int)rect.height);

		setTextureSize({ (unsigned int)rect.width, (unsigned int)rect.height });
		setSize({ rect.width, rect.height });
	}

	tRenderRect::tRenderRect(tObject* _owner, nlohmann::json& js) :
		tGroup(_owner, js),
		render_squad(sf::VertexArray(sf::Quads, 4))
	{

		std::vector<unsigned char> vuc = js["clear_color"].get<std::vector<unsigned char>>();
		clear_color = {vuc[0], vuc[1], vuc[2], vuc[3]};

		std::vector<float> vf = js["size"].get<std::vector<float>>();
		setSize({ vf[0], vf[1] });

		std::vector<unsigned int> vui = js["texture_size"].get<std::vector<unsigned int>>();
		render_texture.create(vui[0], vui[1]);
		setTextureSize({ vui[0], vui[1] });
	}

	tRenderRect::~tRenderRect() {
	}

	void tRenderRect::setTextureSize(sf::Vector2u new_size) {
		render_texture.create(new_size.x, new_size.y);
		render_squad[0].texCoords = { 0.f, 0.f };
		render_squad[1].texCoords = { (float)new_size.x - 1, 0.f };
		render_squad[2].texCoords = { (float)new_size.x - 1, (float)new_size.y - 1 };
		render_squad[3].texCoords = { 0.f, (float)new_size.y - 1 };
	}

	void tRenderRect::setClearColor(sf::Color color) {
		clear_color = color;
	}

	void tRenderRect::setSize(sf::Vector2f new_size) {
		render_squad[0].position = { x, y };
		render_squad[1].position = { x + new_size.x, y };
		render_squad[2].position = { x + new_size.x, y + new_size.y };
		render_squad[3].position = { x, y + new_size.y };
	}

	void tRenderRect::setPosition(sf::Vector2f new_position) {
		tObject::setPosition(new_position);
		render_squad[1].position = { x + render_squad[1].position.x - render_squad[0].position.x, y + render_squad[1].position.y - render_squad[0].position.y };
		render_squad[2].position = { x + render_squad[2].position.x - render_squad[0].position.x, y + render_squad[2].position.y - render_squad[0].position.y };
		render_squad[3].position = { x + render_squad[3].position.x - render_squad[0].position.x, y + render_squad[3].position.y - render_squad[0].position.y };
		render_squad[0].position = { x, y };
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

	sf::FloatRect tRenderRect::getLocalBounds() {
		return sf::FloatRect(
			render_squad[0].position.x,
			render_squad[0].position.y,
			render_squad[1].position.x - render_squad[0].position.x,
			render_squad[3].position.y - render_squad[0].position.y
		);
	}

	nlohmann::json tRenderRect::saveParamsInJson() {
		nlohmann::json js = tGroup::saveParamsInJson();

		sf::Vector2f size = render_squad[2].position - render_squad[0].position;
		sf::Vector2u tex_size = render_texture.getSize();

		js["size"] = { size.x, size.y };
		js["texture_size"] = { tex_size.x, tex_size.y };
		js["clear_color"] = { clear_color.r, clear_color.g, clear_color.b, clear_color.a };

		return js;
	}

	tDesktop::tDesktop(std::string path_to_folder) :
		tGroup(nullptr),
		custom_font_loaded(false),
		json_configuration(),
		screen_code(0),
		window_size({1280, 720})
	{
		background.setPosition({ 0, 0 });
		background.setFillColor({40, 40, 40, 255});

		old_mouse_position = sf::Mouse::getPosition();

		this->path_to_folder = path_to_folder;
		std::string config_file_name = "\\Content\\Config\\menu.conf";

		std::ifstream file(path_to_folder + config_file_name);
		try {
			if (!file.is_open()) {
				throw(-1);
			}
			else {
				file >> json_configuration;
				file.close();

				std::map<std::string, unsigned char> styles;
				styles["Close"] = sf::Style::Close;
				styles["Default"] = sf::Style::Default;
				styles["Fullscreen"] = sf::Style::Fullscreen;
				styles["None"] = sf::Style::None;
				styles["Resize"] = sf::Style::Resize;
				styles["Titlebar"] = sf::Style::Titlebar;

				std::string style_str = json_configuration["window"]["style"].get<std::string>();
				unsigned char style = sf::Style::Close;
				if (styles.find(style_str) != styles.end()) {	// Если такой стиль есть, то запоминаем его номер
					style = styles[style_str];
				}

				std::vector<unsigned int> w_size = json_configuration["window"]["size"].get<std::vector<unsigned int>>();;

				background.setSize({ (float)w_size[0], (float)w_size[1] });
				window_size = { w_size[0], w_size[1] };

				window.create(
					sf::VideoMode(w_size[0], w_size[1]),
					json_configuration["window"]["caption"].get<std::string>(),
					style
				);
				font_default.loadFromFile(path_to_folder + json_configuration["window"]["font_default"].get<std::string>());

				window.setKeyRepeatEnabled(false);
			}
		}
		catch (int error) {
			switch (error) {
			case -1: {
				std::cout << "Can't open file '" << path_to_folder << config_file_name << "'\n";
				break;
			}
			}
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
					}
				}
				switch (e.code) {	// Обработка событий от "дальних" объектов
					case static_cast<int>(tEvent::codes::FontRequest) : {
						e.type = static_cast<int>(tEvent::types::Broadcast);
						e.code = static_cast<int>(tEvent::codes::FontAnswer);
						if (!custom_font_loaded) e.font.font = &font_default;
						else e.font.font = &custom_font;
						e.address = e.from;
						e.from = this;
						message(e);
						message(e.address, static_cast<int>(tEvent::types::Broadcast), static_cast<int>(tEvent::codes::UpdateTexture), this);
						break;
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
					case static_cast<int>(tEvent::codes::CloseApplication) : {	// Закрыть приложение
						window.close();
						clearEvent(e);
						break;
					}
				}
				break;
			}
		}
	}

	void tDesktop::updateTexture() {
		return;
	}

	void tDesktop::draw(sf::RenderTarget& target) {
		target.draw(background);
		tGroup::draw(target);
	}

	sf::FloatRect tDesktop::getLocalBounds() {
		return {
			0,
			0,
			(float)window_size.x,
			(float)window_size.y
		};
	}

	bool tDesktop::pointIsInsideMe(sf::Vector2i point) {
		sf::Vector2i size = (sf::Vector2i)window.getSize();
		return (point.x >= 0 && point.x <= size.x && point.y >= 0 && point.y <= size.y);
	}

	nlohmann::json tDesktop::saveParamsInJson() {
		nlohmann::json js;
		
		sf::Vector2u size = window.getSize();

		js = tGroup::saveParamsInJson();

		js["what_is_it"] = objects_json_ids.tDesktop;
		js["what_is_it_string"] = "tDesktop";

		js["window"]["caption"] = "SFML_Game environment editor";
		js["window"]["size"] = { size.x, size.y };
		js["window"]["style"] = "Titlebar";
		js["window"]["font_default"] = "\\Content\\Fonts\\PT Sans.ttf";

		return js;
	}

	tRectShape::tRectShape(tObject* _owner, sf::FloatRect rect, sf::Color fill_color) :
		tObject(_owner)
	{
		shape.setPosition({rect.left, rect.top});
		shape.setSize({rect.width, rect.height});
		shape.setFillColor(fill_color);
	}

	tRectShape::tRectShape(tObject* _owner, nlohmann::json& js) :
		tObject(_owner, js)
	{		
		std::vector<float> vf = js["size"].get<std::vector<float>>();
		shape.setSize({ vf[0], vf[1] });

		std::vector<unsigned char> vuc = js["color"].get<std::vector<unsigned char>>();
		shape.setFillColor({ vuc[0], vuc[1], vuc[2], vuc[3] });

		shape.setPosition({ x, y });
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

	void tRectShape::updateTexture() {
		return;
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

	nlohmann::json tRectShape::saveParamsInJson() {
		nlohmann::json js = tObject::saveParamsInJson();

		sf::Vector2f size = shape.getSize();
		sf::Color color = shape.getFillColor();

		js["what_is_it"] = objects_json_ids.tRectShape;
		js["what_is_it_string"] = "tRectShape";
		js["size"] = { size.x, size.y };
		js["color"] = { color.r, color.g, color.b, color.a };

		return js;
	}

	tText::tText(tObject* _owner, sf::Vector2f position, std::wstring string) :
		tObject(_owner),
		font_loaded(false)
	{
		tObject::setPosition(position);
		text_object.setString(string);
		text_object.setFillColor({ 255, 255, 255, 255 });
		text_object.setCharacterSize(24);
		text_object.setOutlineThickness(1);
		text_object.setPosition(position);
	}

	tText::tText(tObject* _owner, nlohmann::json& js) :
		tObject(_owner, js),
		font_loaded(false)
	{
		std::vector<int> vi = js["text"].get<std::vector<int>>();
		text_object.setString(convertIntVectorToWstring(vi));
		
		std::vector<unsigned char> vuc = js["color"].get<std::vector<unsigned char>>();
		text_object.setFillColor({ vuc[0], vuc[1], vuc[2], vuc[3] });

		text_object.setCharacterSize(js["char_size"].get<unsigned int>());

		text_object.setOutlineThickness((float)js["outline_thickness"].get<unsigned int>());

		text_object.setPosition({ x, y });
	}

	tText::~tText() {
	}

	void tText::setString(std::wstring new_string) {
		text_object.setString(new_string);
	}

	void tText::setTextColor(sf::Color new_color) {
		text_object.setFillColor(new_color);
	}

	void tText::setFont(sf::Font new_font) {
		font_loaded = true;
		font = new_font;
		text_object.setFont(font);
	}

	void tText::setCharSize(unsigned int new_char_size) {
		text_object.setCharacterSize(new_char_size);
	}

	void tText::setOutlineThickness(unsigned char new_thickness) {
		text_object.setOutlineThickness(new_thickness);
	}

	bool tText::getFontState() {
		return font_loaded;
	}

	sf::Text& tText::getTextObject() {
		return text_object;
	}

	sf::Color tText::getFillColor() {
		return text_object.getFillColor();
	}

	sf::FloatRect tText::getLocalBounds() {
		return text_object.getLocalBounds();
	}

	bool tText::pointIsInsideMe(sf::Vector2i point) {
		return false;
	}

	nlohmann::json tText::saveParamsInJson() {
		nlohmann::json js = tObject::saveParamsInJson();

		sf::Color color = text_object.getFillColor();
		std::wstring text = (std::wstring)text_object.getString();
		unsigned int char_size = text_object.getCharacterSize();
		unsigned int thickness = (unsigned int)text_object.getOutlineThickness();

		js["what_is_it"] = objects_json_ids.tText;
		js["what_is_it_string"] = "tText";
		js["color"] = { color.r, color.g, color.b, color.a };
		js["char_size"] = char_size;
		js["outline_thickness"] = thickness;
		js["text"] = text;

		return js;
	}

	void tText::draw(sf::RenderTarget& target) {
		if (checkOption(option_mask.can_be_drawn)) {
			if (font_loaded) {
				target.draw(text_object);
				return;
			}
			message(nullptr, static_cast<int>(tEvent::types::Broadcast), static_cast<int>(tEvent::codes::FontRequest), this);
		}
	}

	void tText::setPosition(sf::Vector2f new_position) {
		tObject::setPosition(new_position);
		text_object.setPosition(new_position);
	}

	void tText::handleEvent(tEvent& e) {
		if (checkOption(option_mask.can_be_drawn)) {
			switch (e.type) {
				case static_cast<int>(tEvent::types::Broadcast) : {
					if (e.address == this) {
						switch (e.code) {
							case static_cast<int>(tEvent::codes::FontAnswer) : {
								font_loaded = true;
								text_object.setFont(*e.font.font);
								break;
							}
						}
					}
					break;
				}
			}
		}
	}

	void tText::updateTexture() {
		return;
	}

	tButton::tButton(tObject* _owner, sf::FloatRect rect) :
		tRenderRect(_owner, rect),
		custom_skin_loaded(false),
		alignment(static_cast<int>(text_alignment_type::Left)),
		side_offset(10),
		text_offset(sf::Vector2u(0, 0)),
		self_code(static_cast<int>(tEvent::codes::Nothing))
	{
		initButton();
	}

	tButton::tButton(tObject* _owner, nlohmann::json& js) :
		tRenderRect(_owner, js),
		custom_skin_loaded(false),
		side_offset(10)
	{
		std::vector<int> vi = js["text_offset"].get<std::vector<int>>();
		text_offset = { vi[0], vi[1] };

		self_code = js["code"].get<int>();

		alignment = js["alignment"].get<char>();
	}

	tButton::~tButton() {
	}

	void tButton::updateTexture() {
		render_texture.clear(clear_color);
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

			sf::Color front_color = sf::Color(150, 150, 150, 255);	// 0--------------------------------1
			sf::Color top_color = sf::Color(120, 120, 120, 255);	// |\                              /|
			sf::Color side_color = sf::Color(70, 70, 70, 255);		// | 4----------------------------5 |
			sf::Color bottom_color = sf::Color(20, 20, 20, 255);	// | |                            | |
																	// | |                            | |
			sf::Vector2f point[] = {								// | |                            | |
				{0, 0},												// | 7----------------------------6 |
				{tex_size.x, 0},									// |/                              \|
				{tex_size.x, tex_size.y},							// 3--------------------------------2
				{0, tex_size.y},
				{offset, offset},
				{tex_size.x - offset, offset},
				{tex_size.x - offset, tex_size.y - offset},
				{offset, tex_size.y - offset}
			};
			
			for (int i = 0; i < 4; i++) {					// Лицевая сторона
				arr[i].color = front_color;
				arr[i].position = point[4 + i];
			}
			render_texture.draw(arr);
			for (int i = 0; i < 4; i++)						// Верхняя сторона
				arr[i].color = top_color;
			arr[0].position = point[0];
			arr[1].position = point[1];
			arr[2].position = point[5];
			arr[3].position = point[4];
			render_texture.draw(arr);
			for (int i = 0; i < 4; i++)						// Боковые стороны
				arr[i].color = side_color;
			arr[0].position = point[0];
			arr[1].position = point[4];
			arr[2].position = point[7];
			arr[3].position = point[3];
			render_texture.draw(arr);
			arr[0].position = point[5];
			arr[1].position = point[1];
			arr[2].position = point[2];
			arr[3].position = point[6];
			render_texture.draw(arr);
			for (int i = 0; i < 4; i++)						// Нижжняя сторона
				arr[i].color = bottom_color;
			arr[0].position = point[7];
			arr[1].position = point[6];
			arr[2].position = point[2];
			arr[3].position = point[3];
			render_texture.draw(arr);
		}
		tText* text_object = (tText*)elem.front();
		if (text_object->getFontState()) {				// Если подгружен шрифт, то выводим текст
			sf::Text text_to_display = text_object->getTextObject();
			mouse_inside[0] ? text_to_display.setStyle(sf::Text::Style::Bold) : text_to_display.setStyle(sf::Text::Style::Regular);	// При наведении на кнопку мышью, текст подчёркивается
			switch (alignment) {			// Настройка выравнивания
				case static_cast<int>(tButton::text_alignment_type::Right) : {
					text_to_display.setOrigin({ (float)text_to_display.getLocalBounds().width, (float)text_to_display.getLocalBounds().height });
					text_to_display.setPosition({ (float)render_texture.getSize().x - side_offset - text_offset.x, (float)render_texture.getSize().y / 2 + text_offset.y });
					break;
				}
				case static_cast<int>(tButton::text_alignment_type::Middle) : {
					text_to_display.setOrigin({ text_to_display.getLocalBounds().width / 2, (float)text_to_display.getLocalBounds().height });
					text_to_display.setPosition({ (float)render_texture.getSize().x / 2 + text_offset.x, (float)render_texture.getSize().y / 2 + text_offset.y });
					break;
				}
				case static_cast<int>(tButton::text_alignment_type::Left) :
				default: {
					text_to_display.setOrigin({ 0, (float)text_to_display.getLocalBounds().height });
					text_to_display.setPosition({ (float)side_offset + text_offset.x, (float)render_texture.getSize().y / 2 + text_offset.x });
					break;
				}
			}
			text_to_display.setFillColor(sf::Color::Black);	// Немного контраста
			text_to_display.move({ 1, 1 });
			render_texture.draw(text_to_display);
			text_to_display.setFillColor(text_object->getFillColor());		// А это уже сам вывод текста
			text_to_display.move({ -1, -1 });
			render_texture.draw(text_to_display);
		}
		else {
			message(nullptr, static_cast<int>(tEvent::types::Broadcast), static_cast<int>(tEvent::codes::FontRequest), this);
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

	void tButton::setTextAlignment(char new_alignment) {
		switch (new_alignment) {
			case static_cast<int>(tButton::text_alignment_type::Middle) :
			case static_cast<int>(tButton::text_alignment_type::Right) : {
			alignment = new_alignment;
			break;
			}
			case static_cast<int>(tButton::text_alignment_type::Left) : {
			default:
				alignment = static_cast<int>(tButton::text_alignment_type::Left);
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

	void tButton::initButton() {
		elem.push_back(new tText(this));
	}

	void tButton::setFont(sf::Font new_font) {
		tText* text = (tText*)elem.front();
		text->setFont(new_font);
	}

	void tButton::setString(std::wstring new_string) {
		tText* text = (tText*)elem.front();
		text->setString(new_string);
	}

	void tButton::setTextColor(sf::Color new_color) {
		tText* text = (tText*)elem.front();
		text->setTextColor(new_color);
	}

	void tButton::setCharSize(unsigned int new_char_size) {
		tText* text = (tText*)elem.front();
		text->setCharSize(new_char_size);
	}

	void tButton::setOutlineThickness(unsigned char new_thickness) {
		tText* text = (tText*)elem.front();
		text->setOutlineThickness(new_thickness);
	}

	bool tButton::pointIsInsideMe(sf::Vector2i point) {
		sf::FloatRect rect = getGlobalBounds();
		return (point.x >= rect.left && point.x <= rect.left + rect.width && point.y >= rect.top && point.y <= rect.top + rect.height);
	}

	nlohmann::json tButton::saveParamsInJson() {
		nlohmann::json js = tRenderRect::saveParamsInJson();

		js["what_is_it"] = objects_json_ids.tButton;
		js["what_is_it_string"] = "tButton";
		js["code"] = self_code;
		js["alignment"] = alignment;
		js["text_offset"] = { text_offset.x, text_offset.y };

		return js;
	}

	void tButton::draw(sf::RenderTarget& target) {
		if (checkOption(option_mask.can_be_drawn)) {
			target.draw(render_squad, &render_texture.getTexture());
		}
	}

	void tButton::handleEvent(tEvent& e) {
		if (checkOption(option_mask.can_be_drawn)) {
			switch (e.type) {
				case static_cast<int>(tEvent::types::Broadcast) : {
					if (e.address == this) {	// Для конкретно этой кнопки
						switch (e.code) {
							case static_cast<int>(tEvent::codes::FontAnswer) : {	// Забрать выданный системой шрифт
								setFont(*e.font.font);
								clearEvent(e);
								break;
							}
							case static_cast<int>(tEvent::codes::UpdateTexture) : {	// Обновить текстуру
								updateTexture();
								clearEvent(e);
								break;
							}
						}
					}
					switch (e.code) {			// Для всех остальных
						case static_cast<int>(tEvent::codes::ResetButtons) : {
							if (e.from != this && e.from != owner) {
								mouse_inside[0] = false;
								mouse_inside[1] = false;
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
								message(owner, static_cast<int>(tEvent::types::Broadcast), static_cast<int>(tEvent::codes::UpdateTexture), this);
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
	}


	tWindow::tWindow(tObject* _owner, sf::FloatRect rect, std::wstring _caption) :
		tRenderRect(_owner),
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

		initWindow();
	}

	tWindow::tWindow(tObject* _owner, nlohmann::json& js) :
		tRenderRect(_owner, js),
		font_loaded(false)
	{
		std::vector<int> vi = js["caption"].get<std::vector<int>>();
		caption = convertIntVectorToWstring(vi);

		std::vector<float> vf = js["caption_offset"].get<std::vector<float>>();
		caption_offset = { vf[0], vf[1] };

		std::vector<unsigned char> vuc = js["color_heap"].get<std::vector<unsigned char>>();
		color_heap = { vuc[0], vuc[1], vuc[2], vuc[3] };
		vuc.clear();

		vuc = js["color_space"].get<std::vector<unsigned char>>();
		color_space = { vuc[0], vuc[1], vuc[2], vuc[3] };
		vuc.clear();

		vuc = js["color_caption_active"].get<std::vector<unsigned char>>();
		color_caption_active = { vuc[0], vuc[1], vuc[2], vuc[3] };
		vuc.clear();

		vuc = js["color_caption_inactive"].get<std::vector<unsigned char>>();
		color_caption_inactive = { vuc[0], vuc[1], vuc[2], vuc[3] };
		vuc.clear();

		changeOneOption(option_mask.can_be_moved, true);
		changeOneOption(option_mask.can_be_resized, true);

		initWindow();
	}

	tWindow::~tWindow() {
	}

	void tWindow::initWindow() {
		sf::FloatRect rect = getLocalBounds();
		
		tRectShape *r = new tRectShape(this, { 0, 0, rect.width, heap_height }, color_heap);	// Шапка
		_insert(r);

		r = new tRectShape(this, { 0, heap_height, rect.width, rect.height - heap_height }, color_space);	// Рабочее пространство
		_insert(r);

		tButton* b = new tButton(this, {0, 0, heap_height - 4 , heap_height - 4 });
		b->setAnchor(tObject::anchors.upper_right_corner);
		b->setPosition({ -heap_height + 2, 2});
		b->setCode(static_cast<int>(edt::tEvent::codes::Close));
		if (!font_loaded) {
			message(nullptr, static_cast<int>(tEvent::types::Broadcast), static_cast<int>(tEvent::codes::FontRequest), this);
		}
		else {
			b->setFont(font);
		}
		b->setString(L"x");
		b->setTextColor({ 255, 255, 255, 255 });
		b->setCharSize(20);
		b->setOutlineThickness(1);
		b->setTextAlignment(static_cast<int>(tButton::text_alignment_type::Middle));
		b->setTextOffset({ 0, -3 });
		b->updateTexture();
		_insert(b);
	}

	void tWindow::setCaption(std::wstring new_caption) {
		caption = new_caption;
	}

	std::wstring tWindow::getCaption() {
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

	void tWindow::updateTexture() {
		render_texture.clear(clear_color);
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
		else {	// Если шрифта нет, мы должны запросить его, и после получения обновить текстуру
			message(nullptr, static_cast<int>(tEvent::types::Broadcast), static_cast<int>(tEvent::codes::FontRequest), this);
			tEvent e;
			e.type = static_cast<int>(tEvent::types::Broadcast);
			e.code = static_cast<int>(tEvent::codes::UpdateTexture);
			e.from = this;
			e.address = this;
			putEvent(e);
		}

		sf::VertexArray frame(sf::LineStrip, 5);	// Рисование обводки окна
		sf::FloatRect rect = getLocalBounds();
		sf::Color color = { 140, 140, 140, 255 };
		frame[0].position = { 1, 0 };
		frame[1].position = { rect.width - 1, 0 };
		frame[2].position = { rect.width - 1, rect.height - 2 };
		frame[3].position = { 1, rect.height - 2 };
		frame[4].position = { 1, 1 };
		if (checkOption(tObject::option_mask.is_active)) color = { 0, 122, 204, 255 };
		for (int i = 0; i < 5; i++) frame[i].color = color;
		render_texture.draw(frame);

		render_texture.display();
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

	nlohmann::json tWindow::saveParamsInJson() {
		nlohmann::json js = tRenderRect::saveParamsInJson();

		js["what_is_it"] = objects_json_ids.tWindow;
		js["what_is_it_string"] = "tWindow";

		js["caption"] = caption;
		js["caption_offset"] = { caption_offset.x, caption_offset.y };
		js["color_heap"] = { color_heap.r, color_heap.g, color_heap.b, color_heap.a };
		js["color_space"] = { color_space.r, color_space.g, color_space.b, color_space.a };
		js["color_caption_active"] = { color_caption_active.r, color_caption_active.g, color_caption_active.b, color_caption_active.a };
		js["color_caption_inactive"] = { color_caption_inactive.r, color_caption_inactive.g, color_caption_inactive.b, color_caption_inactive.a };

		return js;
	}

	void tWindow::draw(sf::RenderTarget& target) {
		if (checkOption(option_mask.can_be_drawn)) {
			target.draw(render_squad, &render_texture.getTexture());
		}
	}

	void tWindow::handleEvent(tEvent& e) {
		if (checkOption(option_mask.can_be_drawn)) {
			tGroup::handleEvent(e);
			switch (e.type) {
				case static_cast<int>(tEvent::types::Broadcast) : {
					if (e.address == this) {	// Для конкретно этого окна
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
							case static_cast<int>(tEvent::codes::UpdateTexture) : {	// Обновить текстуру
								updateTexture();
								clearEvent(e);
								break;
							}
							case static_cast<int>(tEvent::codes::FontAnswer) : {	// Забрать выданный системой шрифт
								font_loaded = true;
								setFont(*e.font.font);
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
					if (e.address == this) {	// Для конкретно этого окна
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
							if (e.mouse.button == sf::Mouse::Left && pointIsInsideMe({ e.mouse.x, e.mouse.y })) {
								if (e.mouse.what_happened == sf::Event::MouseButtonPressed) {
									// Если в окно тыкнули левой кнопкой мыши
									if (pointIsInHeap({ e.mouse.x, e.mouse.y }) && checkOption(option_mask.can_be_moved)) {
										changeOneOption(option_mask.is_moving, true);
									}
									message(owner, static_cast<int>(tEvent::types::Broadcast), static_cast<int>(tEvent::codes::Activate), this);
									updateTexture();
									clearEvent(e);
								}
								else {
									// Если отпустили кнопку
									if (e.mouse.what_happened == sf::Event::MouseButtonReleased) {
										changeOneOption(option_mask.is_moving, false);
										clearEvent(e);
									}
								}
							}
							break;
						}
						case static_cast<int>(tEvent::codes::MouseMoved) : {
							mouse_inside[1] = mouse_inside[0];
							mouse_inside[0] = pointIsInsideMe({ e.mouse.x, e.mouse.y });
							if (mouse_inside[0]) {
								message(nullptr, static_cast<int>(tEvent::types::Broadcast), static_cast<int>(tEvent::codes::ResetButtons), this);
								updateTexture();
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
	}

}