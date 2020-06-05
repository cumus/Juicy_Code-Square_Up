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
#define SUPER_COST 40
#define TOWER_COST 30
#define BARRACKS_COST 50
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
	
	GATHER,
	WARNING,
	
	SPAWNER_STATE,

	WIN_BUTTON,
	LOSE_BUTTON,

	WIN,
	LOSE,	
};

enum UnitType : int
{
	UNKNOWN,
	GATHERER,
	UNIT_MELEE,
	UNIT_RANGED,
	UNIT_SUPER,

	ENEMY_MELEE,
	ENEMY_RANGED,
	ENEMY_SUPER,
	ENEMY_SPECIAL,

	//Structures//
	BASE_CENTER,
	TOWER,
	BARRACKS,
	LAB,
	EDGE,
	CAPSULE,
	SPAWNER,

	//Other//
	EARTHQUAKE,

	MAX_UNIT_TYPES
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

struct Mission 
{
	Mission(const char* name, PlayerStats type, int reward,int m);
	~Mission();
	void OnComplete();
	void Update(int num);
	void SetPos(RectF target,RectF txt);

	Gameobject* mission;
	C_Image* imgRetail;
	C_Text* text;
	int reward;
	int progress;
	int max;
	PlayerStats rewardType;
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
	int GetGearsCount();
	void UpdateStat(int stat, int count);

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
	void UpdateBuildingMode();
	void UpdatePause();
	void UpdateSelection();

	void UpdateStateMachine();
	void OnEventStateMachine(GameplayState state);

	void ResetScene();
	void ChangeToScene(SceneType scene);

	bool OnMainScene() const;
	inline bool SaveFileExists() const;

public:

	int id_mouse_tex;

	// Selection
	iPoint groupStart,mouseExtend;
	bool groupSelect;
	std::vector<Gameobject*> group;
	Gameobject* selection = nullptr;
	GameplayState current_state;

	//Enemy spawn
	vec spawnPoints[5];
	bool spawnPointsOccuped[5];
		

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
	C_Text* hud_texts[MAX_PLAYER_STATS];
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

	//Intro
	float introAnim;
	float introFrameTime;
	int introRow;
	int introColumn;
	C_Image* logo = nullptr;
	int max = 0;

	//MenuAnim
	float menuAnim;
	float menuFrameTime;
	int menuRow;
	int menuColumn;
	C_Image* imgMenu = nullptr;


	//Missions
	Mission* gatherEdge = nullptr;
	Mission* buildTower = nullptr;
	Mission* buildBarracks = nullptr;

	//--------STATE MACHINE VARIABLES--------
		
	Gameobject* not_go;

	//------Images/Buttons------
	C_Button* next;
	C_Button* skip;
	C_Button* not_inactive;
	C_Image * not;

	bool first_time_pause_button;
	bool paused_yet = false;
	bool endScene = false;
};

#endif // __SCENE_H__