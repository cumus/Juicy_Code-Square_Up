#ifndef __MAP_H__
#define __MAP_H__

#include "MapContainer.h"
#include "Vector3.h"
#include <list>
#include <string>

enum MapOrientation
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};

//struct Properties
//{
//	struct Property
//	{
//		std::string* name;
//		int value;
//	};
//
//	~Properties()
//	{
//		std::list<Property*>* item;
//		item = list.start;
//
//		while (item != NULL)
//		{
//			DEL(item->data);
//			item = item->next;
//		}
//
//		list.clear();
//	}
//
//	int Get(const char* name, int default_value = 0) const;
//
//	p2List<Property*>	list;
//};

struct MapData
{
	int						width;
	int						height;
	int						tile_width;
	int						tile_height;
	SDL_Color				background_color;
	MapOrientation			type;
	std::vector<TileSet*>	tilesets;
	std::vector<MapLayer*>	layers;
};

class Map
{
public:

	Map();
	~Map();

	bool Load(const char* file);
	void CleanUp();

	bool IsValid() const;
	void Draw() const;

	static Map* GetMap();
	static const Map* GetMapC();

	static void SwapMapType();
	static void SetMapScale(float scale);

	// Map Data Getters
	TileSet* GetTilesetFromTileId(int id) const;
	bool GetRectAndTexId(int tile_id, SDL_Rect& section, int& text_id) const;
	const MapLayer& GetMapWalkabilityLayer();

	static void GetTileSize_I(int& width, int& height);
	static void GetTileSize_F(float& width, float& height);

	// Coordinate conversions
	static std::pair<int, int> I_MapToWorld(int x, int y, int z = 0);
	static std::pair<int, int> I_WorldToMap(int x, int y);
	static std::pair<float, float> F_MapToWorld(float x, float y, float z = 0.0f);
	static std::pair<float, float> F_MapToWorld(vec vec);
	static std::pair<float, float> F_WorldToMap(float x, float y);
	static std::pair<int, int> WorldToTileBase(float x, float y);

	//Debug Draw
	bool draw_walkability = false;

private:

	void ParseHeader(pugi::xml_node& node);
	bool ParseTilesets(pugi::xml_node& node);
	bool ParseLayers(pugi::xml_node& node);
	void ParseObjectGroups(pugi::xml_node& node);

private:

	// File info
	bool loaded = false;
	std::string	path = "undefined";

	// Static fields
	static Map* map;
	static MapOrientation type;

	static float	scale;
	static int		width;
	static int		height;
	static int		tile_width;
	static int		tile_height;

	static std::pair<int, int>		size_i;
	static std::pair<float, float>	size_f;

	// Other tiled fields
	// SDL_Color background_color;
	// enum as string render order
	// int next_object_id

	// Map Data
	std::vector<TileSet>		tilesets;
	std::vector<MapLayer>		layers;
	std::vector<MapObjectGroup>	obj_groups;

public:

	MapData data;
};

#endif // __MAP_H__