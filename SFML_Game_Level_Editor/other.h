#pragma once

const float hd_ratio = 16.f / 9.f;

std::string cutOffLast(std::string str, int count_words);	// �������� ��������� ���������� ������ '\\' ���� � �����

sf::Vector2u setSizeIn16_9Ratio(sf::Vector2u size);			// ��������� �������� ������ ��� ������ 16:9