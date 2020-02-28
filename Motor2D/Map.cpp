#include "Map.h"
#include "Defs.h"
#include "Log.h"
#include <math.h>

Map::Map() : Module("map")
{}

Map::~Map()
{}

bool Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");

	maps_folder = config.child("folder").child_value();
	
	return true;
}

bool Map::CleanUp()
{
	LOG("Unloading map");

	map.CleanUp();

	while (!frontier.empty()) frontier.pop();
	while (!visited.empty()) visited.pop();

	return true;
}

bool Map::LoadFromFile(const char* file_name)
{
	map.Load(maps_folder.c_str(), file_name);
	return map.IsValid();
}

void Map::Draw() const
{
	map.Draw();
}

const MapContainer* Map::GetMap() const
{
	return &map;
}

void Map::SwapMapType()
{
	map.type = (map.type == MAPTYPE_ISOMETRIC) ? MAPTYPE_ORTHOGONAL : MAPTYPE_ISOMETRIC;
}

std::pair<int, int> Map::I_MapToWorld(int x, int y) const
{
	return map.I_MapToWorld(x, y);
}

std::pair<int, int> Map::I_WorldToMap(int x, int y) const
{
	return map.I_WorldToMap(x, y);
}

std::pair<float, float> Map::F_MapToWorld(float x, float y) const
{
	return std::pair<float, float>();
}

std::pair<float, float> Map::F_WorldToMap(float x, float y) const
{
	return std::pair<float, float>();
}

