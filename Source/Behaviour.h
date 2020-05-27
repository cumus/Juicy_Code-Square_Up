#ifndef __BEHAVIOUR_H__
#define __BEHAVIOUR_H__

#include "Component.h"
#include "Point.h"
#include "Canvas.h"
#include "Scene.h"
#include "Audio.h"
#include "Collider.h"

#include <vector>
#include <list>
#include <map>

#define RAYCAST_TIME 0.1
#define CREATION_TIME 1.0

enum UnitType : int
{
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
	WALL,
	BARRACKS,
	LAB,
	EDGE,
	CAPSULE,
	SPAWNER,
	
	MAX_UNIT_TYPES
};

enum UnitState : int
{
	IDLE,
	MOVING,
	MOVING_N,
	MOVING_S,
	MOVING_W,
	MOVING_E,
	MOVING_NE,
	MOVING_NW,
	MOVING_SE,
	MOVING_SW,
	ATTACKING,
	ATTACKING_N,
	ATTACKING_S,
	ATTACKING_W,
	ATTACKING_E,
	ATTACKING_NE,
	ATTACKING_NW,
	ATTACKING_SE,
	ATTACKING_SW,
	CHASING,
	BASE,

	// Building
	BUILDING,
	NO_UPGRADE,
	DESTROYED,
	FIRST_UPGRADE,
	SECOND_UPGRADE,

	// Capsule
	FLY,
	LANDING_1,
	LANDING_2,
	POSE,
	OPEN,

	// Mob_Drop
	STAY
};



class Sprite;
class AnimatedSprite;
class AudioSource;


class Behaviour : public Component
{
public:
	Behaviour(Gameobject* go, UnitType type, UnitState starting_state, ComponentType comp_type = BEHAVIOUR);
	virtual ~Behaviour();

	void RecieveEvent(const Event& e) override;
	void PreUpdate() override;

	void Load(pugi::xml_node& node) override;
	void Save(pugi::xml_node& node) const override;

	void Selected();
	void UnSelected();
	void OnDamage(int damage);
	void OnKill(const UnitType type);
	void ActivateSprites();
	void DesactivateSprites();
	void CheckFoWMap(bool debug=false);
	bool IsDestroyed();
	vec GetPos();
	void GetTilesInsideRadius();
	Collider* GetBodyCollider();
	Collider* GetSelectionCollider();
	RectF GetSelectionRect();
	virtual void UpdatePath(int x,int y) {}
	virtual void AfterDamageAction() {}
	virtual void OnRightClick(vec pos, vec modPos) {}
	virtual void DoAttack() {}
	virtual void OnDestroy(){}
	virtual void create_bar() {}
	virtual void update_health_ui() {}
	virtual void CreatePanel() {}
	virtual void UpdatePanel() {}
	virtual void create_creation_bar() {}
	virtual void update_creation_bar() {}
	virtual void Upgrade() {}
	virtual void UpdateWalkabilityTiles() {}
	virtual void FreeWalkabilityTiles() {}
	virtual void Repath() {};
	virtual void OnCollision(Collider selfCol, Collider col) {}
	static bool IsHidden(double id) { return b_map[id]->visible; }
	void SetColliders();


	UnitType GetType() const { return type; }
	UnitState* GetStatePtr() { return &current_state; }
	UnitState GetState() { return current_state; }
	UnitState GetSpriteState() { return spriteState; }
	UnitState* GetSpriteStatePtr() { return &spriteState; }

	//void QuickSort();
	unsigned int GetBehavioursInRange(vec pos, float dist, std::map<float, Behaviour*>& res) const;

protected:

	virtual void AddUnitToQueue(UnitType type, vec pos = vec(), float time = -1) {}

public: 
	
