#ifndef __DIALOGSYSTEM_H__
#define __DIALOGSYSTEM_H__

#include "Canvas.h"

#define CHOOSE_OPTION 1

struct SDL_Texture;
class GuiInputText;

enum Option
{
	NONE,
	OPTION_A,
	OPTION_B,
	OPTION_C
};

class DialogSystem 
{
public:
	DialogSystem();
	~DialogSystem();

	bool Start();

	bool Update();

	bool CleanUp();

	void CreateScreenUI();
	void UpdateScreenUI(Option answer = NONE);


public:

	Gameobject* dialogGo;
	C_Image* retail;
	C_Text* txt;
	C_Text* txtOptionA;
	C_Text* txtOptionB;
	C_Text* txtOptionC;
	bool end;
	bool question;
	int dStep;
};
#endif