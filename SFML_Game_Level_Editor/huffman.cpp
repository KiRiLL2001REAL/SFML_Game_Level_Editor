#include "huffman.h"
#include <queue>

namespace huf {

	void huffman_compression::makeCodes(tTree* root, std::map<unsigned char, std::vector<bool>>& codes, std::vector<bool>& current_code) {
		if (root->c != 0) {
			codes[root->c] = current_code;
		}
		else {
			if (root->left) {	// 0
				current_code.push_back(false);
				makeCodes(root->left, codes, current_code);
				current_code.pop_back();
			}
			if (root->right) {	// 1
				current_code.push_back(true);
				makeCodes(root->right, codes, current_code);
				current_code.pop_back();
			}
		}
	}

	huffman_compression::tTree* huffman_compression::makeTree(std::map<unsigned char, unsigned int>& freq) {
		std::list<tTree*> list;	// ��������������� ������ � ���������� ��������� ��� ���������� ������ �����

		for (auto& it : freq) {	// ������������� ������
			list.push_back(new tTree(it.first, it.second));
		}

		while (list.size() != 1) {	// �������� ������ �����
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
		if (root->c != 0) {
			std::cout << "'" << root->c << "' : " << root->count << "\n";
		}
		else {
			std::cout << root->count << "\n";
		}
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

	void huffman_compression::compress(std::string path_to_file_from_folder, std::string resulting_file_extention) {
		selectFile(path_to_file_from_folder);
		std::ifstream file(path_to_folder + filename);
		if (file.is_open() && resFilename != "") {
			resFilename += resulting_file_extention;

			std::map<unsigned char, unsigned int> freq;	// ������������� ������ ��� �������� ���-�� ��������� ������� �� ��������
			unsigned int dictionary_size = 0;	// ���-�� �������� � �������
			while (!file.eof()) {	// ������� ���������
				unsigned char c;
				c = file.get();
				//if (c == 255) continue;	// ���� �� EOF, �� ������� ������
				freq[c]++;
				if (freq[c] == 1) dictionary_size++;
			}
			
			tTree* huffman_root = makeTree(freq);					// ������ ������ ��������
			//printTree(huffman_root);
			std::map<unsigned char, std::vector<bool> > codes;	// ������������� ������ ��� �������� �������� � ����� � ��� � ���� vector<bool>
			std::vector<bool> temp_vec = {};	// ������ ����
			makeCodes(huffman_root, codes, temp_vec);			// ��������� ����� � ���� vector<bool>
			delete huffman_root;									// ���� �������������, �������� ������ ������ �� ���������
			
			std::ofstream ofile(path_to_folder + resFilename, std::ofstream::binary);	// �������� ����� ��� ������

			ofile.write((char*)&dictionary_size, sizeof(dictionary_size));	// ������ ������� �������
			for (auto& it : freq) {										
				ofile.write((char*)&it.first, sizeof(it.first));	// ������ �����
				ofile.write((char*)&it.second, sizeof(it.second));	// ������ �������
			}

			file.clear(); file.seekg(0);	// ��������� �� ������ ����������� �����
			unsigned int byte = 0;
			unsigned char bit_count = 0;	// ���-�� �������� ��� � ��������� �����

			std::list<char> buffer = {};	// ��������� ��� � ���� (��������), �������� 'bit_count', � ����� ������� ����� � ����

			while (!file.eof()) {	// ������ ����
				unsigned char c;
				c = file.get();
				std::vector<bool> bits = codes[c];	// ��������� ��� (vector<bool>) � ������ ��� ��������� ��������
				for (std::vector<bool>::iterator it = bits.begin(); it != bits.end(); it++) {
					byte = byte << 1;	// �������� ��� ������� � ���������� 'byte'
					byte |= (*it) && true;
					bit_count++;
				}
				while (bit_count >= 8) {	// ���� ������� 8 ���, �� ���������� �� � �����
					unsigned char a = byte >> (bit_count - 8);	// ������ � 'a' ������ 8 ��� ����
					buffer.push_back(a);
					a = 0;	// ������ 'a' ������ ������
					for (unsigned char i = 0; i < (bit_count - 8); i++) {
						a = a << 1;
						a |= 1;
					}
					byte &= a;	// ������� ��, ��� �� �������� � ����� (������ ���������� �����)
					bit_count -= 8;
				}
			}
			if (byte != 0) {		// ���������� � ����� ��, ��� ��������
				buffer.push_back(byte);
			}

			ofile.write((char*)&bit_count, sizeof(bit_count));	// ���������� � �������������� ���� ���-�� �������� ��� � ��������� �����
			for (auto& it : buffer) {	// ���������� � �������������� ���� '�������� ����' �� ������
				ofile.write((char*)&it, sizeof(it));
			}

			file.close();
			ofile.close();
		}
		else {
			std::cout << "huffman error: file '.." << filename << "' does not exist.\n";
		}
	}

	void huffman_compression::decompress(std::string path_to_file_from_folder, std::string resulting_file_extention) {
		selectFile(path_to_file_from_folder);
		std::ifstream file(path_to_folder + filename);
		if (file.is_open() && resFilename != "") {
			resFilename += resulting_file_extention;

			unsigned int dictionary_size;
			file.read((char*)&dictionary_size, sizeof(dictionary_size));
			
			std::map<unsigned char, unsigned int> freq;	// ������������� ������ ��� �������� ���-�� ��������� ������� �� ��������
			for (unsigned int i = 0; i < dictionary_size; i++) {	// ����������
				unsigned char c;
				file.read((char*)&c, sizeof(c));
				unsigned int f;
				file.read((char*)&f, sizeof(f));
				freq[c] = f;
				//std::cout << "'" << c << "' (" << (int)c << ") : " << f << "\n";
			}

			unsigned char last_byte_bit_count;
			file.read((char*)&last_byte_bit_count, sizeof(last_byte_bit_count));

			std::ofstream ofile(path_to_folder + resFilename);	// �������� ����� ��� ������

			tTree* huffman_root = makeTree(freq);				// ������ ������ ��������
			//std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n";
			//printTree(huffman_root);
			tTree* p = huffman_root;							// ���������, ��������� �� ������ � ���������� ��� � �����

			while (!file.eof()) {
				unsigned char c;
				file.read((char*)&c, sizeof(c));
				std::list<bool> char_bits = {};
				for (int i = 0; i < 8; i++) {
					unsigned char a = c % 2;
					char_bits.push_front(a);	// 1 - true, 0 - false
					c = c >> 1;
				}
				if (file.eof() && last_byte_bit_count != 0) {	// ��������� ���� ���� �����������
					for (char i = 0; i < 8 - last_byte_bit_count; i++) {
						char_bits.pop_front();
					}
				}
				if (!file.eof())
				for (auto& it : char_bits) {
					if (it) { p = p->right; }
					else { p = p->left; }
					if (p->c != 0) {
						ofile.write((char*)&p->c, sizeof(p->c));
						p = huffman_root;
					}
				}
			}

			delete huffman_root;
			file.close();
			ofile.close();
		}
		else {
			std::cout << "huffman error: file '.." << filename << "' does not exist.\n";
		}
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
	
	bool huffman_compression::myCompare::operator()(tTree* a, tTree* b) const {
		return a->count < b->count;
	}
}