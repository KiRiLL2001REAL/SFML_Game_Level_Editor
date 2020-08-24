#pragma once

#include "editor.h"

const unsigned char code_about_paragraph = 100;

class myDesktop : public edt::tDesktop {
protected:
	enum class screen_codes { Menu, MapEditor, SPREditor, NPCEditor };

public:
	myDesktop(std::string path_to_folder) : edt::tDesktop(path_to_folder) {};
	virtual ~myDesktop();

	virtual void changeScreen(char new_screen_code);
	virtual void handleEvent(edt::tEvent& e);
};