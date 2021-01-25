#include "stdafx.h"
#include <string>
#include <fstream>
#include <vector>
#include <map>

#include "huffman.h"

namespace huf {

	void huffman_compression::makeCodes(tTree* root, std::map<unsigned char, std::string>& codes, std::string& current_code) {
		if (!root->left && !root->right) {
			codes[root->c] = current_code;
		}
		else {
			if (root->left) {
				current_code += "0";
				makeCodes(root->left, codes, current_code);
				current_code.erase(current_code.end() - 1);
			}
			if (root->right) {
				current_code += "1";
				makeCodes(root->right, codes, current_code);
				current_code.erase(current_code.end() - 1);
			}
		}
	}

	huffman_compression::tListNode* huffman_compression::makeTree(std::map<unsigned char, unsigned int>& freq) {
		std::map<unsigned char, unsigned int>::iterator it = freq.begin();
		
		tListNode* list_first, * list_last, * p;
		unsigned int list_count_elements = 0;

		list_first = new tListNode();						// Инициализация списка, заполнение первой ячейки
		list_first->el = new tTree(it->first, it->second);
		list_last = list_first;

		it++;
		list_count_elements++;
		while (it != freq.end()) {	// Заполнение списка
			p = new tListNode();
			p->el = new tTree(it->first, it->second);
			list_last->next = p;
			p->prev = list_last;
			list_last = p;

			it++;
			list_count_elements++;
		}

		tListNode* min[2];
		while (list_count_elements != 1) {
			/*
			
			Поиск двух минимумов типа tListNode;

			*/
			min[0] = nullptr; min[1] = nullptr;		
			p = list_first;
			while (p) {
				if (!min[0] || p->el->count < min[0]->el->count) {
					min[1] = min[0];
					min[0] = p;
				}
				else if (!min[1] || p->el->count < min[1]->el->count) {
					min[1] = p;
				}
				p = p->next;
			}

			list_count_elements--;		// Каждую итерацию "извлекается" 1 элемент списка, а второй изменяется.

			tTree* m[2];	// Созраняем tTree элементы, 
			for (char i = 0; i < 2; i++) m[i] = min[i]->el;

			/*
			
			Удаление первого минимума из списка
			
			*/
			p = min[0]->prev;
			if (min[0]->prev) {			// Если min[0] не является началом списка
				p->next = min[0]->next;			// Пробрасываем "next" указатель элемента "p" через "min[0]" к "min[0]->next"
			}
			else {
				list_first = list_first->next;	// Иначе переставляем list_first на элемент вперёд, чтобы не испортить указатель
			}
			if (min[0]->next) {			// Если min[0] не является концом списка
				min[0]->next->prev = p;			// Пробрасываем "prev" указатель элемента "min[0]->next" через "min[0]" к "p"
			}
			else {
				list_last = list_last->prev;	// Иначе переставляем list_last на элемент назад, чтобы не испортить указатель
			}
			delete min[0];

			/*
			
			Создание нового узла tTree и помещение его во второй минимум
			
			*/
			min[1]->el = new tTree(m[0], m[1]);
		}

		return list_first;
	}

	huffman_compression::huffman_compression(std::string _path_to_folder) :
		path_to_folder(_path_to_folder),
		filename(""),
		resFilename("")
	{
	}

	void huffman_compression::selectFile(std::string path_to_file_from_folder) {
		filename = path_to_file_from_folder;
		resFilename = filename;
		std::string::iterator it;
		for (it = resFilename.end() - 1; it >= resFilename.begin(); it--) {
			if (*it == '.') break;
		}
		resFilename.erase(it, resFilename.end());
	}