	static std::map<double, Behaviour*> b_map;
	UnitState current_state, new_state;
	UnitState spriteState;
	vec pos;
	int max_life, current_life, damage, current_lvl = 0, max_lvl = 0;
	std::vector<iPoint> tilesVisited;
	AnimatedSprite* characteR = nullptr;
	bool providesVisibility,visible;
	Collider* bodyColl = nullptr;
	Collider* visionColl = nullptr;
	Collider* attackColl = nullptr;
	Collider* selColl = nullptr;
	RectF selectionRect;
	std::pair<int,int> selectionOffset;
	std::pair<float, float> baseCollOffset;
	std::pair<float, float> visionCollOffset;
	std::pair<float, float>	attackCollOffset;

protected:	

	// Stats
	UnitType type;
	float attack_range, vision_range,dieDelay;
	float rayCastTimer;
	bool shoot,drawRanges;
	Audio_FX deathFX;
	std::pair<float, float> visionRange;
	std::pair<float, float> atkRange;
	Behaviour* objective;
	std::vector<iPoint> lastFog;

	// Complementary components
	AudioSource* audio;
	Sprite* selection_highlight;

	// Mini Life Bars
	struct Lifebar
	{
		void Create(Gameobject* parent);
		void Show();
		void Hide();
		void Update(float life, int lvl);

		Gameobject* go;
		Sprite* green_bar;
		Sprite* upgrades;
		vec offset;
		SDL_Rect life_starting_section;
		SDL_Rect upgrades_starting_section;
	} mini_life_bar;

	//Bulding Units Components
	float pos_y_HUD;
	int bar_text_id;
	Gameobject* selectionPanel = nullptr;
	Gameobject* btnTower = nullptr;
	Gameobject* btnBarrack = nullptr;
	Gameobject* btnBaseCenter = nullptr;
	Gameobject* bar_go = nullptr;
	Gameobject* creation_bar_go = nullptr;
	C_Image* bar;
	C_Image* portrait;
	C_Text* text;
	C_Image* healthbar;
	C_Image* red_health;
	C_Image* green_health;
	C_Image* health_boarder;
	C_Image* upgrades;
};

class BuildingWithQueue : public Behaviour
{
public:

	BuildingWithQueue(Gameobject* go, UnitType type, UnitState starting_state, ComponentType comp_type = BEHAVIOUR);

	void Update() override;
	void AddUnitToQueue(UnitType type, vec pos = vec(), float time = -1) override;

protected:

	struct QueuedUnit
	{
		QueuedUnit(UnitType type = MAX_UNIT_TYPES, Gameobject* go = nullptr, vec pos = vec(), float time = -1);
		QueuedUnit(const QueuedUnit& copy);
		float Update();

		UnitType type;
		vec pos;
		float time;
		float current_time;
		Transform* transform;
	};

	vec spawnPoint;
	Sprite* icon;
	Sprite* progress_bar;
	static SDL_Rect bar_section;
	std::list<QueuedUnit> build_queue;
};

class B_Unit : public Behaviour
{
public:

	B_Unit(Gameobject* go, UnitType type, UnitState starting_state, ComponentType comp_type = B_MELEE_UNIT);

	void Update() override;
	void OnRightClick(vec pos, vec modPos) override;
	void DoAttack() override;
	void OnDestroy() override;
	void UpdatePath(int x, int y) override;
	void CheckPathTiles();
	void ChangeState();
	void CheckDirection(fPoint actualPos);
	void ShootRaycast();
	void DrawRanges();
	void Repath() override;
	virtual void UnitAttackType() {}
	void OnCollision(Collider selfCol, Collider col) override;

protected:
	float speed;
	int damage;
	float atkTime, atkTimer;
	Audio_FX attackFX;
	vec attackPos;
	iPoint movDest;
	std::vector<iPoint>* path;
	std::pair<int, int> destPos;
	iPoint nextTile;
	bool next;
	bool move;
	bool gotTile;
	bool positiveX;
	bool positiveY;
	int dirX;
	int dirY;
	bool goingBase;
	bool arriveDestination;
	bool calculating_path;
	bool chasing;
	std::pair<float, float> atkObjPos;
	std::pair<float, float> shootPos;
	Behaviour* atkObj;
	Behaviour* chaseObj;
	bool moveOrder;
};

#endif // __BEHAVIOUR_H_