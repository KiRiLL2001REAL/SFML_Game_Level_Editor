#include "other.h"

std::string cutOffLast(std::string str, int count_words) {
	std::string String = str;
	int level_down = count_words;
	int i = String.length();
	while (level_down != 0 && String.length() != 0) {
		if (String[i] != '\\' && i > 0) {
			i--;
		}
		else {
			level_down--;
			String.erase(i, String.length() - i);
			i = String.length();
		}
	}
	return String;
}

sf::Vector2u setSizeIn16_9Ratio(sf::Vector2u size) {
	int w = size.x;
	int h = size.y;

	if ((float)w / h >= hd_ratio) {	// Пользователь вытянул окно по горизонтали
		if (h % 9 != 0)		// Восстановление формата 16:9
			h += 9 - h % 9;
		w = int((float)h * hd_ratio);
	}
	else {							// Пользователь вытянул окно по вертикали
		if (w % 16 != 0)	// Восстановление формата 16:9
			w += 16 - w % 16;
		h = int((float)w / hd_ratio);
	}

	return sf::Vector2u(w, h);
}