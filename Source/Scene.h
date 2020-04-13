#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Map.h"
#include "Gameobject.h"
#include "Point.h"
#include "Canvas.h"
#include "Minimap.h"

#include <vector>

enum SceneType : int
{
	EMPTY,
	TEST,
	INTRO,
	MENU,
	MAIN,
	MAIN_FROM_SAFE,
	END,
	CREDITS
};

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();
	~Scene();

	bool PreUpdate() override;
	bool Update() override;
	bool PostUpdate() override;
	bool CleanUp() override;

	void RecieveEvent(const Event& e) override;

	Gameobject* GetRoot();
	const Gameobject* GetRoot() const;
	Gameobject* AddGameobject(const char* name, Gameobject* parent = nullptr);
	Gameobject* MouseClickSelect(int mouse_x, int mouse_y);

private:

	void GodMode();

	bool LoadTestScene();
	bool LoadIntroScene();
	bool LoadMenuScene();
	bool LoadMainScene();
	bool LoadEndScene();

	bool PauseMenu();
	//bool DestroyPauseMenu();

	bool ChangeToScene(SceneType scene);

	void PlaceMode(int building_type);

public:

	int id_mouse_tex;

	bool pause = false;
	bool test = true;
	bool level = true;

	// PAUSE
	Gameobject* pause_canvas_go;

	// Selection
	iPoint groupStart,mouseExtend;
	bool groupSelect;
	std::vector<Gameobject*> group;
	
	// HUD
	Gameobject* hud_canvas_go;
	int building_bars_created = 0;

	//IA
	std::pair<int, int> baseCenterPos;

	//Player Resources
	int mob_drop = 0;
	int edge_value = 100;

	// END SCREEN
	bool win = false;
	int time = 0;
	int edge_collected = 0;
	int units_created = 0;
	int	units_lost = 0;
	int	units_killed = 0;

private:

	Map map;
	Minimap* minimap;
	Gameobject root;
	bool god_mode = false;

	// Scene Transitions
	enum Fade : int
	{
		NO_FADE = 0,
		FADE_OUT,
		FADE_IN
	} fading = NO_FADE;
	float fade_timer;
	float fade_duration;
	SceneType current_scene;
	SceneType next_scene;

	/*bool shoot = false;//temp
	std::pair<int, int> pos;
	std::pair<int, int> atkPos;
	float rayCastTimer = 0;*/

	// Player stats
	Gameobject* resources_go;
	Gameobject* resources_2_go;
	
	C_Text* text_mobdrop_value = nullptr;
	C_Text* text_edge_value = nullptr;

	Transform* placing_building = nullptr;
	
};

#endif // __SCENE_H__