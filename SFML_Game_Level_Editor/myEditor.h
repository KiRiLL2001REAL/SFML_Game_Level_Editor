#pragma once

#include "editor.h"

static const struct sScreenCodes {	// Коды кнопок
	static const unsigned char about_program = 100;
	static const unsigned char Menu = 101;
	static const unsigned char MapEditor = 102;
	static const unsigned char SprEditor = 103;
	static const unsigned char NpcEditor = 104;
	static const unsigned char Settings = 105;
} button_codes;

class myDesktop : public edt::tDesktop {
public:
	myDesktop(std::string path_to_folder, unsigned char screen_code) : edt::tDesktop(path_to_folder, screen_code) {};
	virtual ~myDesktop();

	virtual void changeScreen(char new_screen_code);
	virtual void handleEvent(edt::tEvent& e);
};