	bool huffman_compression::compress(std::string path_to_file_from_folder, std::string resulting_file_extention) {
		selectFile(path_to_file_from_folder);
		std::fstream file(path_to_folder + filename, std::fstream::binary | std::fstream::in);
		if (file.is_open() && resFilename != "") {
			resFilename += resulting_file_extention;

			unsigned long long file_size;	// Вычислим размер файла в байтах
			file.seekg(0, std::fstream::end);
			file_size = file.tellg();
			file.clear(); file.seekg(0);

			/*
			
			ПОДСЧЁТ ЧАСТОТЫ ВХОЖДЕНИЯ ДЛЯ КАЖДОГО СИМВОЛА
			
			*/

			std::map<unsigned char, unsigned int> freq;	// Ассоциативный массив для подсчёта кол-ва вхождений каждого из символов

			for (unsigned long long iii = 0; iii < file_size; iii++) {	// Подсчёт вхождений символов
				unsigned char c;
				file.read((char*)&c, sizeof(c));
				freq[c]++;
			}
			unsigned int dictionary_size = freq.size();;	// Кол-во символов в словаре

			/*

			ПОСТРОЕНИЕ КОДОВ

			*/

			tListNode* ln = makeTree(freq);
			tTree* huffman_root = ln->el;					// Корень дерева хаффмана
			delete ln;
			std::map<unsigned char, std::string> codes;	// Ассоциативный массив для хранения символов и кодов к ним в виде string
			std::string temp_str = {};	// Просто надо
			makeCodes(huffman_root, codes, temp_str);	// Генерация кодов в виде string
			delete huffman_root;									// Коды сгенерированы, хранение дерева больше не требуется
			
			std::fstream ofile(path_to_folder + resFilename, std::ofstream::binary | std::fstream::out);	// Открытие файла для записи

			std::string head = "HUF00";
			for (int i = 0; i < 5; i++) ofile.write((char*)&head[i], sizeof(head[i]));

			ofile.write((char*)&dictionary_size, sizeof(dictionary_size));	// Запись размера словаря
			for (auto& it : freq) {										
				ofile.write((char*)&it.first, sizeof(it.first));	// Запись буквы
				ofile.write((char*)&it.second, sizeof(it.second));	// Запись частоты
			}

			/*

			КОДИРОВАНИЕ ФАЙЛА

			*/

			file.clear(); file.seekg(0);	// Переходим на начало кодируемого файла

			unsigned long long bc = 0;	// Найду кол-во полезных бит в последнем байте
			for (auto& it : freq) {
				bc += (unsigned long long)it.second * (codes[it.first].size());	// [частота вхождения символа]*[длина кода символа]
			}
			unsigned char bit_count = bc % 8;	// <--- Вот оно

			ofile.write((char*)&bit_count, sizeof(bit_count));	// Записываем в результирующий файл кол-во полезных бит в последнем байте
			
			bit_count = 0;	// Теперь здесь лежит кол-во бит, собранных в byte

			unsigned char c;
			std::string byte = "";
			std::vector<unsigned char> data_buffer = {};
			unsigned int lalala = 0;

			for (unsigned long long iii = 0; iii < file_size; iii++) {	// Читаем файл	
				file.read((char*)&c, sizeof(c));
				byte += codes[c];
				bit_count += (unsigned char)codes[c].size();
				while (bit_count >= 8) {	// Если собрали хотя бы 8 бит, то записываем их (8) в файл
					std::string tmp = "        ";	// В 'a' находятся первые 8 бит кода
					for (int i = 0; i < 8; i++) { tmp[i] = byte[i]; }
					unsigned char a = 0;
					for (unsigned char i = 0; i < 8; i++) {
						a = a << 1;
						a |= tmp[i] == '1';
					}

					if (lalala % 2 == 0) {	// немного безумия
						a = ~a;
					}
					lalala++;

					data_buffer.push_back(a); //ofile.write((char*)&a, sizeof(a));
					byte.erase(0, 8);	// Оставим то, что не записали в файл (начало следующего байта(-ов))
					bit_count -= 8;
				}
				
			}
			if (bit_count != 0) {		// Записываем в буфер то, что осталось
				unsigned char a = 0;
				for (unsigned char i = 0; i < byte.size(); i++) {
					a = a << 1;
					a |= byte[i] == '1';
				}

				if (lalala % 2 == 0) {	// немного безумия
					a = ~a;
				}
				lalala++;

				data_buffer.push_back(a); //ofile.write((char*)&a, sizeof(a));
			}

			for (auto& it : data_buffer) { // Вывод data_buffer в файл
				ofile.write((char*)&it, sizeof(it));
			}

			ofile.close();
			file.close();
			return true;
		}
		else {
			std::cout << "Huffman error: file '.." << filename << "' does not exist.\n";
		}
		return false;
	}

