#pragma once

std::string cutOffLast(std::string str, int count_words);	// �������� ��������� ���������� ������ '\\' ���� � �����

void make_json_user_friendly(std::string path_to_file);		// �������������� json � ����������� ���, ���� �� ����������� � ���� ������

void print_json(nlohmann::json &js, std::string path_to_file);		// ������� json-��������� � ���� � ����������� ����

std::wstring convertIntVectorToWstring(std::vector<int>& vec);	// ����������� std::vector<int> � std::wstring