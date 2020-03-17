#include "MapContainer.h"
#include "Application.h"
#include "Render.h"
#include "Input.h"
#include "Scene.h"
#include "TextureManager.h"
#include "Map.h"
#include "JuicyMath.h"
#include "Defs.h"
#include "Log.h"

#include "optick-1.3.0.0/include/optick.h"

#include <math.h>

TileSet::TileSet() : 
	name("none")
{}

TileSet::TileSet(const TileSet & copy) :
	name(copy.name),
	firstgid(copy.firstgid),
	tile_width(copy.tile_width),
	tile_height(copy.tile_height),
	spacing(copy.spacing),
	margin(copy.margin),
	tilecount(copy.tilecount),
	//columns(copy.columns),
	offset_x(copy.offset_x),
	offset_y(copy.offset_y),
	num_tiles_width(copy.num_tiles_width),
	num_tiles_height(copy.num_tiles_height),
	texture_id(copy.texture_id)
	//terrain_types(copy.terrain_types),
	//data(copy.data)
{}

TileSet::~TileSet()
{}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_height));
	return rect;
}

MapLayer::MapLayer()
{}

MapLayer::MapLayer(const MapLayer & copy) :
	name(copy.name),
	width(copy.width),
	height(copy.height),
	data(copy.data),
	properties(copy.properties)
{}

MapLayer::~MapLayer()
{
	data.clear();
	properties.clear();
}

bool MapLayer::ParseProperties(pugi::xml_node layer_properties)
{
	bool ret = false;
	drawable = true;

	if (layer_properties)
	{
		for (pugi::xml_node property = layer_properties.child("property"); property; property = property.next_sibling("property"))
		{
			std::pair<std::string, float> pair = { property.attribute("name").as_string(), property.attribute("value").as_float() };

			if (pair.first == "Nodraw" && pair.second != 0)
				drawable = false;

			properties.push_back(pair);
		}

		ret = true;
	}

	return ret;
}

bool MapLayer::ParseData(pugi::xml_node layer_data)
{
	bool ret = false;

	if (layer_data)
	{
		data.clear();

		for (pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
			data.push_back(tile.attribute("gid").as_uint());

		data.shrink_to_fit();
		
		ret = (data.size() == data.capacity());
	}

	return ret;
}

int MapLayer::GetID(int x, int y) const
{
	int ret = -1;

	if (x >= 0 && x < width &&
		y >= 0 && y < height)
		ret = data[(y*width) + x];

	return ret;
}

float MapLayer::GetProperty(const char* value, float default_value) const
{
	for (std::pair<std::string, float> p : properties)
		if (p.first == value)
			return p.second;

	return default_value;
}

MapObject::MapObject()
{}

MapObject::MapObject(const MapObject & copy) :
	id(copy.id),
	x(copy.x),
	y(copy.y),
	width(copy.width),
	height(copy.height)
{}

MapObject::~MapObject()
{}

MapObjectGroup::MapObjectGroup()
{}

MapObjectGroup::MapObjectGroup(const MapObjectGroup & copy) :
	name(copy.name),
	objects(copy.objects)
{}

MapObjectGroup::~MapObjectGroup()
{
	objects.clear();
}
