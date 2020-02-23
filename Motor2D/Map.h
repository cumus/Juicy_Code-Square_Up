#ifndef __MAP_H__
#define __MAP_H__

#include "Module.h"
#include "Collisions.h"
#include "Point.h"
#include "PugiXml/src/pugixml.hpp"

#include <string>
#include <list>
#include <queue>
#include <vector>

#define MAX_MAP_COLLIDERS 100

struct SDL_Texture;

struct Properties
{
	~Properties();

	int Get(const char* name, int default_value = 0) const;

	struct Property
	{
		std::string name;
		int value;
	};

	std::list<Property*>	list;
};

// ----------------------------------------------------
struct MapLayer
{
	~MapLayer();

	inline unsigned int Get(int x, int y) const;

	std::string		name;
	int				width;
	int				height;
	Properties		properties;

	std::vector<unsigned int>	data;
};

struct MapObject
{
	std::string name;
	Collider* col[MAX_MAP_COLLIDERS] = { nullptr };
	unsigned int id = 0u;
};

// ----------------------------------------------------
struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	std::string			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;
	SDL_Color			background_color;
	MapTypes			type;

	std::list<TileSet*>		tilesets;
	std::list<MapLayer*>	layers;
	std::list<MapObject*>	objects;
};

// ----------------------------------------------------
class Map : public Module
{
public:

	Map();

	// Destructor
	~Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf) override;

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp() override;

	// Load new map
	bool Load(const char* path);

	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);
	bool LoadObjects(pugi::xml_node& node, MapObject* object);

	TileSet* GetTilesetFromTileId(int id) const;

public:

	MapData data;

private:

	pugi::xml_document	map_file;
	std::string			folder;
	bool				map_loaded;

	/// BFS
	std::queue<iPoint>	frontier;
	std::queue<iPoint>	visited;
};

#endif // __MAP_H__