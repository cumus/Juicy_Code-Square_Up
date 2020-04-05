#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "Component.h"
#include "Render.h"
#include "SDL/include/SDL_rect.h"
#include "SDL/include/SDL_pixels.h"

class Sprite : public Component
{
public:

	Sprite(Gameobject* game_object);
	~Sprite();

	void SetLayer(Layer layer);
	void PostUpdate() override;

public:

	int tex_id = -1;
	SDL_Rect section;
	SDL_Color color;
	Layer spriteLayer = SCENE;
};

#endif // __SPRITE_H__