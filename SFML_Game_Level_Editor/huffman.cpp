#include "huffman.h"

namespace huf {

	void huffman_compression::makeCodes(tTree* root, std::map<char, std::vector<bool>>& codes, std::vector<bool>& current_code) {
		if (root->c != 0) {
			codes[root->c] = current_code;
		}
		else {
			if (root->left) {
				current_code.push_back(false);
				makeCodes(root->left, codes, current_code);
				current_code.pop_back();
			}
			if (root->right) {
				current_code.push_back(true);
				makeCodes(root->right, codes, current_code);
				current_code.pop_back();
			}
		}
	}

	huffman_compression::tTree* huffman_compression::makeTree(std::map<char, unsigned int>& freq) {
		std::list<tTree*> list;	// Вспомогательный список с найденными символами для построения дерева кодов

		for (auto& it : freq) {	// Инициализация списка
			list.push_back(new tTree(it.first, it.second));
		}

		while (list.size() != 1) {	// Создание дерева кодов
			list.sort(myCompare());
			tTree* first = list.front();
			list.pop_front();
			tTree* second = list.front();
			list.pop_front();
			tTree* node = new tTree(first, second);
			list.push_front(node);
		}

		return list.front();
	}

	/*
	void huffman_compression::printTree(tTree* root, unsigned int tabs) {
		if (root->left) {
			printTree(root->left, tabs + 1);
		}
		if (root->right) {
			printTree(root->right, tabs + 1);
		}
		for (unsigned int i = 0; i < tabs; i++) {
			std::cout << "\t";
		}
		if (!root->left && !root->right) {
			std::cout << "'" << root->c << "' : " << root->count << "\n";
		}
		else {
			std::cout << root->count << "\n";
		}
	}
	*/

	huffman_compression::huffman_compression(std::string _path_to_folder) :
		path_to_folder(_path_to_folder),
		filename(""),
		resFilename("")
	{
	}

	void huffman_compression::selectFileToCompress(std::string path_to_file_from_folder) {
		filename = path_to_file_from_folder;
		resFilename = filename;
		std::string::iterator it;
		for (it = resFilename.end() - 1; it >= resFilename.begin(); it--) {
			if (*it == '.') break;
		}
		resFilename.erase(it, resFilename.end());
	}

	void huffman_compression::compress(std::string path_to_file_from_folder, std::string resulting_file_extention) {
		selectFileToCompress(path_to_file_from_folder);
		std::ifstream file(path_to_folder + filename);
		if (file.is_open() && resFilename != "") {
			resFilename += resulting_file_extention;

			std::map<char, unsigned int> freq;	// Ассоциативный массив для подсчёта кол-ва вхождений каждого из символов
			unsigned int dictionary_size = 0;	// Кол-во символов в словаре
			while (!file.eof()) {	// Подсчёт вхождений
				char c;
				file >> c;
				freq[c]++;
				if (freq[c] == 1) dictionary_size++;
			}
			
			tTree* huffman_root = makeTree(freq);					// Корень дерева хаффмана
			std::map<char, std::vector<bool> > codes;	// Ассоциативный массив для хранения символов и кодов к ним в виде vector<bool>
			std::vector<bool> temp_vec = {};	// Просто надо
			makeCodes(huffman_root, codes, temp_vec);	// Генерация кодов в виде vector<bool>
			delete huffman_root;									// Коды сгенерированы, хранение дерева больше не требуется
			
			std::ofstream ofile(path_to_folder + resFilename, std::ofstream::binary);	// Открытие файла для записи

			ofile.write((char*)&dictionary_size, sizeof(dictionary_size));	// Запись размера словаря
			for (auto& it : freq) {										
				ofile.write((char*)&it.first, sizeof(it.first));	// Запись буквы
				ofile.write((char*)&it.second, sizeof(it.second));	// Запись частоты
			}

			file.clear(); file.seekg(0);	// Переходим на начало кодируемого файла
			unsigned int byte = 0;
			while (!file.eof()) {	// Читаем файл
				char c;
				file >> c;
				std::vector<bool> bits = codes[c];	// Соотносим код (vector<bool>) с только что считанным символом
				for (unsigned int i = 0; i < bits.size(); i++) {	// Получаем код символа в переменную 'byte'
					byte = byte << 1;
					if (bits[i]) {
						byte |= 1;
					}
				}
				if (byte >= 255) {	// Если собрали 8 бит, то записываем их в результирующий файл
					unsigned char a = byte % 256;
					byte = byte >> 8;
					ofile.write((char*)&a, sizeof(a));
				}
			}
			if (byte != 0) {		// Записываем в результирующий файл то, что осталось
				unsigned char a = byte;
				ofile.write((char*)&a, sizeof(a));
			}

			file.close();
			ofile.close();
		}
		else {
			std::cout << "huffman error: file '.." << filename << "' does not exist.\n";
		}
	}

	huffman_compression::tTree::tTree(char _c, unsigned int _count) :
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
	
	bool huffman_compression::myCompare::operator()(tTree* a, tTree* b) const {
		return a->count < b->count;
	}
}