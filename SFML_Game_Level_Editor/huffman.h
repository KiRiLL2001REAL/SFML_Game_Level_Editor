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
			char c;
			unsigned int count;
			
			tTree* left, * right;
			tTree(char _c, unsigned int _count);
			tTree(tTree* _left, tTree* _right);
			~tTree();
		};

		struct myCompare {
		public:
			bool operator()(tTree* a, tTree* b) const;
		};

		void selectFileToCompress(std::string path_to_file_from_folder);
		void makeCodes(tTree* root, std::map<char, std::vector<bool> >& codes, std::vector<bool>& current_code);
		tTree* makeTree(std::map<char, unsigned int> &freq);

		//void printTree(tTree* root, unsigned int tabs = 0);

	public:
		huffman_compression(std::string path_to_folder);

		void compress(std::string path_to_file_from_folder, std::string resulting_file_extention = ".huf");

	};
}