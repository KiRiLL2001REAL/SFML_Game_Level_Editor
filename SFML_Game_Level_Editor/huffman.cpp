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

		list_first = new tListNode();						// ������������� ������, ���������� ������ ������
		list_first->el = new tTree(it->first, it->second);
		list_last = list_first;

		it++;
		list_count_elements++;
		while (it != freq.end()) {	// ���������� ������
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
			
			����� ���� ��������� ���� tListNode;

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

			list_count_elements--;		// ������ �������� "�����������" 1 ������� ������, � ������ ����������.

			tTree* m[2];	// ��������� tTree ��������, 
			for (char i = 0; i < 2; i++) m[i] = min[i]->el;

			/*
			
			�������� ������� �������� �� ������
			
			*/
			p = min[0]->prev;
			if (min[0]->prev) {			// ���� min[0] �� �������� ������� ������
				p->next = min[0]->next;			// ������������ "next" ��������� �������� "p" ����� "min[0]" � "min[0]->next"
			}
			else {
				list_first = list_first->next;	// ����� ������������ list_first �� ������� �����, ����� �� ��������� ���������
			}
			if (min[0]->next) {			// ���� min[0] �� �������� ������ ������
				min[0]->next->prev = p;			// ������������ "prev" ��������� �������� "min[0]->next" ����� "min[0]" � "p"
			}
			else {
				list_last = list_last->prev;	// ����� ������������ list_last �� ������� �����, ����� �� ��������� ���������
			}
			delete min[0];

			/*
			
			�������� ������ ���� tTree � ��������� ��� �� ������ �������
			
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

			unsigned long long file_size;	// �������� ������ ����� � ������
			file.seekg(0, std::fstream::end);
			file_size = file.tellg();
			file.clear(); file.seekg(0);

			/*
			
			����ר� ������� ��������� ��� ������� �������
			
			*/

			std::map<unsigned char, unsigned int> freq;	// ������������� ������ ��� �������� ���-�� ��������� ������� �� ��������

			for (unsigned long long iii = 0; iii < file_size; iii++) {	// ������� ��������� ��������
				unsigned char c;
				file.read((char*)&c, sizeof(c));
				freq[c]++;
			}
			unsigned int dictionary_size = freq.size();;	// ���-�� �������� � �������

			/*

			���������� �����

			*/

			tListNode* ln = makeTree(freq);
			tTree* huffman_root = ln->el;					// ������ ������ ��������
			delete ln;
			std::map<unsigned char, std::string> codes;	// ������������� ������ ��� �������� �������� � ����� � ��� � ���� string
			std::string temp_str = {};	// ������ ����
			makeCodes(huffman_root, codes, temp_str);	// ��������� ����� � ���� string
			delete huffman_root;									// ���� �������������, �������� ������ ������ �� ���������
			
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

			ofile.write((char*)&bit_count, sizeof(bit_count));	// ���������� � �������������� ���� ���-�� �������� ��� � ��������� �����
			
			bit_count = 0;	// ������ ����� ����� ���-�� ���, ��������� � byte

			unsigned char c;
			std::string byte = "";
			std::vector<unsigned char> data_buffer = {};
			unsigned int lalala = 0;

			for (unsigned long long iii = 0; iii < file_size; iii++) {	// ������ ����	
				file.read((char*)&c, sizeof(c));
				byte += codes[c];
				bit_count += (unsigned char)codes[c].size();
				while (bit_count >= 8) {	// ���� ������� ���� �� 8 ���, �� ���������� �� (8) � ����
					std::string tmp = "        ";	// � 'a' ��������� ������ 8 ��� ����
					for (int i = 0; i < 8; i++) { tmp[i] = byte[i]; }
					unsigned char a = 0;
					for (unsigned char i = 0; i < 8; i++) {
						a = a << 1;
						a |= tmp[i] == '1';
					}

					if (lalala % 2 == 0) {	// ������� �������
						a = ~a;
					}
					lalala++;

					data_buffer.push_back(a); //ofile.write((char*)&a, sizeof(a));
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

				if (lalala % 2 == 0) {	// ������� �������
					a = ~a;
				}
				lalala++;

				data_buffer.push_back(a); //ofile.write((char*)&a, sizeof(a));
			}

			for (auto& it : data_buffer) { // ����� data_buffer � ����
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

				unsigned char last_byte_bit_count;
				file.read((char*)&last_byte_bit_count, sizeof(last_byte_bit_count));

				file_size -= sizeof(last_byte_bit_count);
				//
				// ������ file_size �������� � ���� ������, ���������������, ������������ ��� ����� ����� (���, ��� ���)
				//

				std::fstream ofile(path_to_folder + resFilename, std::fstream::out | std::fstream::binary);	// �������� ����� ��� ������

				tListNode* ln = makeTree(freq);
				tTree* huffman_root = ln->el;				// ������ ������ ��������
				delete ln;
				
				tTree* p = huffman_root;							// ���������, ��������� �� ������ � ���������� ��� � �����
				
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