#pragma once

namespace edt {

	class tAbstractBasicClass;

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
			static const unsigned int StopAndDoNotMove = 16;
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
}