	bool huffman_compression::decompress(std::string path_to_file_from_folder, std::string resulting_file_extention) {
		selectFile(path_to_file_from_folder);
		std::fstream file(path_to_folder + filename, std::fstream::binary | std::fstream::in);
		if (file.is_open() && resFilename != "") {
			resFilename += resulting_file_extention;

			unsigned long long file_size;	// Вычислим размер файла в байтах
			file.seekg(0, std::fstream::end);
			file_size = file.tellg();
			file.clear(); file.seekg(0);

			file_size -= 5;
			std::string head = "     ";		// Проверяем на совпадение HEAD файлика
			for (int i = 0; i < 5; i++) file.read((char*)&head[i], sizeof(head[i]));
			if (head == "HUF00") {
				/*

				СЧИТЫВАНИЕ СЛОВАРЯ

				*/

				unsigned int dictionary_size;
				file.read((char*)&dictionary_size, sizeof(dictionary_size));

				file_size -= sizeof(dictionary_size);

				std::map<unsigned char, unsigned int> freq;	// Ассоциативный массив для хранения кол-ва вхождений каждого из символов
				for (unsigned int i = 0; i < dictionary_size; i++) {	// Считывание
					unsigned char c;
					file.read((char*)&c, sizeof(c));
					unsigned int f;
					file.read((char*)&f, sizeof(f));
					freq[c] = f;

					file_size -= sizeof(c) + sizeof(f);
				}

				unsigned char last_byte_bit_count;
				file.read((char*)&last_byte_bit_count, sizeof(last_byte_bit_count));

				file_size -= sizeof(last_byte_bit_count);
				//
				// ТЕПЕРЬ file_size СОДЕРЖИТ В СЕБЕ РАЗМЕР, НЕПОСРЕДСТВЕННО, ИНТЕРЕСУЮЩЕЙ НАС ЧАСТИ ФАЙЛА (той, где код)
				//

				std::fstream ofile(path_to_folder + resFilename, std::fstream::out | std::fstream::binary);	// Открытие файла для записи

				tListNode* ln = makeTree(freq);
				tTree* huffman_root = ln->el;				// Корень дерева хаффмана
				delete ln;
				
				tTree* p = huffman_root;							// Указатель, прыгающий по дереву и собирающий код в буквы
				
				unsigned char mask[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };
				unsigned begI = 0; 
				unsigned int lalala = 0;

				for (unsigned long long iii = 0; iii < file_size; iii++) {
					unsigned char c;
					file.read((char*)&c, sizeof(c));

					if (lalala % 2 == 0) {
						c = ~c;
					}
					lalala++;

					if (last_byte_bit_count != 0)
						if (iii == file_size - 1) {
							begI = 8 - last_byte_bit_count;
						}
					for (int i = begI; i < 8; i++) {
						if ((c & mask[i]) == 0) {
							p = p->left;
						}
						else {
							p = p->right;
						}
						if ((!p->left) && (!p->right)) {
							ofile.write((char*)&p->c, sizeof(p->c));
							p = huffman_root;
						}
					}
				}

				delete huffman_root;
				file.close();
				ofile.close();

				return true;
			}
			else {
				std::cout << "Huffman decompressing failed. Signature of file '.." << filename << "' does not match with huffman's one.\n";
			}
		}
		else {
			std::cout << "Huffman error: file '.." << filename << "' does not exist.\n";
		}
		return false;
	}

	huffman_compression::tTree::tTree(unsigned char _c, unsigned int _count) :
		c(_c),
		count(_count),
		left(nullptr),
		right(nullptr)
	{
	}
	
	huffman_compression::tTree::tTree(tTree* _left, tTree* _right) :
		c(0),
		count(_left->count + _right->count),
		left(_left),
		right(_right)
	{
	}
	
	huffman_compression::tTree::~tTree() {
		if (left) {
			delete left;
		}
		if (right) {
			delete right;
		}
	}
	
	huffman_compression::tListNode::tListNode() :
		el(nullptr),
		next(nullptr),
		prev(nullptr)
	{

	}
}