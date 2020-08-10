#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <map>

namespace huf {

	class huffman_compression {

	protected:
		std::string path_to_folder;
		std::string filename;
		std::string resFilename;

		class tTree {
		public:
			unsigned char c;
			unsigned int count;
			bool is_letter;
			
			tTree* left, * right;
			tTree(unsigned char _c, unsigned int _count);
			tTree(tTree* _left, tTree* _right);
			~tTree();
		};

		struct myCompare {
		public:
			bool operator()(tTree* a, tTree* b) const;
		};

		void selectFile(std::string path_to_file_from_folder);
		void makeCodes(
			tTree* root, 						// Корень словаря
			std::map<unsigned char,	std::string>& codes, // Ассоциативный массив символов с их кодами в виде vector<bool>
			std::string& current_code		// ССылка на пустой массив (так надо)
		);
		tTree* makeTree(std::map<unsigned char, unsigned int> &freq);

		void printTree(tTree* root, unsigned int tabs = 0);

	public:
		huffman_compression(std::string path_to_folder);

		void compress(std::string path_to_file_from_folder, std::string resulting_file_extention = ".huf", bool debug = false);
		void decompress(std::string path_to_file_from_folder, std::string resulting_file_extention = ".txt", bool debug = false);

	};
}