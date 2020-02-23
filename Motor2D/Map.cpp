#include <math.h>
#include "Defs.h"
#include "Log.h"
#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Collisions.h"
#include "Map.h"

Map::Map() : Module("map"), map_loaded(false)
{}

Map::~Map()
{}

// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");

	folder = config.child("folder").child_value();
	
	return true;
}

void Map::Draw()
{
	if(!map_loaded)
		return;

	for(std::list<MapLayer*>::iterator it = data.layers.begin(); it != data.layers.end(); ++it)
	{
		MapLayer* layer = *it;

		if(layer->properties.Get("Nodraw") != 0)
			continue;

		for(int y = 0; y < data.height; ++y)
		{
			for(int x = 0; x < data.width; ++x)
			{
				int tile_id = layer->Get(x, y);
				if(tile_id > 0)
				{
					TileSet* tileset = GetTilesetFromTileId(tile_id);

					SDL_Rect r = tileset->GetTileRect(tile_id);
					iPoint pos = MapToWorld(x, y);

					App->render->Blit(tileset->texture, pos.x, pos.y, &r);
				}
			}
		}
	}

	
}

int Properties::Get(const char* value, int default_value) const
{
	for (std::list<Property*>::const_iterator it = list.begin(); it != list.end(); ++it)
	{
		if((*it)->name == value)
			return (*it)->value;
	}

	return default_value;
}

TileSet* Map::GetTilesetFromTileId(int id) const
{
	std::list<TileSet*>::const_iterator it = data.tilesets.begin();
	TileSet* set = *it;

	for (; it != data.tilesets.end(); ++it)
	{
		if(id < (*it)->firstgid)
			break;

		set = *it;
	}

	return set;
}

iPoint Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (data.tile_width * 0.5f);
		ret.y = (x + y) * (data.tile_height * 0.5f);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint Map::WorldToMap(int x, int y) const
{
	iPoint ret;
	ret.x = 0;
	ret.y = 0;

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		
		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = int( (x / half_width + y / half_height) / 2);
		ret.y = int( (y / half_height - (x / half_width)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}

// Called before quitting
bool Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	for (std::list<TileSet*>::iterator it = data.tilesets.begin(); it != data.tilesets.end(); ++it)
		delete *it;

	data.tilesets.clear();

	// Remove all layers
	for (std::list<MapLayer*>::iterator it = data.layers.begin(); it != data.layers.end(); ++it)
		delete *it;

	data.layers.clear();

	//remove colliders
	for (std::list<MapObject*>::iterator it = data.objects.begin(); it != data.objects.end(); ++it)
		delete *it;

	data.objects.clear();

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool Map::Load(const char* file_name)
{
	bool ret = true;
	std::string tmp = folder + file_name;

	pugi::xml_parse_result result = map_file.load_file(tmp.c_str());

	if(result)
	{
		// Load general info
		ret = LoadMap();
		pugi::xml_node map_node = map_file.child("map");

		// Load all tilesets info
		for (pugi::xml_node tileset_child = map_node.child("tileset"); tileset_child && ret; tileset_child = tileset_child.next_sibling("tileset"))
		{
			TileSet* tileset = new TileSet();

			if (LoadTilesetDetails(tileset_child, tileset) && LoadTilesetImage(tileset_child, tileset))
				data.tilesets.push_back(tileset);
			else
				ret = false;
		}

		// Load layer info
		for (pugi::xml_node layer_child = map_node.child("layer"); layer_child && ret; layer_child = layer_child.next_sibling("layer"))
		{
			MapLayer* layer = new MapLayer();

			if (LoadLayer(layer_child, layer))
				data.layers.push_back(layer);
			else
				ret = false;
		}

		//Load object info (colliders)
		for (pugi::xml_node object_child = map_node.child("objectgroup"); object_child && ret; object_child = object_child.next_sibling("objectgroup"))
		{
			MapObject* obj = new MapObject;

			if (LoadObjects(object_child, obj))
				data.objects.push_back(obj);
			else
				ret = false;
		}

	}
	else
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Log Map details
	if (ret)
	{
		map_loaded = ret;

		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		/*for (std::list<TileSet*>::iterator it = data.tilesets.begin(); it != data.tilesets.end(); ++it)
		{
			LOG("Tileset ----");
			LOG("name: %s firstgid: %", (*it)->name.c_str(), (*it)->firstgid);
			LOG("tile width: %d tile height: %d", (*it)->tile_width, (*it)->tile_height);
			LOG("spacing: %d margin: %d", (*it)->spacing, (*it)->margin);
		}

		for (std::list<MapLayer*>::iterator it = data.layers.begin(); it != data.layers.end(); ++it)
		{
			LOG("Layer ----");
			LOG("name: %s", (*it)->name.c_str());
			LOG("tile width: %d tile height: %d", (*it)->width, (*it)->height);
		}*/
	}
	else
	{
		LOG("Error Parsing Map file content.");
	}

	return ret;
}

// Load map general properties
bool Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;
		std::string bg_color = map.attribute("backgroundcolor").as_string();
		if(bg_color.length() > 0)
		{
			std::string red = "0", green = "0", blue = "0";
			/* TODO: parse background color string
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);*/

			int v = 0;

			sscanf_s(red.c_str(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.c_str(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.c_str(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		std::string orientation = map.attribute("orientation").as_string();

		if(orientation == "orthogonal") data.type = MAPTYPE_ORTHOGONAL;
		else if(orientation == "isometric") data.type = MAPTYPE_ISOMETRIC;
		else if(orientation == "staggered") data.type = MAPTYPE_STAGGERED;
		else data.type = MAPTYPE_UNKNOWN;
	}

	return ret;
}

bool Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name = tileset_node.attribute("name").as_string();
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.c_str(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();
	LoadProperties(node, layer->properties);
	pugi::xml_node layer_data = node.child("data");

	if(layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		DEL(layer);
	}
	else
	{
		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		int i = 0;
		for(pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->data[i++] = tile.attribute("gid").as_int(0);
		}
	}

	return ret;
}

// Load a group of properties from a node and fill a list with it
bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = false;

	pugi::xml_node data = node.child("properties");

	if(data != NULL)
	{
		pugi::xml_node prop;

		for(prop = data.child("property"); prop; prop = prop.next_sibling("property"))
		{
			Properties::Property* p = new Properties::Property();

			p->name = prop.attribute("name").as_string();
			p->value = prop.attribute("value").as_int();

			properties.list.push_back(p);
		}
	}

	return ret;
}

bool Map::LoadObjects(pugi::xml_node& node, MapObject* object)
{
	bool ret = true;

	object->name = node.attribute("name").as_string();
	object->id = node.attribute("id").as_uint();
	pugi::xml_node col_object = node.child("object");

	if (node == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		DEL(object);
	}
	else
	{
		object = new MapObject;
		
		for (int i = 0; col_object; col_object = col_object.next_sibling("object"))
		{
			object->col[i] = new Collider({ col_object.attribute("x").as_int(0),col_object.attribute("y").as_int(0),col_object.attribute("width").as_int(0),col_object.attribute("height").as_int(0) }, COLLIDER);
			App->collisions->AddCollider(object->col[i]->rect, object->col[i]->type);
			i++;
		}

	}

	return ret;
}