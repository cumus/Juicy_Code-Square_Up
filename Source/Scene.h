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

enum GameplayState : int
{
	LORE,
	CAM_MOVEMENT,
	R_CLICK_MOVEMENT,

	EDGE_STATE,
	RESOURCES,
	MOBDROP,

	BUILD,
	UPGRADE,

	GATHERER_STATE,
	MELEE,
	MELEE_ATK,
	ENEMY,
	ENEMY_ATK,

	TOWER_STATE,
	TOWER_ATK,
	BASE_CENTER_STATE,
	BARRACKS_STATE,

	WIN,
	LOSE
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
	Gameobject* AddGameobjectToCanvas(const char* name);

	void SetSelection(Gameobject* go = nullptr, bool call_unselect = true);
	Gameobject* MouseClickSelect(int mouse_x, int mouse_y);

	void SpawnMeleeIA(float x, float y);
	void SpawnRangedIA(float x, float y);
	void SpawnSuperIA(float x, float y);
	void SpawnSpecialIA(float x, float y);

private:

	void GodMode();

	bool LoadTestScene();
	bool LoadIntroScene();
	bool LoadMenuScene();
	bool LoadMainScene();
	bool LoadEndScene();

	void UpdateFade();
	void UpdateHUD();
	void UpdateBuildingMode();
	void UpdatePause();
	void UpdateSelection();
	void UpdateSpawner();

	void UpdateStateMachine();
	void OnEventStateMachine(GameplayState state);

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
	//Gameobject* pause_canvas_go;

	// Selection
	iPoint groupStart,mouseExtend;
	bool groupSelect;
	std::vector<Gameobject*> group;
	Gameobject* selection = nullptr;

	// HUD
	Gameobject* hud_canvas_go;
	Gameobject* pause_background_go;
	int building_bars_created = 0;
	int current_melee_units = 0;
	int melee_units_created = 0;
	int current_ranged_units = 0;
	int ranged_units_created = 0;
	int current_gatherer_units = 0;
	int gatherer_units_created = 0;


	//IA
	std::pair<int, int> baseCenterPos;

	//Temporal/////
	//Enemy spawn
	std::vector<vec> spawnPoints;
	bool activateSpawn = false;
	int currentSpawns, maxSpawns;
	float spawnCounter, cooldownSpawn;
	//////////////

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
	GameplayState current_state;



	//--------STATE MACHINE VARIABLES--------

	float distance;
	float last_distance;
	float total_distance;
	bool r_c_comprobation = true;
	int tutorial_clicks = 0;

	std::pair<float, float> last_cam_pos;
	std::pair<float, float> current_cam_pos;

	//-------Gameobjects--------

	Gameobject* lore_go;
	Gameobject* cam_mov_go;
	Gameobject* R_click_mov_go;
	Gameobject* gather_go;
	Gameobject* edge_go;
	Gameobject* edge_t_go;
	Gameobject* base_center_go;
	Gameobject* barracks_state_go;
	Gameobject* resources_state_go;
	Gameobject* melee_go;
	Gameobject* enemy_go;
	Gameobject* melee_atk_go;
	Gameobject* enemy_atk_go;
	Gameobject* mobdrop_go;
	Gameobject* build_go;
	Gameobject* upgrade_go;
	Gameobject* tower_state_go;
	Gameobject* tower_atk_go;

	//------Images/Buttons------

	C_Image* cam_mov;
	C_Image* R_click_mov;
	C_Image* edge;
	C_Image* base_center;
	C_Image* barracks_state;
	C_Image* resources;
	C_Image* melee;
	C_Image* enemy;
	C_Image* melee_atk;
	C_Image* enemy_atk;
	C_Image* mobdrop;
	C_Image* build;
	C_Image* upgrade;
	C_Image* tower_state;
	C_Image* tower_atk;
	C_Button* next;

	//States next_state;

	/*bool shoot = false;//temp
	std::pair<int, int> pos;
	std::pair<int, int> atkPos;
	float rayCastTimer = 0;*/

	// Player stats
	Gameobject* resources_go;
	Gameobject* resources_2_go;
	
	C_Text* text_mobdrop_value = nullptr;
	C_Text* text_edge_value = nullptr;
	C_Text* text_current_melee_units = nullptr;
	C_Text* text_melee_units_created = nullptr;
	C_Text* text_current_ranged_units = nullptr;
	C_Text* text_ranged_units_created = nullptr;
	C_Text* text_current_gatherer_units = nullptr;
	C_Text* text_gatherer_units_created = nullptr;

	Transform* placing_building = nullptr;
	
};

#endif // __SCENE_H__