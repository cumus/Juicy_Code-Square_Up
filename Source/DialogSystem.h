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
	C_Image* portrait;
	C_Image* queen;
	C_Image* soldier;

	//units--------------
	C_Image* units;
	C_Image* buildings;

	//resources----------
	C_Image* resources;
	C_Image* capsule;
	C_Image* edge;
	C_Image* gear;
	C_Image* gold;

	//enemies------------
	C_Image* enemies;

	//gathedge-----------
	C_Image* gathedge;

	//text--------------
	C_Text* txt;
	C_Text* txt2;
	C_Text* txt3;
	C_Text* space;
	bool end;
	int dStep;
};
#endif