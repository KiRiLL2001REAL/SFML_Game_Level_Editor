#include "stdafx.h"
#include "editor.h"

#define vec std::vector

namespace edt {

	tAbstractBasicClass::tAbstractBasicClass(tAbstractBasicClass* _owner) :
		owner(_owner)
	{
	}

	tAbstractBasicClass::tAbstractBasicClass(const tAbstractBasicClass& a) :
		owner(a.owner)
	{
	}

	tAbstractBasicClass::~tAbstractBasicClass()
	{
	}

	void tAbstractBasicClass::setOwner(tAbstractBasicClass* new_owner) {
		owner = new_owner;
	}

	void tAbstractBasicClass::clearEvent(tEvent& e) {
		e.type = tEvent::types.Nothing;
	}

	void tAbstractBasicClass::message(tAbstractBasicClass* addr, unsigned int type, unsigned int code, tAbstractBasicClass* from) {
		tEvent e;
		e.address = addr;
		e.from = from;
		e.type = type; // Из какой сферы событие
		e.code = code; // Код события
		if (addr) addr->handleEvent(e);
		else putEvent(e);
	}

	void tAbstractBasicClass::message(tEvent e) {
		if (e.address) e.address->handleEvent(e);
		else putEvent(e);
	}

	tAbstractBasicClass* tAbstractBasicClass::getOwner() const {
		return owner;
	}

	void tAbstractBasicClass::putEvent(tEvent e) {
		if (owner != nullptr)
			owner->putEvent(e);
	}

	void tAbstractBasicClass::getEvent(tEvent& e) {
		return;
	}

	nlohmann::json tAbstractBasicClass::getParamsInJson() const {
		return nlohmann::json();
	}

	sf::FloatRect tAbstractBasicClass::getLocalBounds() const {
		return sf::FloatRect(
			0,
			0,
			1.f,
			1.f
		);
	}

	sf::FloatRect tAbstractBasicClass::getGlobalBounds() const {
		if (owner != nullptr) {
			sf::FloatRect owner_rect = getOwner()->getGlobalBounds();
			sf::FloatRect local_rect = getLocalBounds();

			return {
				owner_rect.left + local_rect.left,
				owner_rect.top + local_rect.top,
				local_rect.width,
				local_rect.height
			};
		}
		return sf::FloatRect(0, 0, 0, 0);
	}

	tObject::tObject(tAbstractBasicClass* _owner) :
		tAbstractBasicClass(_owner),
		anchor(0b00001001),
		x(0),
		y(0),
		options(option_mask.can_be_drawn)
	{
		mouse_inside[0] = false;
		mouse_inside[1] = false;
	}

	tObject::tObject(tAbstractBasicClass* _owner, nlohmann::json& js) :
		tObject(_owner)
	{
		anchor = js["anchor"].get<unsigned char>();

		options = js["options"].get<unsigned char>();
		
		vec<float> vf = js["position"].get<vec<float>>();
		
		setPosition({ vf[0], vf[1] });
	}

	tObject::tObject(const tObject& o) :
		tAbstractBasicClass(o),
		anchor(o.anchor),
		x(o.x),
		y(o.y),
		options(o.options)
	{
		mouse_inside[0] = false;
		mouse_inside[1] = false;
	}

	tObject::~tObject() {
	}

	sf::Vector2f tObject::getPosition() const {
		return sf::Vector2f(x, y);
	}

	unsigned char tObject::getOptions() const {
		return options;
	}

	bool tObject::checkOption(unsigned char option) const {
		return (options & option) == option; // Если результат побитовой конъюнкции совпал с "option", то всё окей
	}

	void tObject::changeOneOption(const unsigned char &one_option, const bool &state) {
		if (state) {	// Если нужно взвести бит
			options |= one_option;	// Применяем результат побитового ИЛИ от "options" и параметра "one_option"
			return;
		}
		options &= ~one_option;	// Иначе применяем результат побитового И от "options" и инвертированного параметра "one_option"
	}

	void tObject::setOptions(const unsigned char &new_options) {
		options = new_options;
	}

	void tObject::setAnchor(const unsigned char &new_anchor) {
		anchor = new_anchor;
	}

	unsigned char tObject::getAnchor() const {
		return anchor;
	}

	void tObject::move(sf::Vector2f deltaPos) {
		x += deltaPos.x;
		y += deltaPos.y;
	}

	void tObject::setPosition(const sf::Vector2f &new_position) {
		x = new_position.x;
		y = new_position.y;
	}

	void tObject::draw(sf::RenderTarget& target) {
		return;
	}

	void tObject::setSize(const sf::Vector2f &new_size) {
		return;
	}

