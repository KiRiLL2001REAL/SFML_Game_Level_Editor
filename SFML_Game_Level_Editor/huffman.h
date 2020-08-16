#pragma once

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
			
			tTree* left, * right;
			tTree(unsigned char _c, unsigned int _count);
			tTree(tTree* _left, tTree* _right);
			~tTree();
		};

		struct tListNode {
			tListNode();

			tTree* el;
			tListNode* next, * prev;
		};

		void selectFile(std::string path_to_file_from_folder);
		void makeCodes(
			tTree* root, 						// Корень словаря
			std::map<unsigned char,	std::string>& codes, // Ассоциативный массив символов с их кодами в виде vector<bool>
			std::string& current_code		// ССылка на пустой массив (так надо)
		);
		tListNode* makeTree(std::map<unsigned char, unsigned int> &freq);

	public:
		huffman_compression(std::string path_to_folder);

		bool compress(std::string path_to_file_from_folder, std::string resulting_file_extention = ".huf");
		bool decompress(std::string path_to_file_from_folder, std::string resulting_file_extention = ".txt");

	};
}