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

	unsigned int GetID(int x, int y) const;
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

enum MapOrientation
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};

class MapContainer
{
public:

	bool Load(const char* directory, const char* file);
	bool IsValid() const;

	void Draw() const;
	void CleanUp();

	bool GetTilesetFromTileId(int id, TileSet& set) const;
	bool GetRectAndTexId(int tile_id, SDL_Rect& section, int& text_id) const;

	std::pair<int, int> I_MapToWorld(int x, int y) const;
	std::pair<int, int> I_WorldToMap(int x, int y) const;

	std::pair<float, float> F_MapToWorld(float x, float y) const;
	std::pair<float, float> F_WorldToMap(float x, float y) const;

	std::pair<int, int> WorldToTileBase(float x, float y) const;

private:

	void ParseHeader(pugi::xml_node & node);
	bool ParseTilesets(pugi::xml_node & node);
	bool ParseLayers(pugi::xml_node & node);
	void ParseObjectGroups(pugi::xml_node & node);

	// Get Triangle abc Area
	static float TriangleArea(
		const std::pair<float, float> a,
		const std::pair<float, float> b,
		const std::pair<float, float> c);

	// Check Point p is inside Triangle abc
	static bool PointInsideTriangle(
		const std::pair<float, float> p,
		const std::pair<float, float> a,
		const std::pair<float, float> b,
		const std::pair<float, float> c);

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

	float scale = 1.0f;

	// enum as string render order
	// int next_object_id

	MapOrientation type = MAPTYPE_UNKNOWN;

	std::vector<TileSet>		tilesets;
	std::vector<MapLayer>		layers;
	std::vector<MapObjectGroup>	obj_groups;
};

#endif // __MAP_H__