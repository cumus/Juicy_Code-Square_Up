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
#define MAX_GATHERER_LVL 5
#define MAX_MELEE_LVL 5
#define MAX_RANGED_LVL 5
#define MAX_SUPER_LVL 5
#define GATHERER_UPGRADE_COST 35
#define MELEE_UPGRADE_COST 40
#define RANGED_UPGRADE_COST 50
#define SUPER_UPGRADE_COST 60

enum SceneType : int
{
	EMPTY,
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
	LOSE,

	MOBDROP
};

enum PlayerStats : int
{
	// Stats w/ text
	CURRENT_EDGE,
	CURRENT_MOB_DROP,
	CURRENT_GOLD,

	CURRENT_MELEE_UNITS,
	CURRENT_RANGED_UNITS,
	CURRENT_GATHERER_UNITS,
	CUERRENT_SUPER_UNITS,
	CURRENT_BARRACKS,
	CURRENT_TOWERS,
	CURRENT_SPAWNERS,

	TOTAL_MELEE_UNITS,
	TOTAL_RANGED_UNITS,
	TOTAL_SUPER_UNITS,
	TOTAL_GATHERER_UNITS,
	TOTAL_BARRACKS,
	TOTAL_TOWERS,

	// Stats w/out text
	EDGE_COLLECTED,
	MOB_DROP_COLLECTED,
	GOLD_COLLECTED,
	UNITS_CREATED,
	UNITS_LOST,
	UNITS_KILLED,

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

	Transform* SpawnBehaviour(int type, vec pos = { 0.f, 0.f, 0.f });
	
	static bool DamageAllowed();
	static bool DrawCollisions();
	static int GetStat(int stat);

	// Scene Serialization
	void SaveGameNow();
	void LoadGameNow();

private:

	void GodMode();
	void ToggleGodMode();

	void LoadIntroScene();
	void LoadMenuScene();
	void LoadMainScene();
	void LoadOptionsScene();
	void LoadEndScene();

	void LoadMainHUD();
	void LoadTutorial();
	void LoadBaseCenter();
	void LoadStartingMapResources();

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

	Gameobject root;
	Map map;

	// God Mode
	static bool god_mode;
	static bool no_damage;
	static bool draw_collisions;
	bool drawSelection = false;

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
	static int player_stats[MAX_PLAYER_STATS];
	bool win = false;
	//int time = 0;

	//Earthquake
	int timeEarthquake = 0;
	bool earthquake = false;
	float shakeTimer = 0;
	float earthquakeTimer = 0;

	//Unit upgrades
	int gathererLvl = 0;
	int meleeLvl = 0;
	int rangedLvl = 0;
	int superLvl = 0;


	//--------STATE MACHINE VARIABLES--------

	float distance;
	float last_distance;
	float total_distance;
	bool r_c_comprobation = true;
	int tutorial_clicks = 0;

	std::pair<float, float> last_cam_pos;
	std::pair<float, float> current_cam_pos;

	//-------Gameobjects--------
	Gameobject* edge_t_go;
	Gameobject* not_go;

	//------Images/Buttons------
	C_Button* next;
	C_Button* skip;
	C_Button* not_inactive;
	C_Image * not;

	bool first_time_pause_button;
	bool paused_yet = false;
	bool endScene = false;

	// Music and Sounds Value
	static int music_value;
	static int sfx_value;
};

#endif // __SCENE_H__