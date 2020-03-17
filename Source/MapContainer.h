#ifndef __MAP_CONTAINER_H__
#define __MAP_CONTAINER_H__

#include "SDL/include/SDL_rect.h"
#include "PugiXml/src/pugixml.hpp"

#include <string>
#include <vector>

class TileSet
{
public:

	TileSet();
	TileSet(const TileSet& copy);
	~TileSet();

	//bool ParseTerrainTypes(pugi::xml_node tileset_terraintypes);
	//bool ParseData(pugi::xml_node tileset_tiles);

	SDL_Rect GetTileRect(int id) const;

public:

	std::string	name;
	int	firstgid = 0;
	int	tile_width = 0;
	int	tile_height = 0;

	// Sections
	int	spacing = 0;
	int	margin = 0;
	int	tilecount = 0;
	//int	columns = 0;

	int	offset_x = 0;
	int	offset_y = 0;

	// Texture
	int texture_id = -1;
	int	num_tiles_width = 0;
	int	num_tiles_height = 0;

	//std::vector<std::pair<std::string, int>> terrain_types;
	//std::vector<std::pair<int, SDL_Rect>> data;
};

class MapLayer
{
public:

	MapLayer();
	MapLayer(const MapLayer& copy);
	~MapLayer();

	bool ParseProperties(pugi::xml_node layer_properties);
	bool ParseData(pugi::xml_node layer_data);

	int GetID(int x, int y) const;
	float GetProperty(const char* name, float default_value = 0) const;

public:

	std::string	name;
	int	width, height;
	bool drawable;

private:

	std::vector<std::pair<std::string, float>> properties;
	std::vector<unsigned int> data;
};

struct MapObject
{
	MapObject();
	MapObject(const MapObject& copy);
	~MapObject();

	unsigned int id = 0;
	float x = 0;
	float y = 0;
	float width = 0;
	float height = 0;
};

struct MapObjectGroup
{
	MapObjectGroup();
	MapObjectGroup(const MapObjectGroup& copy);
	~MapObjectGroup();

	std::string name;
	//SDL_Color color;
	std::vector<MapObject> objects;
};

#endif // __MAP_CONTAINER_H__