	sf::Vector2f tObject::getRelativeStartPosition() const {
		unsigned char i = 0;
		sf::FloatRect owner_rect;
		sf::Vector2f offset = { 0, 0 };

		owner_rect = getOwner()->getLocalBounds();

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

	bool tObject::pointIsInsideMe(sf::Vector2i point) const {
		return false;
	}

	nlohmann::json tObject::getParamsInJson() const {
		nlohmann::json js;

		js["position"] = { x, y };
		js["anchor"] = anchor;
		js["options"] = options;

		return js;
	}

	tGroup::tGroup(tAbstractBasicClass* _owner) :
		tAbstractBasicClass(_owner),
		elem({})
	{
	}

	tGroup::tGroup(tAbstractBasicClass* _owner, nlohmann::json& js) :
		tAbstractBasicClass(_owner),
		elem({})
	{
		makeObjectsFromJson(_owner, js);
	}

	tGroup::tGroup(const tGroup& g) :
		tAbstractBasicClass(g),
		elem(g.elem)
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

	void tGroup::_insert(tAbstractBasicClass *object) {
		if (elem.size() != 0) {
			((tObject*)elem.back())->changeOneOption(tObject::option_mask.is_active, false);
			message(elem.back(), tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
		}
		elem.push_back(object);
		((tObject*)elem.back())->changeOneOption(tObject::option_mask.is_active, true);
		message(elem.back(), tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
	}

	bool tGroup::_delete(tAbstractBasicClass *object) {
		bool success = false;
		list<tAbstractBasicClass*>::iterator it;
		for (it = elem.begin(); it != elem.end(); it++) {	// Поиск удаляемого элемента
			if (*it == object) {
				success = true;
				bool act = false;
				if (*it == elem.back()) {
					act = true;
				}
				delete* it;		// Удаляем его	1) из памяти
				elem.erase(it);	//				2) из контейнера
				((tObject*)elem.back())->changeOneOption(tObject::option_mask.is_active, true);
				message(elem.back(), tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
				break;	// Вываливаемся из перебора, чтобы не поймать аксес виолэйшн
			}
		}
		return success;
	}

	void tGroup::select(tAbstractBasicClass *object) {
		list<tAbstractBasicClass*>::iterator it;
		for (it = elem.begin(); it != elem.end(); it++) {	// Пока не нашли, перебираем список
			if (*it == object) {
				tAbstractBasicClass* obj = *it;		// Запоминаем объект
				elem.erase(it);			// Удаляем из списка
				((tObject*)elem.back())->changeOneOption(tObject::option_mask.is_active, false);
				message(elem.back(), tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
				elem.push_back(obj);	// Кидаем в конец списка
				break;
			}
		}
	}

	void tGroup::forEach(unsigned int code, tAbstractBasicClass* from = nullptr) {
		if (!from) {	// Если не сказано от кого событие, то отправителем становится сам почтальон
			from = this;
		}
		for (list<tAbstractBasicClass*>::reverse_iterator it = elem.rbegin(); it != elem.rend(); it++) {
			switch (code) {
				case tEvent::codes.Deactivate: {
					message(*it, tEvent::types.Broadcast, tEvent::codes.Deactivate, from);
					break;
				}
				case tEvent::codes.Show: {
					message(*it, tEvent::types.Broadcast, tEvent::codes.Show, from);
					break;
				}
				case tEvent::codes.Hide: {
					message(*it, tEvent::types.Broadcast, tEvent::codes.Hide, from);
					break;
				}
				case tEvent::codes.ResetButtons: {
					message(*it, tEvent::types.Broadcast, tEvent::codes.ResetButtons, from);
					break;
				}
			}

		}
	}

	void tGroup::draw(sf::RenderTarget& target) {
		for (list<tAbstractBasicClass*>::iterator it = elem.begin(); it != elem.end(); it++) {
			(*it)->draw(target);
		}
	}

	void tGroup::handleEvent(tEvent& e) {
		for (list<tAbstractBasicClass*>::reverse_iterator it = elem.rbegin(); it != elem.rend(); it++) {
			(*it)->handleEvent(e);
		}
	}

	void tGroup::makeObjectsFromJson(tAbstractBasicClass* _owner, nlohmann::json& js) {
		for (nlohmann::json::iterator it = js["elem"].begin(); it != js["elem"].end(); it++) {
			unsigned int what_is_it = (*it)["what_is_it"].get<unsigned char>();
			switch (what_is_it) {
			case objects_json_ids.tText: {
				tText* el = new tText(_owner, *it);
				_insert(el);
				break;
			}
			case objects_json_ids.tButton: {
				tButton* el = new tButton(_owner, *it);
				_insert(el);
				break;
			}
			case objects_json_ids.tRectShape: {
				tRectShape* el = new tRectShape(_owner, *it);
				_insert(el);
				break;
			}
			case objects_json_ids.tWindow: {
				tWindow* el = new tWindow(_owner, *it);
				_insert(el);
				break;
			}
			case objects_json_ids.tDisplay: {
				tDisplay* el = new tDisplay(_owner, *it);
				_insert((tRenderRect*)el);
				break;
			}
			}
		}
	}

	nlohmann::json tGroup::getParamsInJson() const {
		nlohmann::json js;
		unsigned int id = 0;
		for (auto &it : elem) {
			std::string str = std::to_string(id);
			js["elem"][str] = it->getParamsInJson();
			id++;
		}

		return js;
	}

	tRenderRect::tRenderRect(tAbstractBasicClass* _owner, sf::FloatRect rect) :
		tObject(_owner),
		render_squad(sf::VertexArray(sf::Quads, 4)),
		clear_color({ 0, 0, 0, 255 }),
		need_rerender(true)
	{
		tObject::setPosition({ rect.left, rect.top });
		render_texture.create((unsigned int)rect.width, (unsigned int)rect.height);

		setTextureSize({ (unsigned int)rect.width, (unsigned int)rect.height });
		setSize({ rect.width, rect.height });
	}

	tRenderRect::tRenderRect(tAbstractBasicClass* _owner, nlohmann::json& js) :
		tObject(_owner, js),
		render_squad(sf::VertexArray(sf::Quads, 4)),
		need_rerender(true)
	{
		vec<unsigned char> vuc = js["clear_color"].get<vec<unsigned char>>();
		clear_color = {vuc[0], vuc[1], vuc[2], vuc[3]};

		vec<float> vf = js["size"].get<vec<float>>();
		setSize({ vf[0], vf[1] });

		vec<unsigned int> vui = js["texture_size"].get<vec<unsigned int>>();
		render_texture.create(vui[0], vui[1]);
		setTextureSize({ vui[0], vui[1] });

		setPosition({ x, y });
	}

	tRenderRect::tRenderRect(const tRenderRect& r) :
		tObject(r),
		render_squad(r.render_squad),
		clear_color(r.clear_color),
		need_rerender(r.need_rerender)
	{
		sf::Sprite spr;
		spr.setTexture(r.render_texture.getTexture());
		render_texture.draw(spr);
		render_texture.display();
	}

	tRenderRect::~tRenderRect() {
	}

	void tRenderRect::setTextureSize(const sf::Vector2u &new_size) {
		render_texture.create(new_size.x, new_size.y);
		render_squad[0].texCoords = { 0.f, 0.f };
		render_squad[1].texCoords = { (float)new_size.x - 1, 0.f };
		render_squad[2].texCoords = { (float)new_size.x - 1, (float)new_size.y - 1 };
		render_squad[3].texCoords = { 0.f, (float)new_size.y - 1 };
		need_rerender = true;
	}

	void tRenderRect::setClearColor(const sf::Color &color) {
		clear_color = color;
	}

	sf::Vector2u tRenderRect::getTextureSize() const {
		return render_texture.getSize();
	}

	void tRenderRect::setSize(const sf::Vector2f &new_size) {
		render_squad[0].position = { x, y };
		render_squad[1].position = { x + new_size.x, y };
		render_squad[2].position = { x + new_size.x, y + new_size.y };
		render_squad[3].position = { x, y + new_size.y };
	}

	void tRenderRect::setPosition(const sf::Vector2f &new_position) {
		tObject::setPosition(new_position);
		sf::Vector2f offset = getRelativeStartPosition();

		render_squad[1].position = { x + render_squad[1].position.x - render_squad[0].position.x, y + render_squad[1].position.y - render_squad[0].position.y };
		render_squad[2].position = { x + render_squad[2].position.x - render_squad[0].position.x, y + render_squad[2].position.y - render_squad[0].position.y };
		render_squad[3].position = { x + render_squad[3].position.x - render_squad[0].position.x, y + render_squad[3].position.y - render_squad[0].position.y };
		render_squad[0].position = { x, y };

		for (unsigned int i = 0; i < 4; i++) {
			render_squad[i].position += offset;
		}
	}

	void tRenderRect::draw(sf::RenderTarget& target) {
		if (checkOption(option_mask.can_be_drawn)) {
			if (need_rerender) {
				need_rerender = false;
				updateTexture();
			}
			target.draw(render_squad, &render_texture.getTexture());	// Отобразиться
		}
	}

	void tRenderRect::move(sf::Vector2f delta) {
		tObject::move(delta);
		sf::Vector2f pos;
		for (char i = 0; i < 4; i++) {
			pos = render_squad[i].position;
			render_squad[i].position = pos + delta;
		}
	}

	sf::FloatRect tRenderRect::getLocalBounds() const {
		return sf::FloatRect(
			render_squad[0].position.x,
			render_squad[0].position.y,
			render_squad[1].position.x - render_squad[0].position.x,
			render_squad[3].position.y - render_squad[0].position.y
		);
	}

	nlohmann::json tRenderRect::getParamsInJson() const {
		nlohmann::json js = tObject::getParamsInJson();

		sf::Vector2f size = render_squad[2].position - render_squad[0].position;
		sf::Vector2u tex_size = render_texture.getSize();

		js["size"] = { size.x, size.y };
		js["texture_size"] = { tex_size.x, tex_size.y };
		js["clear_color"] = { clear_color.r, clear_color.g, clear_color.b, clear_color.a };

		return js;
	}

	tDesktop::tDesktop(std::string path_to_folder, unsigned char _screen_code) :
		tGroup(nullptr),
		custom_font_loaded(false),
		json_configuration(),
		screen_code(_screen_code),
		window_size({1280, 720})
	{
		background.setPosition({ 0, 0 });
		background.setFillColor({40, 40, 40, 255});

		old_mouse_position = sf::Mouse::getPosition();

		this->path_to_folder = path_to_folder;
		std::string config_file_name = "\\Content\\Config\\forms.conf";

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
		changeScreen(screen_code);
		
		while (window.isOpen()) {
			completeEvents();

			window.clear();
			draw(window);
			window.display();

			sf::sleep(sf::milliseconds(5));
		};
		/*sf::Vector2u size = window.getSize();
		nlohmann::json js;
		js["menu"] = getParamsInJson();
		js["sfml_window"]["caption"] = "SFML_Game environment editor";
		js["sfml_window"]["size"] = { size.x, size.y };
		js["sfml_window"]["style"] = "Default";
		js["sfml_window"]["font_default"] = "\\Content\\Fonts\\PT Sans.ttf";
		print_json(js, path_to_folder + "\\Content\\Config\\forms.conf");*/
		/*std::fstream file(path_to_folder + "\\Content\\Config\\forms.conf", std::fstream::out);
		file << js;
		file.close();*/
	}

	bool tDesktop::windowIsOpen() const {
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

	void tDesktop::saveData() const {
	}

	void tDesktop::loadCustomFont(std::string path_to_font) {
		if (custom_font.loadFromFile(path_to_font)) {
			custom_font_loaded = true;
		}
		else {
			std::cout << "tDesktop.loadcustomFont error: Invalid path_to_font.\n";
		};
	}

	void tDesktop::completeEvents() {
		tEvent e;
		getEvent(e);
		while (e.type != e.types.Nothing) {
			handleEvent(e);
			getEvent(e);
		}
	}

	sf::Font& tDesktop::getFont() const {
		if (custom_font_loaded)
			return (sf::Font&)custom_font;
		else 
			return (sf::Font&)font_default;
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
						e.type = e.types.Button;
						e.code = e.codes.CloseApplication;
						e.address = this;
						break;
					}
					case sf::Event::KeyPressed:			// Нажата или отпущена какая-либо кнопка на клавиатуре
					case sf::Event::KeyReleased: {
						e.type = e.types.Keyboard;
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
						e.type = e.types.Mouse;
						e.code = e.codes.MouseButton;
						event.type == sf::Event::MouseButtonPressed ?
							e.mouse.what_happened = sf::Event::MouseButtonPressed : e.mouse.what_happened = sf::Event::MouseButtonReleased;
						e.mouse.button = event.mouseButton.button;
						e.mouse.x = event.mouseButton.x;
						e.mouse.y = event.mouseButton.y;
						e.address = this;
						break;
					}
					case sf::Event::MouseMoved: {			// Мышь двинулась куда-то
						e.type = e.types.Mouse;
						e.code = e.codes.MouseMoved;
						e.mouse.x = event.mouseMove.x;
						e.mouse.y = event.mouseMove.y;
						e.mouse.dX = e.mouse.x - old_mouse_position.x;
						e.mouse.dY = e.mouse.y - old_mouse_position.y;
						old_mouse_position = { e.mouse.x , e.mouse.y };
						e.address = this;
						break;
					}
					default: {
						e.type = e.types.Nothing;
						break;
					}
					}
				}
				else {
					e.type = e.types.Nothing;
				}
			}
		}
	}

	void tDesktop::handleEvent(tEvent& e) {
		tGroup::handleEvent(e);
		switch (e.type) {
			case e.types.Broadcast: {	// Общего типа
				if (e.address == this) {			// Обработка событий для этого объекта
					switch (e.code) {
						case e.codes.Delete: {		// Удалить объект
							_delete(e.from);
							clearEvent(e);
							break;
						}
						case e.codes.Activate: {		// Установить фокус на объект
							select(e.from);
							((tObject*)e.from)->changeOneOption(tObject::option_mask.is_active, true);
							clearEvent(e);
							break;
						}
						case e.codes.Deactivate: {	// Снять фокус с объекта
							((tObject*)e.from)->changeOneOption(tObject::option_mask.is_active, false);
							clearEvent(e);
							break;
						}
						case e.codes.Show: {			// Показать объект (если он скрыт)
							((tObject*)e.from)->changeOneOption(tObject::option_mask.can_be_drawn, true);
							clearEvent(e);
							break;
						}
						case e.codes.Hide: {			// Спрятать объект (если он не скрыт)
							((tObject*)e.from)->changeOneOption(tObject::option_mask.can_be_drawn, false);
							clearEvent(e);
							break;
						}
						case e.codes.Adopt: {			// Стать владельцем объекта
							e.from->setOwner(this);
							_insert(e.from);
							clearEvent(e);
							break;
						}
					}
				}
				switch (e.code) {	// Обработка событий от "дальних" объектов
					case e.codes.FontRequest: {
						e.type = e.types.Broadcast;
						e.code = e.codes.FontAnswer;
						if (!custom_font_loaded) {
							e.font.font = &font_default;
						}
						else {
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
			case e.types.Button: {	// От кнопки
				switch (e.code) {
					case e.codes.ResetButtons: {
						forEach(e.codes.ResetButtons, e.from);
						clearEvent(e);
						break;
					}
					case e.codes.CloseApplication: {	// Закрыть приложение
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

	sf::FloatRect tDesktop::getLocalBounds() const {
		return {
			0,
			0,
			(float)window_size.x,
			(float)window_size.y
		};
	}

	bool tDesktop::pointIsInsideMe(sf::Vector2i point) const {
		sf::Vector2i size = (sf::Vector2i)window.getSize();
		return (point.x >= 0 && point.x <= size.x && point.y >= 0 && point.y <= size.y);
	}

	nlohmann::json tDesktop::getParamsInJson() const {
		nlohmann::json js;

		js = tGroup::getParamsInJson();

		js["what_is_it"] = objects_json_ids.tDesktop;
		js["what_is_it_string"] = "tDesktop";

		return js;
	}

	tRectShape::tRectShape(tAbstractBasicClass* _owner, sf::FloatRect rect, sf::Color fill_color) :
		tObject(_owner)
	{
		setPosition({rect.left, rect.top});
		setSize({rect.width, rect.height});
		setColor(fill_color);
	}

	tRectShape::tRectShape(tAbstractBasicClass* _owner, nlohmann::json& js) :
		tObject(_owner, js)
	{		
		vec<float> vf = js["size"].get<vec<float>>();
		setSize({ vf[0], vf[1] });

		vec<unsigned char> vuc = js["color"].get<vec<unsigned char>>();
		setColor({ vuc[0], vuc[1], vuc[2], vuc[3] });

		setPosition({ x, y });
	}

	tRectShape::tRectShape(const tRectShape& s) :
		tObject(s),
		shape(s.shape)
	{
	}
	
	tRectShape::~tRectShape() {
	}

	void tRectShape::setColor(const sf::Color &new_color) {
		shape.setFillColor(new_color);
	}

	void tRectShape::draw(sf::RenderTarget& target) {
		if (checkOption(option_mask.can_be_drawn)) {
			target.draw(shape);
		}
	}

	void tRectShape::setPosition(const sf::Vector2f &new_position) {
		tObject::setPosition(new_position);
		shape.setPosition(new_position + getRelativeStartPosition());
	}

	void tRectShape::setSize(const sf::Vector2f &new_size) {
		shape.setSize(new_size);
	}

	void tRectShape::move(sf::Vector2f delta) {
		tObject::move(delta);
		shape.move(delta);
	}

	void tRectShape::updateTexture() {
		return;
	}

	void tRectShape::handleEvent(tEvent& e) {
		if (checkOption(option_mask.can_be_drawn)) {
			switch (e.type) {
				case e.types.Mouse: {
					mouse_inside[1] = mouse_inside[0];
					mouse_inside[0] = pointIsInsideMe({ e.mouse.x, e.mouse.y });
					switch (e.code) {
						case e.codes.MouseButton: {
							if (mouse_inside[0]) {	// Если мышь внутри, ...
								if (e.mouse.what_happened == sf::Event::MouseButtonReleased) {	// Если отпустили кнопку
									changeOneOption(option_mask.is_moving, false);
								}
								else if (e.mouse.what_happened == sf::Event::MouseButtonPressed) {
									if (e.mouse.button == sf::Mouse::Left) {	// Если тыкнули левой кнопкой мыши
										if (checkOption(option_mask.can_be_moved)) {
											changeOneOption(option_mask.is_moving, true);
										}
										message(getOwner(), e.types.Broadcast, e.codes.Activate, this);
									}
								}
								clearEvent(e);
							}
							break;
						}
						case e.codes.MouseMoved: {
							if (checkOption(option_mask.is_moving)) {
								move({ (float)e.mouse.dX, (float)e.mouse.dY });
								message(getOwner(), e.types.Broadcast, e.codes.UpdateTexture, this);
							}
							if (mouse_inside[0] != mouse_inside[1]) {
								message(nullptr, e.types.Broadcast, e.codes.ResetButtons, this);
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

	bool tRectShape::pointIsInsideMe(sf::Vector2i point) const {
		sf::FloatRect rect = getGlobalBounds();
		return (point.x >= rect.left && point.x <= rect.left + rect.width && point.y >= rect.top && point.y <= rect.top + rect.height);
	}

	sf::FloatRect tRectShape::getLocalBounds() const {
		return sf::FloatRect(
			shape.getPosition().x,
			shape.getPosition().y,
			shape.getSize().x * shape.getScale().x,
			shape.getSize().y * shape.getScale().y
		);
	}

	nlohmann::json tRectShape::getParamsInJson() const {
		nlohmann::json js = tObject::getParamsInJson();

		sf::Vector2f size = shape.getSize();
		sf::Color color = shape.getFillColor();

		js["what_is_it"] = objects_json_ids.tRectShape;
		js["what_is_it_string"] = "tRectShape";
		js["size"] = { size.x, size.y };
		js["color"] = { color.r, color.g, color.b, color.a };

		return js;
	}

	tText::tText(tAbstractBasicClass* _owner, sf::Vector2f position, std::wstring string) :
		tObject(_owner),
		font_loaded(false)
	{
		tObject::setPosition(position);
		setString(string);
		setTextColor({ 255, 255, 255, 255 });
		setCharSize(24);
		setOutlineThickness(1);
		setPosition(position);
	}

	tText::tText(tAbstractBasicClass* _owner, nlohmann::json& js) :
		tObject(_owner, js),
		font_loaded(false)
	{
		vec<int> vi = js["text"].get<vec<int>>();
		setString(convertIntVectorToWstring(vi));
		
		vec<unsigned char> vuc = js["color"].get<vec<unsigned char>>();
		setTextColor({ vuc[0], vuc[1], vuc[2], vuc[3] });

		setCharSize(js["char_size"].get<unsigned int>());

		setOutlineThickness(js["outline_thickness"].get<unsigned int>());

		setPosition({ x, y });
	}

	tText::tText(const tText& t) :
		tObject(t),
		text_object(t.text_object),
		font(t.font),
		font_loaded(t.font_loaded)
	{
	}

	tText::~tText() {
	}

	void tText::setString(const std::wstring &new_string) {
		text_object.setString(new_string);
	}

	void tText::setTextColor(const sf::Color &new_color) {
		text_object.setFillColor(new_color);
	}

	void tText::setFont(const sf::Font &new_font) {
		font_loaded = true;
		font = new_font;
		text_object.setFont(font);
	}

	void tText::setCharSize(const unsigned int &new_char_size) {
		text_object.setCharacterSize(new_char_size);
	}

	void tText::setOutlineThickness(const unsigned char &new_thickness) {
		text_object.setOutlineThickness(new_thickness);
	}

	bool tText::getFontState() const {
		return font_loaded;
	}

	sf::Text& tText::getTextObject() const {
		return (sf::Text&)text_object;
	}

	sf::Color tText::getFillColor() const {
		return text_object.getFillColor();
	}

	sf::FloatRect tText::getLocalBounds() const {
		return text_object.getLocalBounds();
	}

	bool tText::pointIsInsideMe(sf::Vector2i point) const {
		sf::FloatRect rect = getGlobalBounds();
		return (point.x >= rect.left && point.x <= rect.left + rect.width && point.y >= rect.top && point.y <= rect.top + rect.height);
	}

	nlohmann::json tText::getParamsInJson() const {
		nlohmann::json js = tObject::getParamsInJson();

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
			// если код в предыдущем блоке выполнился, то код ниже не выполняется
			message(nullptr, tEvent::types.Broadcast, tEvent::codes.FontRequest, this);
			tEvent e;
			e.address = getOwner();
			e.from = this;
			e.type = tEvent::types.Broadcast;
			e.code = tEvent::codes.UpdateTexture;
			putEvent(e);
		}
	}

	void tText::setPosition(const sf::Vector2f &new_position) {
		tObject::setPosition(new_position);
		text_object.setPosition(new_position + getRelativeStartPosition());
	}

	void tText::handleEvent(tEvent& e) {
		if (checkOption(option_mask.can_be_drawn)) {
			switch (e.type) {
				case e.types.Broadcast: {
					if (e.address == this) {
						switch (e.code) {
							case e.codes.FontAnswer: {
								font_loaded = true;
								text_object.setFont(*e.font.font);
								message(getOwner(), e.types.Broadcast, e.codes.UpdateTexture, this);
								clearEvent(e);
								break;
							}
						}
					}
					break;
				}
				case e.types.Mouse: {
					mouse_inside[1] = mouse_inside[0];
					mouse_inside[0] = pointIsInsideMe({ e.mouse.x, e.mouse.y });
				}
			}
		}
	}

	void tText::move(sf::Vector2f delta) {
		tObject::move(delta);
		text_object.move(delta);
	}

	void tText::updateTexture() {
		return;
	}

	tButton::tButton(tAbstractBasicClass* _owner, sf::FloatRect rect) :
		tRenderRect(_owner, rect),
		self_code(tEvent::codes.Nothing),
		custom_skin_loaded(false),
		alignment(static_cast<int>(text_alignment_type::Left)),
		side_offset(10),
		text_offset(sf::Vector2u(0, 0)),
		text(new tText(this))
	{
	}

	tButton::tButton(tAbstractBasicClass* _owner, nlohmann::json& js) :
		tRenderRect(_owner, js),
		custom_skin_loaded(false),
		side_offset(10),
		text(new tText(this, js["text"]))
	{
		vec<int> vi = js["text_offset"].get<vec<int>>();
		text_offset = { vi[0], vi[1] };
		self_code = js["code"].get<int>();
		alignment = js["alignment"].get<char>();

	}

	tButton::tButton(const tButton& b) :
		tRenderRect(b),
		side_offset(b.side_offset),
		self_code(b.self_code),
		custom_skin_loaded(b.custom_skin_loaded),
		alignment(b.alignment),
		custom_skin(b.custom_skin),
		text_offset(b.text_offset),
		text(b.text)
	{
	}

	tButton::~tButton() {
		delete text;
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
		if (text->getFontState()) {				// Если подгружен шрифт, то выводим текст
			sf::Text text_to_display = text->getTextObject();
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
			text_to_display.setFillColor(text->getFillColor());		// А это уже сам вывод текста
			text_to_display.move({ -1, -1 });
			render_texture.draw(text_to_display);
		}
		else {
			message(nullptr, tEvent::types.Broadcast, tEvent::codes.FontRequest, text);
		}
		render_texture.display();
	}

	void tButton::loadCustomSkin(std::string path_to_skin) {
		if (custom_skin.loadFromFile(path_to_skin)) {
			custom_skin_loaded = true;
			setSize(sf::Vector2f((float)custom_skin.getSize().x, (float)custom_skin.getSize().y));
			setTextureSize(sf::Vector2u(custom_skin.getSize().x, custom_skin.getSize().y));
			message(this, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
		}
		else {
			std::cout << "tButton.loadCustomSkin error: Invalid path_to_skin.\n";
		}
	}

	void tButton::setTextAlignment(const char &new_alignment) {
		switch (new_alignment) {
			case static_cast<int>(tButton::text_alignment_type::Middle) :
			case static_cast<int>(tButton::text_alignment_type::Right) : {
				alignment = new_alignment;
				break;
			}
			case static_cast<int>(tButton::text_alignment_type::Left) :
			default: {
				alignment = static_cast<int>(tButton::text_alignment_type::Left);
				break;
			}
		}
		message(this, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
	}

	void tButton::setTextOffset(const sf::Vector2i &new_offset) {
		text_offset = new_offset;
		message(this, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
	}

	void tButton::setCode(const int &new_code) {
		self_code = new_code;
	}

	void tButton::setFont(const sf::Font &new_font) {
		text->setFont(new_font);
	}

	void tButton::setString(const std::wstring &new_string) {
		text->setString(new_string);
	}

	void tButton::setTextColor(const sf::Color &new_color) {
		text->setTextColor(new_color);
	}

	void tButton::setCharSize(const unsigned int &new_char_size) {
		text->setCharSize(new_char_size);
	}

	void tButton::setOutlineThickness(const unsigned char &new_thickness) {
		text->setOutlineThickness(new_thickness);
	}

	bool tButton::pointIsInsideMe(sf::Vector2i point) const {
		sf::FloatRect rect = getGlobalBounds();
		return (point.x >= rect.left && point.x <= rect.left + rect.width && point.y >= rect.top && point.y <= rect.top + rect.height);
	}

	nlohmann::json tButton::getParamsInJson() const {
		nlohmann::json js = tRenderRect::getParamsInJson();

		js["what_is_it"] = objects_json_ids.tButton;
		js["what_is_it_string"] = "tButton";
		js["code"] = self_code;
		js["alignment"] = alignment;
		js["text_offset"] = { text_offset.x, text_offset.y };
		js["text"] = text->getParamsInJson();

		return js;
	}

	void tButton::handleEvent(tEvent& e) {
		if (checkOption(option_mask.can_be_drawn)) {
			text->handleEvent(e);
			switch (e.type) {
				case e.types.Broadcast: {
					if (e.address == this) {	// Для конкретно этой кнопки
						switch (e.code) {
							case e.codes.UpdateTexture: {	// Обновить текстуру
								message(getOwner(), e.types.Broadcast, e.codes.UpdateTexture, this);
								need_rerender = true;
								clearEvent(e);
								break;
							}
						}
					}
					switch (e.code) {			// Для всех остальных
						case e.codes.ResetButtons: {
							if (e.from != this && e.from != getOwner()) {
								mouse_inside[0] = false;
								mouse_inside[1] = false;
								need_rerender = true;
							}
							break;
						}
					}
					break;
				}
				case e.types.Mouse: {
					mouse_inside[1] = mouse_inside[0];
					mouse_inside[0] = pointIsInsideMe({ e.mouse.x, e.mouse.y });
					switch (e.code) {
						case e.codes.MouseMoved: {
							if (mouse_inside[0] != mouse_inside[1]) {	// Если произошло изменение, то генерируем текстуру заново с подчёркнутым текстом
								message(nullptr, e.types.Button, e.codes.ResetButtons, this);
								message(this, e.types.Broadcast, e.codes.UpdateTexture, this);
								clearEvent(e);
							}
							break;
						}
						case e.codes.MouseButton: {
							if (mouse_inside[0]) {
								if (e.mouse.button == sf::Mouse::Left) {
									if (e.mouse.what_happened == sf::Event::MouseButtonReleased) {	// Если левая кнопка мыши отпущена, и мышь находится внутри кнопки, то передаём послание
										message(getOwner(), e.types.Button, self_code, this);
									}
								}
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


	tWindow::tWindow(tAbstractBasicClass* _owner, sf::FloatRect rect, std::wstring _caption) :
		tRenderRect(_owner),
		font_loaded(false),
		caption(_caption),
		color_heap(sf::Color(100, 100, 100, 255)),
		color_area(sf::Color(80, 80, 80, 255)),
		color_caption_active(sf::Color(255, 255, 255, 255)),
		color_caption_inactive(sf::Color(150, 150, 150, 255)),
		caption_offset({2, 2}),
		button_close(new tButton(this, { 0, 0, heap_height - 4 , heap_height - 4 })),
		heap_shape(new tRectShape(this, { 0, 0, rect.width, heap_height }, color_heap)),
		display(new tDisplay(this, { 0, heap_height, rect.width, rect.height - heap_height })),
		scrollbar_v(new tScrollbar(this, true, { 0, 0, tScrollbar::thickness, rect.height - tScrollbar::thickness - heap_height - 1 })),
		scrollbar_h(new tScrollbar(this, false, { 0, 0, rect.width - tScrollbar::thickness - 2, tScrollbar::thickness })),
		last_scrollbar_offset({ 0.f, 0.f })
	{
		changeOneOption(option_mask.can_be_moved, true);
		changeOneOption(option_mask.can_be_resized, true);

		setPosition({ rect.left, rect.top });
		setSize({ rect.width, rect.height });
		setTextureSize({ (unsigned int)rect.width, (unsigned int)rect.height });

		initWindow();
	}

	tWindow::tWindow(tAbstractBasicClass* _owner, nlohmann::json& js) :
		tRenderRect(_owner, js),
		font_loaded(false),
		button_close(new tButton(this, js["button_close"])),
		heap_shape(new tRectShape(this, js["heap_shape"])),
		display(new tDisplay(this, js["display"])),
		scrollbar_v(new tScrollbar(this, js["scrollbar_v"])),
		scrollbar_h(new tScrollbar(this, js["scrollbar_h"])),
		last_scrollbar_offset({ 0.f, 0.f })
	{
		vec<int> vi = js["caption"].get<vec<int>>();
		caption = convertIntVectorToWstring(vi);
		vec<float> vf = js["caption_offset"].get<vec<float>>();
		caption_offset = { vf[0], vf[1] };
		vec<unsigned char> vuc = js["color_heap"].get<vec<unsigned char>>();
		color_heap = { vuc[0], vuc[1], vuc[2], vuc[3] };
		vuc.clear();
		vuc = js["color_area"].get<vec<unsigned char>>();
		color_area = { vuc[0], vuc[1], vuc[2], vuc[3] };
		vuc.clear();
		vuc = js["color_caption_active"].get<vec<unsigned char>>();
		color_caption_active = { vuc[0], vuc[1], vuc[2], vuc[3] };
		vuc.clear();
		vuc = js["color_caption_inactive"].get<vec<unsigned char>>();
		color_caption_inactive = { vuc[0], vuc[1], vuc[2], vuc[3] };
		vuc.clear();
	}

	tWindow::tWindow(const tWindow& w) :
		tRenderRect(w),
		font_loaded(w.font_loaded),
		font(w.font),
		caption(w.caption),
		color_heap(w.color_heap),
		color_area(w.color_area),
		color_caption_active(w.color_caption_active),
		color_caption_inactive(w.color_caption_inactive),
		caption_offset(w.caption_offset),
		button_close(w.button_close),
		heap_shape(w.heap_shape),
		display(w.display),
		scrollbar_v(w.scrollbar_v),
		scrollbar_h(w.scrollbar_h),
		last_scrollbar_offset(w.last_scrollbar_offset)
	{
	}

	tWindow::~tWindow() {
		delete button_close, heap_shape, display, scrollbar_v, scrollbar_h;
	}

	void tWindow::initWindow() {
		sf::FloatRect rect = getLocalBounds();

		button_close->setAnchor(anchors.upper_right_corner);
		button_close->setPosition({ -heap_height + 2, 2 });
		button_close->setCode(tEvent::codes.Close);
		button_close->setString(L"x");
		button_close->setTextColor({ 255, 255, 255, 255 });
		button_close->setCharSize(20);
		button_close->setOutlineThickness(1);
		button_close->setTextAlignment(static_cast<int>(tButton::text_alignment_type::Middle));
		button_close->setTextOffset({ 0, -3 });
		message(button_close, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);

		display->setClearColor(color_area);

		sf::FloatRect display_bounds = display->getLocalBounds();

		scrollbar_v->setAnchor(anchors.upper_right_corner);
		scrollbar_v->setPosition({ -tScrollbar::thickness - 2, heap_height });
		scrollbar_v->setTargetSize({ display_bounds.width, display_bounds.height });
		scrollbar_v->setTargetTextureSize(display->getTextureSize());

		scrollbar_h->setAnchor(anchors.bottom_left_corner);
		scrollbar_h->setPosition({ 1, -tScrollbar::thickness - 2 });
		scrollbar_h->setTargetSize({ display_bounds.width, display_bounds.height });
		scrollbar_h->setTargetTextureSize(display->getTextureSize());
	}

	void tWindow::setCaption(const std::wstring &new_caption) {
		caption = new_caption;
	}

	tDisplay* tWindow::getDisplayPointer() const {
		return display;
	}

	std::wstring tWindow::getCaption() const {
		return caption;
	}

	void tWindow::setHeapColor(const sf::Color &new_color) {
		color_heap = new_color;
	}

	void tWindow::setAreaColor(const sf::Color &new_color) {
		color_area = new_color;
	}

	void tWindow::setActiveCaptionColor(const sf::Color &new_color) {
		color_caption_active = new_color;
	}

	void tWindow::setInactiveCaptionColor(const sf::Color &new_color) {
		color_caption_inactive = new_color;
	}

	void tWindow::setFont(const sf::Font &new_font) {
		font_loaded = true;
		font = new_font;
	}

	void tWindow::setCaptionOffset(const sf::Vector2f &new_offset) {
		caption_offset = new_offset;
	}

	void tWindow::updateTexture() {
		render_texture.clear(clear_color);

		display->draw(render_texture);
		heap_shape->draw(render_texture);
		button_close->draw(render_texture);
		scrollbar_v->draw(render_texture);
		scrollbar_h->draw(render_texture);

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
			message(nullptr, tEvent::types.Broadcast, tEvent::codes.FontRequest, this);
		}

		sf::VertexArray frame(sf::LineStrip, 5);	// Рисование обводки окна
		sf::FloatRect rect = getLocalBounds();
		sf::Color color = { 140, 140, 140, 255 };
		frame[0].position = { 1, 0 };
		frame[1].position = { rect.width - 1, 0 };
		frame[2].position = { rect.width - 1, rect.height - 2 };
		frame[3].position = { 1, rect.height - 2 };
		frame[4].position = { 1, 1 };
		if (checkOption(tObject::option_mask.is_active)) 
			color = { 0, 122, 204, 255 };
		for (int i = 0; i < 5; i++) 
			frame[i].color = color;
		render_texture.draw(frame);

		render_texture.display();
	}

	void tWindow::setCameraOffset(const sf::Vector2f &new_offset) {
		display->setCameraOffset(new_offset);
	}

	void tWindow::setDisplaySize(const sf::Vector2f &new_size) {
		display->setSize(new_size);
		scrollbar_v->setTargetSize(new_size);
		scrollbar_h->setTargetSize(new_size);
		scrollbar_v->updateScrollerSize();
		scrollbar_h->updateScrollerSize();
		message(scrollbar_v, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
		message(scrollbar_h, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
	}

	void tWindow::setDisplayTextureSize(const sf::Vector2u &new_size) {
		display->setTextureSize(new_size);
		scrollbar_v->setTargetTextureSize(new_size);
		scrollbar_h->setTargetTextureSize(new_size);
		scrollbar_v->updateScrollerSize();
		scrollbar_h->updateScrollerSize();
		message(scrollbar_v, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
		message(scrollbar_h, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
	}

	const int tWindow::getHeapHeight() const {
		return heap_height;
	}

	bool tWindow::pointIsInHeap(sf::Vector2i point) const {
		sf::FloatRect rect = getGlobalBounds();
		return (point.x >= rect.left && point.x <= rect.left + rect.width && point.y >= rect.top && point.y <= rect.top + heap_height);
	}

	bool tWindow::pointIsInsideMe(sf::Vector2i point) const {
		sf::FloatRect rect = getGlobalBounds();
		return (point.x >= rect.left && point.x <= rect.left + rect.width && point.y >= rect.top && point.y <= rect.top + rect.height);
	}

	nlohmann::json tWindow::getParamsInJson() const {
		nlohmann::json js = tRenderRect::getParamsInJson();

		js["what_is_it"] = objects_json_ids.tWindow;
		js["what_is_it_string"] = "tWindow";

		js["caption"] = caption;
		js["caption_offset"] = { caption_offset.x, caption_offset.y };
		js["color_heap"] = { color_heap.r, color_heap.g, color_heap.b, color_heap.a };
		js["color_area"] = { color_area.r, color_area.g, color_area.b, color_area.a };
		js["color_caption_active"] = { color_caption_active.r, color_caption_active.g, color_caption_active.b, color_caption_active.a };
		js["color_caption_inactive"] = { color_caption_inactive.r, color_caption_inactive.g, color_caption_inactive.b, color_caption_inactive.a };

		js["button_close"] = button_close->getParamsInJson();
		js["heap_shape"] = heap_shape->getParamsInJson();
		js["display"] = display->getParamsInJson();
		js["scrollbar_v"] = scrollbar_v->getParamsInJson();
		js["scrollbar_h"] = scrollbar_h->getParamsInJson();

		return js;
	}

	void tWindow::handleEvent(tEvent& e) {
		if (checkOption(option_mask.can_be_drawn)) {

			button_close->handleEvent(e);
			display->handleEvent(e);
			scrollbar_v->handleEvent(e);
			scrollbar_h->handleEvent(e);

			/*
			>>>>>>>>>> Временная штука. Это нужно будет переместить в блок изменения размеров окна (сделать ивент) <<<<<<<<<<
			*/

			auto dispTexSize = display->getTextureSize();

			if (scrollbar_v->checkOption(option_mask.can_be_drawn) && dispTexSize.y <= getTextureSize().y - heap_height) {
				// Если скроллбар активен и размер текстуры дисплея влезают в зону отрисовки окна, тогда выключаем скроллбар
				display->setSize({ getLocalBounds().width, display->getLocalBounds().height });
				scrollbar_v->changeOneOption(option_mask.can_be_drawn, false);
			}
			else if (!scrollbar_v->checkOption(option_mask.can_be_drawn) && dispTexSize.y > getTextureSize().y - heap_height) {
				display->setSize({ getLocalBounds().width - tScrollbar::thickness - 2, display->getLocalBounds().height });
				scrollbar_v->changeOneOption(option_mask.can_be_drawn, true);
			}

			if (scrollbar_h->checkOption(option_mask.can_be_drawn) && display->getTextureSize().x <= getTextureSize().x) {
				// Если скроллбар активен и размер текстуры дисплея влезает в зону отрисовки окна, тогда выключаем скроллбар
				display->setSize({ display->getLocalBounds().width, getLocalBounds().height - heap_height });
				scrollbar_h->changeOneOption(option_mask.can_be_drawn, false);
			}
			else if (!scrollbar_h->checkOption(option_mask.can_be_drawn) && display->getTextureSize().x > getTextureSize().x) {
				display->setSize({ display->getLocalBounds().width, getLocalBounds().height - heap_height - tScrollbar::thickness - 2 });
				scrollbar_h->changeOneOption(option_mask.can_be_drawn, true);
			}
			
			/*
			>>>>>>>>>> Временная штука. Это нужно будет переместить в блок изменения размеров окна (сделать ивент) <<<<<<<<<<
			*/

			sf::Vector2f scrollbar_offset = { scrollbar_h->getPixelOffset(), scrollbar_v->getPixelOffset() };
			if (scrollbar_offset != last_scrollbar_offset) {
				setCameraOffset(scrollbar_offset);
				last_scrollbar_offset = scrollbar_offset;
			}

			switch (e.type) {
				case e.types.Broadcast: {
					if (e.address == this) {	// Для конкретно этого окна
						switch (e.code) {
							case e.codes.Activate: {		// Установить фокус на объект
								((tObject*)e.from)->changeOneOption(tObject::option_mask.is_active, true);
								message(getOwner(), e.types.Broadcast, e.codes.Activate, this);
								clearEvent(e);
								break;
							}
							case e.codes.Deactivate: {	// Снять фокус с объекта
								((tObject*)e.from)->changeOneOption(tObject::option_mask.is_active, false);
								clearEvent(e);
								break;
							}
							case e.codes.Show: {			// Показать объект (если он скрыт)
								((tObject*)e.from)->changeOneOption(tObject::option_mask.can_be_drawn, true);
								clearEvent(e);
								break;
							}
							case e.codes.Hide: {			// Спрятать объект (если он не скрыт)
								((tObject*)e.from)->changeOneOption(tObject::option_mask.can_be_drawn, false);
								clearEvent(e);
								break;
							}
							case e.codes.UpdateTexture: {	// Обновить текстуру
								message(getOwner(), e.types.Broadcast, e.codes.UpdateTexture, this);
								need_rerender = true;
								clearEvent(e);
								break;
							}
							case e.codes.FontAnswer: {	// Забрать выданный системой шрифт
								font_loaded = true;
								setFont(*e.font.font);
								message(this, e.types.Broadcast, e.codes.UpdateTexture, this);
								message(getOwner(), e.types.Broadcast, e.codes.UpdateTexture, this);
								clearEvent(e);
								break;
							}
							default: {				// Если не обработалось, то "проталкиваем" на уровень ниже
								e.address = getOwner();
								message(e);
								clearEvent(e);
								break;
							}
						}
					}
					break;
				}
				case e.types.Button: {
					if (e.address == this) {	// Для конкретно этого окна
						switch (e.code) {
							case e.codes.Close: {			// Закрыть окно
								e.type = e.types.Broadcast;
								e.code = e.codes.Delete;
								e.address = getOwner();
								e.from = this;
								putEvent(e);
								clearEvent(e);
								break;
							}
							default: {				// Если не обработалось, то "проталкиваем" на уровень ниже
								e.address = getOwner();
								putEvent(e);
								clearEvent(e);
							}
						}
					}
					break;
				}
				case e.types.Mouse: {
					mouse_inside[1] = mouse_inside[0];
					mouse_inside[0] = pointIsInsideMe({ e.mouse.x, e.mouse.y });
					switch (e.code) {
						case e.codes.MouseButton: {
							if (mouse_inside[0]) {	// Если мышь внутри, ...
								if (e.mouse.what_happened == sf::Event::MouseButtonReleased) {	// Если отпустили кнопку
									changeOneOption(option_mask.is_moving, false);
								}
								else if (e.mouse.what_happened == sf::Event::MouseButtonPressed) {
									if (e.mouse.button == sf::Mouse::Left) {	// Если в окно тыкнули левой кнопкой мыши
										if (pointIsInHeap({ e.mouse.x, e.mouse.y }) && checkOption(option_mask.can_be_moved)) {
											changeOneOption(option_mask.is_moving, true);
										}
										message(getOwner(), e.types.Broadcast, e.codes.Activate, this);
										need_rerender = true;
									}
								}
								clearEvent(e);
							}
							break;
						}
						case e.codes.MouseMoved: {
							if (mouse_inside[0] != mouse_inside[1]) {
								message(nullptr, e.types.Broadcast, e.codes.ResetButtons, this);
								clearEvent(e);
							}
							if (mouse_inside[0]) {
								message(this, e.types.Broadcast, e.codes.UpdateTexture, this);
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

	tDisplay::tDisplay(tAbstractBasicClass* _owner, sf::FloatRect rect):
		owner(_owner),
		tGroup((tRenderRect*)this),
		tRenderRect(_owner, rect)
	{
	}

	tDisplay::tDisplay(tAbstractBasicClass* _owner, nlohmann::json& js):
		owner(_owner),
		tGroup((tRenderRect*)this, js),
		tRenderRect(_owner, js)
	{
		vec<float> vf = js["camera_offset"].get<vec<float>>();
		setCameraOffset({ vf[0], vf[1] });
	}

	tDisplay::tDisplay(const tDisplay& d):
		owner(d.owner),
		tGroup(d),
		tRenderRect(d)
	{
	}

	tDisplay::~tDisplay()
	{
	}

	void tDisplay::setCameraOffset(const sf::Vector2f &new_offset) {
		render_squad[1].texCoords += new_offset - render_squad[0].texCoords;
		render_squad[2].texCoords += new_offset - render_squad[0].texCoords;
		render_squad[3].texCoords += new_offset - render_squad[0].texCoords;
		render_squad[0].texCoords = new_offset;
	}

	void tDisplay::draw(sf::RenderTarget& target) {
		tRenderRect::draw(target);
	}

	void tDisplay::handleEvent(tEvent& e) {
		if (checkOption(option_mask.can_be_drawn)) {
			tGroup::handleEvent(e);
			switch (e.type) {
				case e.types.Broadcast: {
					if (e.address == (tRenderRect*)this) {
						switch (e.code) {
							case e.codes.Deactivate: {	// Снять фокус с объекта
								((tObject*)e.from)->changeOneOption(tObject::option_mask.is_active, false);
								tAbstractBasicClass::clearEvent(e);
								break;
							}
							case e.codes.Show: {			// Показать объект (если он скрыт)
								((tObject*)e.from)->changeOneOption(tObject::option_mask.can_be_drawn, true);
								tAbstractBasicClass::clearEvent(e);
								break;
							}
							case e.codes.Hide: {			// Спрятать объект (если он не скрыт)
								((tObject*)e.from)->changeOneOption(tObject::option_mask.can_be_drawn, false);
								tAbstractBasicClass::clearEvent(e);
								break;
							}
							case e.codes.Adopt: {			// Стать владельцем объекта
								e.from->setOwner((tRenderRect*)this);
								tAbstractBasicClass::clearEvent(e);
								break;
							}
							case e.codes.UpdateTexture: {	// Обновить текстуру
								tAbstractBasicClass::message(getOwner(), e.types.Broadcast, e.codes.UpdateTexture, (tRenderRect*)this);
								need_rerender = true;
								tAbstractBasicClass::clearEvent(e);
								break;
							}
							default: {				// Если не обработалось, то "проталкиваем" на уровень ниже
								e.address = getOwner();
								tAbstractBasicClass::message(e);
								tAbstractBasicClass::clearEvent(e);
								break;
							}
						}
					}
					break;
				}
				case e.types.Button: {
					if (e.address == (tRenderRect*)this) {
						e.address = getOwner();
						tAbstractBasicClass::putEvent(e);
						tAbstractBasicClass::clearEvent(e);
					}
					break;
				}
				case e.types.Mouse: {
					mouse_inside[1] = mouse_inside[0];
					mouse_inside[0] = pointIsInsideMe({ e.mouse.x, e.mouse.y });
				}
			}
		}
	}

	void tDisplay::updateTexture() {
		render_texture.clear(clear_color);	// Очиститься
		tGroup::draw(render_texture);		// Нарисовать подэлементы
		render_texture.display();			// Обновить "лицевую" текстуру
	}

	void tDisplay::setOwner(tAbstractBasicClass* new_owner) {
		owner = new_owner;
	}

	void tDisplay::setTextureSize(const sf::Vector2u &new_size) {
		render_texture.create(new_size.x, new_size.y);
		need_rerender = true;
	}

	tAbstractBasicClass* tDisplay::getOwner() const {
		return owner;
	}

	bool tDisplay::pointIsInsideMe(sf::Vector2i point) const {
		sf::FloatRect rect = tRenderRect::getGlobalBounds();
		return (point.x >= rect.left && point.x <= rect.left + rect.width && point.y >= rect.top && point.y <= rect.top + rect.height);
	}

	sf::FloatRect tDisplay::getLocalBounds() const {
		return tRenderRect::getLocalBounds();
	}

	nlohmann::json tDisplay::getParamsInJson() const {
		nlohmann::json js = tGroup::getParamsInJson();
		nlohmann::json js1 = tRenderRect::getParamsInJson();

		for (nlohmann::json::iterator it = js1.begin(); it != js1.end(); it++) {	// Прикручиваем содержимое js1 к js
			js[it.key()] = it.value();
		}

		js["what_is_it"] = objects_json_ids.tDisplay;
		js["what_is_it_string"] = "tDisplay";
		js["camera_offset"] = { render_squad[0].texCoords.x, render_squad[0].texCoords.y };

		return js;
	}

	void tScrollbar::initScrollbar(const bool vertical) {
		slider->changeOneOption(option_mask.can_be_moved, true);
		slider->setColor({ 60, 60, 60, 255 });
		if (vertical) {
			arrow_first->setString(L"^");
			arrow_first->setPosition({ 0, 0 });
			arrow_second->setString(L"v");
			arrow_second->setAnchor(anchors.bottom_left_corner);
			arrow_second->setPosition({ 0, -thickness });
			slider->setPosition({ 1, thickness + 1 });
		}
		else {
			arrow_first->setString(L"<");
			arrow_first->setPosition({ 0, 0 });
			arrow_second->setString(L">");
			arrow_second->setAnchor(anchors.upper_right_corner);
			arrow_second->setPosition({ -thickness, 0 });
			slider->setPosition({ thickness + 1, 1 });
		}
		updateScrollerSize();
	}

	tScrollbar::tScrollbar(tAbstractBasicClass* _owner, const bool vertical, sf::FloatRect rect) :
		tRenderRect(_owner),
		arrow_first(new tButton(this, {0, 0, thickness, thickness})),
		arrow_second(new tButton(this, { 0, 0, thickness, thickness })),
		slider(new tRectShape(this, { 0, 0, thickness - 2, thickness - 2 })),
		old_position(0.f, 0.f),
		color_scroller_area({ 241, 241, 241, 255 }),
		target_size({ 1.f, 1.f }),
		target_texture_size({ 1, 1 })
	{
		changeOneOption(option_mask.can_be_moved, false);
		changeOneOption(option_mask.can_be_resized, false);
		changeOneOption(option_mask.vectically_orientated, vertical);

		clear_color = color_scroller_area;

		setPosition({ rect.left, rect.top });
		setSize({ rect.width, rect.height });
		setTextureSize({ (unsigned int)rect.width, (unsigned int)rect.height });

		initScrollbar(vertical);
	}

	tScrollbar::tScrollbar(tAbstractBasicClass* _owner, nlohmann::json& js) :
		tRenderRect(_owner, js),
		arrow_first(new tButton(this, js["arrow_first"])),
		arrow_second(new tButton(this, js["arrow_second"])),
		slider(new tRectShape(this, { 0, 0, thickness, thickness })),
		old_position(0.f, 0.f),
		target_size({ 1.f, 1.f }),
		target_texture_size({ 1, 1 })
	{
		vec<unsigned char> vuc = js["color_scroller_area"].get<vec<unsigned char>>();
		color_scroller_area = { vuc[0], vuc[1], vuc[2], vuc[3] };
		vuc.clear();

		vec<unsigned int> vui = js["target_texture_size"].get<vec<unsigned int>>();
		target_texture_size = { vui[0], vui[1] };
		vui.clear();

		vec<float> vuf = js["target_size"].get<vec<float>>();
		target_size = { vuf[0], vuf[1] };
		vuf.clear();

		clear_color = color_scroller_area;

		updateScrollerSize();
	}

	tScrollbar::tScrollbar(const tScrollbar& s) :
		tRenderRect(s),
		arrow_first(s.arrow_first),
		arrow_second(s.arrow_second),
		slider(s.slider),
		old_position(s.old_position),
		color_scroller_area(s.color_scroller_area),
		target_size(s.target_size),
		target_texture_size(s.target_texture_size)
	{
	}

	tScrollbar::~tScrollbar() {
		delete arrow_first, arrow_second, slider;
	}

	void tScrollbar::updateScrollerSize() {
		sf::FloatRect scrollbar_rect = getLocalBounds();
		sf::Vector2f scroller_size;

		if (checkOption(option_mask.vectically_orientated)) {
			// Отношение лежит в диапазоне от 0 до 1. Домножаем его на максимальный размер ползунка
			// с учётом "стрелок" на скроллбаре и получаем текущий размер ползунка.
			scroller_size.x = thickness - 3;
			scroller_size.y = (target_size.y / target_texture_size.y) * (scrollbar_rect.height - 2 * thickness) - 2;
		}
		else {
			scroller_size.x = (target_size.x / target_texture_size.x) * (scrollbar_rect.width - 2 * thickness) - 2;
			scroller_size.y = thickness - 3;
		}
		slider->setSize(scroller_size);
	}

	void tScrollbar::setTargetSize(const sf::Vector2f &new_size) {
		target_size = new_size;
	}

	void tScrollbar::setTargetTextureSize(const sf::Vector2u &new_size) {
		target_texture_size = new_size;
	}

	void tScrollbar::moveSlider(const int &_step) {
		sf::Vector2f step = { 0.f, 0.f };
		if (checkOption(option_mask.vectically_orientated)) {
			step.y = (float)_step / target_texture_size.y;
		}
		else {
			step.x = (float)_step / target_texture_size.x;
		}
		sf::Vector2f length = { 
			MAX(getLocalBounds().width - 2 * thickness, 0.f), 
			MAX(getLocalBounds().height - 2 * thickness, 0.f)
		};
		step.x *= length.x;
		step.y *= length.y;
		slider->move(step);
		message(this, tEvent::types.Broadcast, tEvent::codes.UpdateTexture, this);
	}

	float tScrollbar::getPixelOffset() const {
		float length_of_moving_zone, slider_pos, targ_tex_size, targ_size;
		if (checkOption(option_mask.vectically_orientated)) {
			length_of_moving_zone = getLocalBounds().height - slider->getLocalBounds().height - thickness;
			slider_pos = slider->getLocalBounds().top;
			targ_tex_size = (float)target_texture_size.y;
			targ_size = target_size.y;
		}
		else {
			length_of_moving_zone = getLocalBounds().width - slider->getLocalBounds().width - thickness;
			slider_pos = slider->getLocalBounds().left;
			targ_tex_size = (float)target_texture_size.x;
			targ_size = target_size.x;
		}
		/*
		lengthOfMovingZone				targetTextureSize - targetSize
		_________________________	=	______________________________
		sliderPos - thickness - 1		offset
		*/
		return (targ_tex_size - targ_size) * (slider_pos - thickness - 1) / length_of_moving_zone;
	}

	void tScrollbar::updateTexture() {
		render_texture.clear(clear_color);

		arrow_first->draw(render_texture);
		arrow_second->draw(render_texture);
		slider->draw(render_texture);

		render_texture.display();
	}

	void tScrollbar::handleEvent(tEvent& e) {
		if (checkOption(option_mask.can_be_drawn)) {
			arrow_first->handleEvent(e);
			arrow_second->handleEvent(e);
			slider->handleEvent(e);
			
			switch (e.type) {
				case e.types.Broadcast: {
					if (e.address == this) {
						switch (e.code) {
							case e.codes.Activate: {		// Установить фокус на объект
								((tObject*)e.from)->changeOneOption(tObject::option_mask.is_active, true);
								message(getOwner(), e.types.Broadcast, e.codes.Activate, this);
								clearEvent(e);
								break;
							}
							case e.codes.Deactivate: {	// Снять фокус с объекта
								((tObject*)e.from)->changeOneOption(tObject::option_mask.is_active, false);
								clearEvent(e);
								break;
							}
							case e.codes.Show: {			// Показать объект (если он скрыт)
								((tObject*)e.from)->changeOneOption(tObject::option_mask.can_be_drawn, true);
								clearEvent(e);
								break;
							}
							case e.codes.Hide: {			// Спрятать объект (если он не скрыт)
								((tObject*)e.from)->changeOneOption(tObject::option_mask.can_be_drawn, false);
								clearEvent(e);
								break;
							}
							case e.codes.UpdateTexture: {	// Обновить текстуру
								message(getOwner(), e.types.Broadcast, e.codes.UpdateTexture, this);
								need_rerender = true;
								clearEvent(e);
								break;
							}
							default: {		// Если не обработалось, то "проталкиваем" на уровень ниже
								e.address = getOwner();
								message(e);
								clearEvent(e);
								break;
							}
						}
					}
					break;
				}
				case e.types.Button: {
					if (e.address == this) {
						if (e.from == arrow_first || e.from == arrow_second) {
							if (e.from == arrow_first) {
								moveSlider(-default_step);
							}
							else {
								moveSlider(default_step);
							}
							message(this, e.types.Broadcast, e.codes.UpdateTexture, this);
						}
					}
					break;
				}
				case e.types.Mouse: {
					mouse_inside[1] = mouse_inside[0];
					mouse_inside[0] = pointIsInsideMe({ e.mouse.x, e.mouse.y });
					switch (e.code) {
						case e.codes.MouseMoved: {
							if (mouse_inside[0] != mouse_inside[1]) {
								message(nullptr, e.types.Broadcast, e.codes.ResetButtons, this);
								clearEvent(e);
							}
							if (mouse_inside[0]) {
								
								message(this, e.types.Broadcast, e.codes.UpdateTexture, this);
								clearEvent(e);
							}
							break;
						}
					}
					break;
				}
			}

			// Нужно вернуть из-за пределов скроллбара (если он там окажется)
			sf::FloatRect slider_rect = slider->getLocalBounds();
			sf::FloatRect rect = getLocalBounds();
			
			if ((old_position.x != slider_rect.left) || (old_position.y != slider_rect.top)) {
				if (checkOption(option_mask.vectically_orientated)) {
					slider_rect.left = 1;
					slider_rect.top = CLAMP(slider_rect.top, thickness + 1, rect.height - slider_rect.height - thickness - 1);
				}
				else {
					slider_rect.top = 1;
					slider_rect.left = CLAMP(slider_rect.left, thickness + 1, rect.width - slider_rect.width - thickness - 1);
				}
				slider->setPosition({ slider_rect.left, slider_rect.top });
				old_position = { slider_rect.left, slider_rect.top };
				message(this, e.types.Broadcast, e.codes.UpdateTexture, this);
			}
		}
	}

	void tScrollbar::setTextureSize(const sf::Vector2u &new_size) {
		render_texture.create(new_size.x, new_size.y);
		render_squad[0].texCoords = { 0.f, 0.f };
		render_squad[1].texCoords = { (float)new_size.x - 1, 0.f };
		render_squad[2].texCoords = { (float)new_size.x - 1, (float)new_size.y - 1 };
		render_squad[3].texCoords = { 0.f, (float)new_size.y - 1 };
		need_rerender = true;
		updateScrollerSize();
	}

	bool tScrollbar::pointIsInsideMe(sf::Vector2i point) const {
		sf::FloatRect rect = getGlobalBounds();
		return (point.x >= rect.left && point.x <= rect.left + rect.width && point.y >= rect.top && point.y <= rect.top + rect.height);
	}

	nlohmann::json tScrollbar::getParamsInJson() const {
		nlohmann::json js = tRenderRect::getParamsInJson();

		js["what_it_it"] = objects_json_ids.tScrollbar;
		js["what_is_it_string"] = "tScrollbar";

		js["arrow_first"] = arrow_first->getParamsInJson();
		js["arrow_second"] = arrow_second->getParamsInJson();
		
		sf::Color color = color_scroller_area;
		js["color_scroller_area"] = { color.r, color.g, color.b, color.a };

		js["target_texture_size"] = { target_texture_size.x, target_texture_size.y };
		js["target_size"] = { target_size.x, target_size.y };

		return js;
	}

}