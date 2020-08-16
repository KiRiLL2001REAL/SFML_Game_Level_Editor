#pragma once

#include "editor.h"

const unsigned char code_about_paragraph = 100;

class myDesktop : public edt::tDesktop {
protected:
	enum class screen_codes { Menu, MapEditor, SPREditor, NPCEditor };

public:
	myDesktop(std::string path_to_folder) : edt::tDesktop(path_to_folder) {};
	~myDesktop() { std::cout << "~myDesktop done.\n"; };

	virtual void changeScreen(char new_screen_code);
	virtual void handleEvent(edt::tEvent& e);

	virtual sf::FloatRect getLocalBounds();
	virtual bool pointIsInsideMe(sf::Vector2i point);
};