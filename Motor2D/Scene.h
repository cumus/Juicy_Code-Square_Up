#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Gameobject.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();
	~Scene();

	bool Awake(pugi::xml_node& config) override;
	bool Start() override;
	bool PreUpdate() override;
	bool Update() override;
	bool PostUpdate() override;
	bool CleanUp() override;

	bool LoadTestScene();

	Gameobject* AddGameobject(const char* name, Gameobject* parent = nullptr);

public:

	int id_mouse_tex;

private:

	Gameobject root;

	Gameobject* go1;
	Gameobject* go2;
};

#endif // __SCENE_H__