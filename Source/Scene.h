#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Map.h"
#include "Gameobject.h"
#include "Point.h"

#include <vector>

enum SceneType : int
{
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

	bool LoadTestScene();
	bool LoadIntroScene();
	bool LoadMenuScene();
	bool LoadMainScene();
	bool ChangeToScene(SceneType scene);

	Gameobject* GetRoot();
	const Gameobject* GetRoot() const;
	Gameobject* AddGameobject(const char* name, Gameobject* parent = nullptr);
	Gameobject* MouseClickSelect(int mouse_x, int mouse_y);

private:

	void GodMode();

public:

	int id_mouse_tex;

	// Selection
	iPoint groupStart,mouseExtend;
	bool groupSelect;
	std::vector<Gameobject*> group;
	
	// HUD
	Gameobject* hud_canvas_go;
	int building_bars_created = 0;
	int unit_bars_created = 0;

private:

	Map map;
	Gameobject root;

	enum SceneState : int
	{
		SCENE_STOPPED = 0,
		SCENE_PLAYING,
		SCENE_PAUSED,
		SCENE_FADE_OUT,
		SCENE_FADE_IN
	} state = SCENE_STOPPED;
	bool god_mode = true;

	SceneType current_scene;
	SceneType next_scene;
	int resources = 100;
};

#endif // __SCENE_H__