#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "Component.h"
#include "SDL/include/SDL_rect.h"

class Sprite : public Component
{
public:

	Sprite(Gameobject* game_object);
	~Sprite();

	void PostUpdate() override;

public:

	int tex_id = -1;
	SDL_Rect section;
	int r, g, b, a;
};

#endif // __SPRITE_H__