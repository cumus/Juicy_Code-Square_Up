#include "Map.h"
#include "Application.h"
#include "Input.h"
#include "Defs.h"
#include "Log.h"

#include "Optick/include/optick.h"
#include "SDL/include/SDL_scancode.h"

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

bool Map::PostUpdate()
{
	map.Draw();
	return true;
}

bool Map::CleanUp()
{
	LOG("Unloading map");

	map.CleanUp();

	return true;
}

bool Map::LoadFromFile(const char* file_name)
{
	if (map.loaded)
		map.CleanUp();

	std::string map_path = App->files.GetBasePath();
	map_path += maps_folder;
	map.Load(map_path.c_str(), file_name);

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

void Map::SetMapScale(float scale)
{
	map.scale = scale;
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
	return map.F_MapToWorld(x, y);
}

std::pair<float, float> Map::F_WorldToMap(float x, float y) const
{
	return map.F_WorldToMap(x, y);
}

std::pair<int, int> Map::WorldToTileBase(int x, int y) const
{
	return map.WorldToTileBase(float(x), float(y));
}
