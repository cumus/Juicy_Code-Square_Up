#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Map.h"
#include "Gameobject.h"
#include "Point.h"
#include "Canvas.h"
#include "Minimap.h"


#include <vector>

#define GATHERER_COST 10
#define MELEE_COST 15
#define RANGED_COST 20
#define SUPER_COST 50
#define TOWER_COST 40
#define BARRACKS_COST 60

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

	SPAWNER_STATE,

	WIN,
	LOSE
};

enum PlayerStats : int
{
	// Stats w/ text
	CURRENT_EDGE,
	CURRENT_MOB_DROP,

	CURRENT_MELEE_UNITS,
	CURRENT_RANGED_UNITS,
	CURRENT_GATHERER_UNITS,
	CURRENT_BARRACKS,
	CURRENT_TOWERS,

	TOTAL_MELEE_UNITS,
	TOTAL_RANGED_UNITS,
	TOTAL_GATHERER_UNITS,
	TOTAL_BARRACKS,
	TOTAL_TOWERS,

	// Stats w/out text
	EDGE_COLLECTED,
	MOB_DROP_COLLECTED,
	UNITS_CREATED,
	UNITS_LOST,
	UNITS_KILLED,
	CURRENT_SPAWNERS,

	MAX_PLAYER_STATS
};

struct SDL_Texture;
class Transform;
class Sprite;

class Scene : public Module
{
public:

	Scene();
	~Scene();

	bool Start() override;
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

	Transform* SpawnBehaviour(int type, vec pos = { 0.f, 0.f, 0.f });
	
	static bool DamageAllowed();
	static bool DrawCollisions();
	static int GetStat(int stat);

private:

	void GodMode();
	void ToggleGodMode();

	void LoadTestScene();
	void LoadIntroScene();
	void LoadMenuScene();
	void LoadMainScene();
	void LoadEndScene();

	void LoadMainHUD();

	void UpdateFade();
	void UpdateStat(int stat, int count);
	void UpdateBuildingMode();
	void UpdatePause();
	void UpdateSelection();
	void UpdateSpawner();

	void UpdateStateMachine();
	void OnEventStateMachine(GameplayState state);

	void ResetScene();
	void ChangeToScene(SceneType scene);

	bool OnMainScene() const;

public:

	int id_mouse_tex;

	// Selection
	iPoint groupStart,mouseExtend;
	bool groupSelect;
	std::vector<Gameobject*> group;
	Gameobject* selection = nullptr;

	//Temporal/////
	//Enemy spawn
	std::vector<vec> spawnPoints;
	bool activateSpawn = false;
	int currentSpawns, maxSpawns;
	float spawnCounter, cooldownSpawn;
	//////////////

	//State Machine
	int tutorial_barrack = 0;
	int tutorial_tower = 0;
	int t_lvl = 1;
	bool tutorial_edge = true;

private:

	bool endScene = false;
	Gameobject root;
	Map map;

	// God Mode
	static bool god_mode;
	static bool no_damage;
	static bool draw_collisions;

	// Scene Transitions
	enum Fade : int
	{
		NO_FADE = 0,
		FADE_OUT,
		FADE_IN
	} fading = NO_FADE;
	float fade_timer;
	float fade_duration;
	bool just_triggered_change = false;
	SceneType current_scene;
	SceneType next_scene;
	GameplayState current_state;
	float scene_change_timer;

	// Place Mode
	Gameobject* imgPreview = nullptr;
	Sprite* buildingImage;
	bool placing_building = false;
	int buildType = -1;

	// Pause
	bool paused_scene = false;
	Gameobject* pause_background_go = nullptr;

	// Player
	C_Text* hud_texts[EDGE_COLLECTED];
	static int player_stats[MAX_PLAYER_STATS]; //Esta perf
	bool win = false;
	int time = 0;

	//Fog of war
	std::vector<double> cacheEnemies;

	//--------STATE MACHINE VARIABLES--------

	float distance;
	float last_distance;
	float total_distance;
	bool r_c_comprobation = true;
	int tutorial_clicks = 0;

	std::pair<float, float> last_cam_pos;
	std::pair<float, float> current_cam_pos;

	vec spawner_pos1 = { 230.f, 160.f };
	vec spawner_pos2 = { 55.f, 200.f };
	vec spawner_pos3 = { 155.f, 300.f };

	vec edge_pos1 = { 150.f, 150.f };
	vec edge_pos2 = { 221.f, 120.f };
	vec edge_pos3 = { 246.f, 79.f };
	vec edge_pos4 = { 66.f, 127.f };
	vec edge_pos5 = { 130.f, 65.f };
	vec edge_pos6 = { 123.f, 65.f };
	vec edge_pos7 = { 41.f, 166.f };
	vec edge_pos8 = { 165.f, 111.f };
	vec edge_pos9 = { 149.f, 72.f };
	vec edge_pos10 = { 143.f, 107.f };
	vec edge_pos11 = { 146.f, 108.f };
	vec edge_pos12 = { 149.f, 106.f };
	vec edge_pos13 = { 151.f, 105.f };
	vec edge_pos14 = { 154.f, 104.f };
	vec edge_pos15 = { 157.f, 102.f };

	//-------Gameobjects--------

	//Gameobject* lore_go;
	//Gameobject* cam_mov_go;
	//Gameobject* R_click_mov_go;
	Gameobject* gather_go;
	//Gameobject* edge_go;
	Gameobject* edge_t_go;
	//Gameobject* base_center_go;
	//Gameobject* barracks_state_go;
	//Gameobject* resources_state_go;
	//Gameobject* melee_go;
	Gameobject* enemy_go;
	//Gameobject* melee_atk_go;
	//Gameobject* enemy_atk_go;
	//Gameobject* mobdrop_go;
	Gameobject* build_go;
	//Gameobject* upgrade_go;
	//Gameobject* tower_state_go;
	//Gameobject* tower_atk_go;
	Gameobject* not_go;

	Gameobject* spawner_go;
	C_Image* spawn_img;
	Gameobject* spawner_text_go;
	C_Text* text_spawner;
	Gameobject* spawner_val_go;

	//------Images/Buttons------

	C_Image* cam_mov;
	//C_Image* R_click_mov;
	//C_Image* edge;
	//C_Image* base_center;
	//C_Image* barracks_state;
	//C_Image* resources;
	//C_Image* melee;
	//C_Image* enemy;
	//C_Image* melee_atk;
	//C_Image* enemy_atk;
	C_Image* mobdrop;
	//C_Image* build;
	//C_Image* upgrade;
	//C_Image* tower_state;
	//C_Image* tower_atk;
	C_Button* next;
	C_Button* skip;
	C_Button* not_inactive;
	C_Image * not;
};

#endif // __SCENE_H__