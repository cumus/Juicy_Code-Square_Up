#ifndef __MAP_CONTAINER_H__
#define __MAP_CONTAINER_H__

#include "SDL/include/SDL_rect.h"
#include "PugiXml/src/pugixml.hpp"

#include <string>
#include <vector>

struct SDL_Texture;

struct TileSet
{
	SDL_Rect& GetTileRect(int id) const;

	int				firstgid;
	std::string		name;
	int				tile_width;
	int				tile_height;
	int				spacing;
	int				margin;
	int				tilecount;
	int				columns;

	int				offset_x;
	int				offset_y;

	int				tex_width;
	int				tex_height;
	int				num_tiles_width;
	int				num_tiles_height;
	SDL_Texture*	texture;
};

class MapLayer
{
public:

	~MapLayer();

	inline unsigned int GetID(int x, int y) const;
	int GetProperty(const char* name, int default_value = 0) const;

public:

	std::string	name = "undefined_layer";
	int			width = 0;
	int			height = 0;

	std::vector<unsigned int> data;
	std::vector<std::pair<const char*, int>> properties;
};

struct MapObjectGroup
{
	std::string name;
	//SDL_Color color;

	struct MapObject
	{
		int id;
		float x;
		float y;
		float width;
		float height;
	};
	std::vector<MapObject> objects;
};

class MapContainer
{
public:

	void Draw() const;
	void CleanUp();
	bool Load(const char* directory, const char* file);
	bool IsValid() const;
	void LogMapDetails() const;

	bool GetTilesetFromTileId(int id, TileSet& set) const;
	
	std::pair<float,float> MapContainer::MapToWorld(float x, float y) const;
	std::pair<float,float> MapContainer::WorldToMap(float x, float y) const;

private:

	void ParseHeader(pugi::xml_node & node);
	bool ParseTilesets(pugi::xml_node & node);
	bool ParseLayers(pugi::xml_node & node);
	void ParseObjectGroups(pugi::xml_node & node);

public:

	bool				loaded = false;

	pugi::xml_document	doc;
	std::string			dir = "undefined";
	std::string			file_name = "undefined";

	int					width = -1;
	int					height = -1;
	int					tile_width = -1;
	int					tile_height = -1;
	//SDL_Color			background_color;

	// enum as string render order
	// int next_object_id

	enum MapOrientation
	{
		MAPTYPE_UNKNOWN = 0,
		MAPTYPE_ORTHOGONAL,
		MAPTYPE_ISOMETRIC,
		MAPTYPE_STAGGERED
	} type = MAPTYPE_UNKNOWN;

	std::vector<TileSet>		tilesets;
	std::vector<MapLayer>		layers;
	std::vector<MapObjectGroup>	obj_groups;
};

#endif // __MAP_H__