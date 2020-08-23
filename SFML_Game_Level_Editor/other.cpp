#include "stdafx.h"
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

void make_json_user_friendly(std::string path_to_file) {
	std::fstream file(path_to_file, std::fstream::in);
	nlohmann::json js;
	if (file.is_open()) {
		file >> js;
		file.close();
		print_json(js, path_to_file);
		return;
	}
	std::cout << "make_json_user_friendly error: can't to find '" << path_to_file << "' file.\n";
}

void print_json(nlohmann::json& js, std::string path_to_file) {
	std::string content = js.dump();

	unsigned char state_machine = 0;
	unsigned int tabs = 0;
	bool need_tabs = false;
	unsigned char deferred_char;
	bool deferred = false;
	std::string buffer = "";

	std::fstream file(path_to_file, std::fstream::out);
	for (unsigned int i = 0; i < content.size(); i++) {
		if (need_tabs) {			// Если надо, печатаем табы
			need_tabs = false;
			for (unsigned int j = 0; j < tabs; j++) {
				buffer += '\t';
			}
			file << buffer;
			buffer = "";
		}
		if (deferred) {		// Печать отложенного символа
			deferred = false;
			file << deferred_char;
		}
		switch (state_machine) {
		case 0: {					// Стандартная обработка и вывод
			switch (content[i]) {
			case '"': {
				state_machine = 1;
				buffer += "\"";
				break;
			}
			case '{':
			case '[': {
				file << content[i] << '\n';
				tabs++;
				need_tabs = true;
				break;
			}
			case '}':
			case ']': {
				file << '\n';
				tabs--;
				need_tabs = true;
				deferred = true;
				deferred_char = content[i];
				break;
			}
			case ',': {
				file << ",\n";
				need_tabs = true;
				break;
			}
			default: {
				file << content[i];
				break;
			}
			}
			break;
		}
		case 1: {					// Вывод строки (частный случай, тк в ней могут содержаться влияющие на форматирование символы)
			buffer += content[i];
			if (content[i] == '"') {
				file << buffer;
				buffer = "";
				state_machine = 0;
			}
			break;
		}
		}
	}
	if (deferred) {		// Печать отложенного символа
		deferred = false;
		file << deferred_char;
	}
	file.close();
}

std::wstring parseIntVectorToWstring(std::vector<int>& vec) {
	std::wstring str = L"";
	wchar_t wchar;
	for (unsigned int i = 0; i < vec.size() / 2; i++) {
		wchar = (vec[i] & 0xFF) << 8 | (vec[i + 1] & 0xFF);
		str += wchar;
	}
	return str;
}
