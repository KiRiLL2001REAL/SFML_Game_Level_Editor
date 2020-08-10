#include "huffman.h"
#include <queue>

namespace huf {

	void huffman_compression::makeCodes(tTree* root, std::map<unsigned char, std::string>& codes, std::string& current_code) {
		if (root->is_letter) {
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
		if (root->is_letter) {
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

	void huffman_compression::compress(std::string path_to_file_from_folder, std::string resulting_file_extention, bool debug) {
		selectFile(path_to_file_from_folder);
		std::fstream file(path_to_folder + filename, std::fstream::binary | std::fstream::in);
		if (file.is_open() && resFilename != "") {
			resFilename += resulting_file_extention;

			unsigned long long file_size;	// �������� ������ ����� � ������
			file.seekg(0, std::fstream::end);
			file_size = file.tellg();
			file.clear(); file.seekg(0);

			/*
			
			����ר� ������� ��������� ��� ������� �������
			
			*/

			std::map<unsigned char, unsigned int> freq;	// ������������� ������ ��� �������� ���-�� ��������� ������� �� ��������

			if (debug) {
				std::cout << "\n>>>>>DEBUG Huffman compressing...\n\n";
			}

			for (unsigned long long iii = 0; iii < file_size; iii++) {	// ������� ��������� ��������
				unsigned char c;
				file.read((char*)&c, sizeof(c));
				freq[c]++;
			}
			unsigned int dictionary_size = freq.size();;	// ���-�� �������� � �������
			if (debug) {
				std::cout << "Dictionary size: " << dictionary_size << " symbols.\n";
				for (auto& it : freq) {
					std::cout << "'" << it.first << "' (" << (int)it.first << ") : " << it.second << "\n";
				}
				std::cout << "End of dictionary.\n\n";
			}
			
			/*

			���������� �����

			*/

			tTree* huffman_root = makeTree(freq);					// ������ ������ ��������
			if (debug) {
				std::cout << "Huffman tree:\n";
				printTree(huffman_root);
				std::cout << "End of Huffman tree.\n\n";
			}
			std::map<unsigned char, std::string> codes;	// ������������� ������ ��� �������� �������� � ����� � ��� � ���� string
			std::string temp_str = {};	// ������ ����
			makeCodes(huffman_root, codes, temp_str);	// ��������� ����� � ���� string
			delete huffman_root;									// ���� �������������, �������� ������ ������ �� ���������
			
			if (debug) {
				std::cout << "Huffman codes for each symbol of dictionary:\n";
				for (auto& it : codes) {
					std::cout << "'" << it.first << "' (" << (int)it.first << ") : ";
					for (unsigned int i = 0; i < it.second.size(); i++) {
						(it.second[i] == '1') ? std::cout << 1 : std::cout << 0;
					}
					std::cout << "\n";
				}
				std::cout << "End of Huffman codes.\n\n";
			}

			std::fstream ofile(path_to_folder + resFilename, std::ofstream::binary | std::fstream::out);	// �������� ����� ��� ������

			std::string head = "HUF00";
			for (int i = 0; i < 5; i++) ofile.write((char*)&head[i], sizeof(head[i]));

			ofile.write((char*)&dictionary_size, sizeof(dictionary_size));	// ������ ������� �������
			for (auto& it : freq) {										
				ofile.write((char*)&it.first, sizeof(it.first));	// ������ �����
				ofile.write((char*)&it.second, sizeof(it.second));	// ������ �������
			}

			/*

			����������� �����

			*/

			file.clear(); file.seekg(0);	// ��������� �� ������ ����������� �����

			unsigned long long bc = 0;	// ����� ���-�� �������� ��� � ��������� �����
			for (auto& it : freq) {
				bc += (unsigned long long)it.second * (codes[it.first].size());	// [������� ��������� �������]*[����� ���� �������]
			}
			unsigned char bit_count = bc % 8;	// <--- ��� ���

			if (debug) {
				std::cout << "Last byte length (in bits): " << (int)bit_count << ".\n\n";
			}

			ofile.write((char*)&bit_count, sizeof(bit_count));	// ���������� � �������������� ���� ���-�� �������� ��� � ��������� �����
			
			bit_count = 0;	// ������ ����� ����� ���-�� ���, ��������� � byte

			unsigned char c;
			std::string byte = "";

			for (unsigned long long iii = 0; iii < file_size; iii++) {	// ������ ����	
				file.read((char*)&c, sizeof(c));
				byte += codes[c];
				bit_count += codes[c].size();
				while (bit_count >= 8) {	// ���� ������� ���� �� 8 ���, �� ���������� �� (8) � ����
					std::string tmp = "        ";	// � 'a' ��������� ������ 8 ��� ����
					for (int i = 0; i < 8; i++) { tmp[i] = byte[i]; }
					unsigned char a = 0;
					for (unsigned char i = 0; i < 8; i++) {
						a = a << 1;
						a |= tmp[i] == '1';
					}
					ofile.write((char*)&a, sizeof(a));
					byte.erase(0, 8);	// ������� ��, ��� �� �������� � ���� (������ ���������� �����(-��))
					bit_count -= 8;
				}
				
			}
			if (bit_count != 0) {		// ���������� � ����� ��, ��� ��������
				unsigned char a = 0;
				for (unsigned char i = 0; i < byte.size(); i++) {
					a = a << 1;
					a |= byte[i] == '1';
				}
				ofile.write((char*)&a, sizeof(a));
			}

			ofile.close();
			
			file.close();

			if (debug) {
				std::cout << ">>>>>DEBUG Huffman compressing done. Check the file '.." << resFilename << "'.\n\n";
			}
		}
		else {
			std::cout << "Huffman error: file '.." << filename << "' does not exist.\n";
		}
	}

	void huffman_compression::decompress(std::string path_to_file_from_folder, std::string resulting_file_extention, bool debug) {
		selectFile(path_to_file_from_folder);
		std::fstream file(path_to_folder + filename, std::fstream::binary | std::fstream::in);
		if (file.is_open() && resFilename != "") {
			resFilename += resulting_file_extention;

			if (debug) {
				std::cout << "\n>>>>>DEBUG Huffman decompressing...\n\n";
			}

			unsigned long long file_size;	// �������� ������ ����� � ������
			file.seekg(0, std::fstream::end);
			file_size = file.tellg();
			file.clear(); file.seekg(0);

			file_size -= 5;
			std::string head = "     ";		// ��������� �� ���������� HEAD �������
			for (int i = 0; i < 5; i++) file.read((char*)&head[i], sizeof(head[i]));
			if (head == "HUF00") {
				/*

				���������� �������

				*/

				unsigned int dictionary_size;
				file.read((char*)&dictionary_size, sizeof(dictionary_size));

				file_size -= sizeof(dictionary_size);

				std::map<unsigned char, unsigned int> freq;	// ������������� ������ ��� �������� ���-�� ��������� ������� �� ��������
				for (unsigned int i = 0; i < dictionary_size; i++) {	// ����������
					unsigned char c;
					file.read((char*)&c, sizeof(c));
					unsigned int f;
					file.read((char*)&f, sizeof(f));
					freq[c] = f;

					file_size -= sizeof(c) + sizeof(f);
				}

				if (debug) {
					std::cout << "Dictionary size: " << dictionary_size << " symbols.\n";
					for (auto& it : freq) {
						std::cout << "'" << it.first << "' (" << (int)it.first << ") : " << it.second << "\n";
					}
					std::cout << "End of dictionary.\n\n";
				}

				unsigned char last_byte_bit_count;
				file.read((char*)&last_byte_bit_count, sizeof(last_byte_bit_count));

				if (debug) {
					std::cout << "Last byte length (in bits): " << (int)last_byte_bit_count << ".\n\n";
				}

				file_size -= sizeof(last_byte_bit_count);
				//
				// ������ file_size �������� � ���� ������, ���������������, ������������ ��� ����� ����� (���, ��� ���)
				//

				std::fstream ofile(path_to_folder + resFilename, std::fstream::out | std::fstream::binary);	// �������� ����� ��� ������

				tTree* huffman_root = makeTree(freq);				// ������ ������ ��������
				if (debug) {
					std::cout << "Huffman tree:\n";
					printTree(huffman_root);
					std::cout << "End of Huffman tree.\n\n";
				}
				tTree* p = huffman_root;							// ���������, ��������� �� ������ � ���������� ��� � �����
				
				unsigned char mask[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };
				unsigned begI = 0; 
				for (unsigned long long iii = 0; iii < file_size; iii++) {
					unsigned char c;
					file.read((char*)&c, sizeof(c));
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
						if (p->is_letter) {
							ofile.write((char*)&p->c, sizeof(p->c));
							p = huffman_root;
						}
					}
				}

				delete huffman_root;
				file.close();
				ofile.close();

				if (debug) {
					std::cout << ">>>>>DEBUG Huffman decompressing done. Check the file '.." << resFilename << "'.\n\n";
				}
			}
			else {
				if (debug) {
					std::cout << ">>>>> DEBUG ";
				}
				std::cout << "Huffman decompressing failed. Signature of file '.." << filename << "' does not match with huffman's one.\n";
			}
		}
		else {
			std::cout << "Huffman error: file '.." << filename << "' does not exist.\n";
		}
	}

	huffman_compression::tTree::tTree(unsigned char _c, unsigned int _count) :
		c(_c),
		count(_count), 
		is_letter(true),
		left(nullptr),
		right(nullptr)
	{
	}
	
	huffman_compression::tTree::tTree(tTree* _left, tTree* _right) :
		c(0),
		count(_left->count + _right->count),
		is_letter(false),
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