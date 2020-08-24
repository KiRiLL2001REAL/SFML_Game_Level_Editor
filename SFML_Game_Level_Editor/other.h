#pragma once

std::string cutOffLast(std::string str, int count_words);	// Отрезает несколько разделённых знаком '\\' слов с конца

void make_json_user_friendly(std::string path_to_file);		// Перерабатывает json в читабельный вид, если он представлен в виде строки

void print_json(nlohmann::json &js, std::string path_to_file);		// Выводит json-структуру в файл в читабельном виде

std::wstring convertIntVectorToWstring(std::vector<int>& vec);	// Преобразует std::vector<int> в std::wstring