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
	map.LogMapDetails();

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

std::pair<int, int> Map::MapToWorld(int x, int y) const
{
	std::pair<float, float> coord = map.MapToWorld(x, y);
	return { coord.first, coord.second };
}

std::pair<int, int> Map::WorldToMap(int x, int y) const
{
	std::pair<float, float> coord = map.WorldToMap(x, y);
	return { coord.first, coord.second };
}

