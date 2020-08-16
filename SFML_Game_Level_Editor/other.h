#pragma once

const float hd_ratio = 16.f / 9.f;

std::string cutOffLast(std::string str, int count_words);	// Отрезает несколько разделённых знаком '\\' слов с конца

sf::Vector2u setSizeIn16_9Ratio(sf::Vector2u size);			// Подгоняет заданный размер под формат 16:9