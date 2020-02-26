#include "MapContainer.h"
#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Defs.h"
#include "Log.h"

void MapContainer::Draw() const
{
	if (!loaded)
		return;

	for (std::vector<MapLayer>::const_iterator it = layers.begin(); it != layers.end(); ++it)
	{
		if (it->GetProperty("Nodraw") != 0)
			continue;

		// TODO: Frustum Culling
		SDL_Rect cam = App->render->camera;
		std::pair<float, float> min = WorldToMap(cam.x, cam.y);
		std::pair<float, float> max = WorldToMap(cam.x + cam.w, cam.y + cam.h);

		/*if (size.first + pos.first > width - size.first)
			size.first = width - size.first;
		else
			size.first += pos.first;

		if (size.second + pos.second > height - size.second)
			size.second = height - size.second;
		else
			size.second += pos.second;*

		if (pos.first < 0) pos.first = 0;
		if (pos.second < 0) pos.second = 0;*/

		float zoom = 2;

		/*std::pair<int, int> midTile = WorldToMap((cam.x + cam.w) / 2, (cam.y + cam.h) / 2);
		max.first = midTile.first + int((float(cam.w) / float(tile_width)) / zoom);
		min.first = midTile.first - int((float(cam.w) / float(tile_width)) / zoom);
		max.second = midTile.second + int((float(cam.h) / float(tile_height)) / zoom);
		min.second = midTile.second - int((float(cam.h) / float(tile_height)) / zoom);*/


		for (int y = min.second - 1; y <= max.second + 1; ++y)
		{
			for (int x = min.first - 1; x <= max.first + 1; ++x)
			{
				TileSet tileset;
				int tile_id = it->GetID(x, y);
				if (tile_id > 0 && GetTilesetFromTileId(tile_id, tileset))
				{
					SDL_Rect r = tileset.GetTileRect(tile_id);
					std::pair<int, int> pos = MapToWorld(x - min.first, y + min.second);
					App->render->Blit(tileset.texture, pos.first - cam.x, pos.second, &r);
				}
			}
		}
	}
}

void MapContainer::CleanUp()
{
	tilesets.clear();
	layers.clear();
	obj_groups.clear();

	doc.reset();
}

bool MapContainer::Load(const char* directory, const char* file)
{
	loaded = false;
	dir = directory;
	file_name = file;
	pugi::xml_parse_result result = doc.load_file((dir + file_name).c_str());

	if (result)
	{
		pugi::xml_node map_node = doc.child("map");
		if (map_node)
		{
			ParseHeader(map_node);

			if (ParseTilesets(map_node) && ParseLayers(map_node))
			{
				ParseObjectGroups(map_node);
				loaded = true;
			}
			else
				LOG("Error parsing map xml file: Could not load tilesets or layers.");
		}
		else
			LOG("Error parsing map xml file: Cannot find 'map' tag.");
	}
	else
		LOG("ERROR: Could not load map xml file %s. pugi error: %s", file, result.description());

	return loaded;
}

bool MapContainer::IsValid() const
{
	return loaded;
}

void MapContainer::LogMapDetails() const
{
	LOG("Map XML file: %s%s\nwidth: %d, height: %d, tile_width: %d, tile_height: %d",
		dir.c_str(), file_name.c_str(), width, height, tile_width, tile_height);

	/*for (std::vector<TileSet>::const_iterator it = tilesets.begin(); it != tilesets.end(); ++it)
	{
		LOG("Tileset ----");
		LOG("name: %s firstgid: %d", it->name.c_str(), it->firstgid);
		LOG("tile width: %d tile height: %d", it->tile_width, it->tile_height);
		LOG("spacing: %d margin: %d", it->spacing, it->margin);
	}

	for (std::vector<MapLayer>::const_iterator it = layers.begin(); it != layers.end(); ++it)
	{
		LOG("Layer ----");
		LOG("name: %s", it->name.c_str());
		LOG("tile width: %d tile height: %d", it->width, it->height);
	}*/
}

bool MapContainer::GetTilesetFromTileId(int id, TileSet& set) const
{
	set = tilesets.front();
	for (std::vector<TileSet>::const_iterator it = tilesets.begin(); it != tilesets.end(); ++it)
	{
		if (id < it->firstgid)
			return true;

		set = *it;
	}

	return false;
}

std::pair<float, float> MapContainer::MapToWorld(float x, float y) const
{
	switch (type)
	{
	case MAPTYPE_ISOMETRIC:
		return { (x - y) * (float(tile_width) * 0.5f), (x + y) * (float(tile_height) * 0.5f) };
	case MAPTYPE_ORTHOGONAL:
		return { x * float(tile_width), y * float(tile_height) };
	default:
		return { x , y };
	}
}

std::pair<float, float> MapContainer::WorldToMap(float x, float y) const
{
	switch (type)
	{
	case MAPTYPE_ISOMETRIC:
		return { (x / float(tile_width)) + (y / float(tile_height)),
		(y / float(tile_height)) - (x / float(tile_width))};
	case MAPTYPE_ORTHOGONAL:
		return { x / float(tile_width), y / float(tile_height) };
	default:
		return { x , y };
	}
}

void MapContainer::ParseHeader(pugi::xml_node & node)
{
	width = node.attribute("width").as_int();
	height = node.attribute("height").as_int();
	tile_width = node.attribute("tilewidth").as_int();
	tile_height = node.attribute("tileheight").as_int();

	/*background_color.r = 0;
	background_color.g = 0;
	background_color.b = 0;
	background_color.a = 0;
	std::string bg_color = node.attribute("backgroundcolor").as_string();
	if (bg_color.length() > 0)
	{
		std::string red = "0", green = "0", blue = "0";
		// TODO: parse background color string
		bg_color.SubString(1, 2, red);
		bg_color.SubString(3, 4, green);
		bg_color.SubString(5, 6, blue);

		int v = 0;

		sscanf_s(red.c_str(), "%x", &v);
		if (v >= 0 && v <= 255) background_color.r = v;

		sscanf_s(green.c_str(), "%x", &v);
		if (v >= 0 && v <= 255) background_color.g = v;

		sscanf_s(blue.c_str(), "%x", &v);
		if (v >= 0 && v <= 255) background_color.b = v;
	}*/

	std::string orientation = node.attribute("orientation").as_string();

	if (orientation == "orthogonal") type = MAPTYPE_ORTHOGONAL;
	else if (orientation == "isometric") type = MAPTYPE_ISOMETRIC;
	else if (orientation == "staggered") type = MAPTYPE_STAGGERED;
	else type = MAPTYPE_UNKNOWN;
}

bool MapContainer::ParseTilesets(pugi::xml_node & node)
{
	bool ret = true;

	// Load all tilesets info
	for (pugi::xml_node tileset_node = node.child("tileset"); tileset_node && ret; tileset_node = tileset_node.next_sibling("tileset"))
	{
		TileSet tileset;

		// Load Tileset Data
		tileset.firstgid = tileset_node.attribute("firstgid").as_int();
		tileset.name = tileset_node.attribute("name").as_string();
		tileset.tile_width = tileset_node.attribute("tilewidth").as_int();
		tileset.tile_height = tileset_node.attribute("tileheight").as_int();
		tileset.spacing = tileset_node.attribute("spacing").as_int();
		tileset.margin = tileset_node.attribute("margin").as_int();
		tileset.tilecount = tileset_node.attribute("tilecount").as_int();
		tileset.columns = tileset_node.attribute("columns").as_int();
		pugi::xml_node offset_node = tileset_node.child("tileoffset");
		if (offset_node)
		{
			tileset.offset_x = offset_node.attribute("x").as_int();
			tileset.offset_y = offset_node.attribute("y").as_int();
		}
		else
		{
			tileset.offset_x = 0;
			tileset.offset_y = 0;
		}

		// Load Tileset Image
		pugi::xml_node image_node = tileset_node.child("image");
		if (image_node)
		{
			std::string tex_path = dir + "/";
			tex_path += image_node.attribute("source").as_string();
			tileset.texture = App->tex->Load(tex_path.c_str());

			if (tileset.texture != nullptr)
			{
				tileset.tex_width = image_node.attribute("width").as_int();
				tileset.tex_height = image_node.attribute("height").as_int();

				if (tileset.tex_width <= 0 || tileset.tex_height <= 0)
				{
					int w, h;
					SDL_QueryTexture(tileset.texture, 0, 0, &w, &h);
					tileset.tex_width = w;
					tileset.tex_height = h;
				}

				tileset.num_tiles_width = tileset.tex_width / tileset.tile_width;
				tileset.num_tiles_height = tileset.tex_height / tileset.tile_height;

				LOG("Loading tileset - %s - correctly!!", tileset.name.c_str());
				tilesets.push_back(tileset);
			}
			else
			{
				LOG("Error loading tileset texture: %s", tex_path.c_str());
				ret = false;
			}
		}
		else
		{
			LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
			ret = false;
		}
	}

	return ret;
}

bool MapContainer::ParseLayers(pugi::xml_node & node)
{
	bool ret = true;

	// Load layer info
	for (pugi::xml_node layer_node = node.child("layer"); layer_node && ret; layer_node = layer_node.next_sibling("layer"))
	{
		MapLayer layer;
		layer.name = layer_node.attribute("name").as_string();
		layer.width = layer_node.attribute("width").as_int(0);
		layer.height = layer_node.attribute("height").as_int(0);

		// Load Layer Properties
		pugi::xml_node properties = layer_node.child("properties");
		if (properties)
			for (pugi::xml_node property = properties.child("property"); property; property = properties.next_sibling("property"))
				layer.properties.push_back(std::pair<const char*, int> {
				property.attribute("name").as_string(), property.attribute("value").as_int() });

		// Load Layer Data
		pugi::xml_node layer_data = layer_node.child("data");
		if (layer_data)
		{
			layer.data.clear();
			layer.data.resize(layer.width * layer.height);

			int i = 0;
			for (pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
				layer.data[++i] = tile.attribute("gid").as_uint(0);

			LOG("Loaded Layer Data - %s, (parsed %d/%d)", layer.name.c_str(), i, layer.width * layer.height);
			layers.push_back(layer);
		}
		else
		{
			LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
			ret = false;
		}
	}

	return ret;
}

void MapContainer::ParseObjectGroups(pugi::xml_node & node)
{
	//Load object groups
	for (pugi::xml_node obj_group_node = node.child("objectgroup"); obj_group_node; obj_group_node = obj_group_node.next_sibling("objectgroup"))
	{
		MapObjectGroup obj_group;

		// Load Map Object Group
		obj_group.name = obj_group_node.attribute("name").as_string();
		//obj_group.color = obj_group_node.attribute("color").as_string();

		// Load objects
		for (pugi::xml_node object_node = obj_group_node.child("object"); object_node; object_node = object_node.next_sibling("object"))
		{
			MapObjectGroup::MapObject obj;
			obj.id = object_node.attribute("id").as_int();
			obj.x = object_node.attribute("x").as_float();
			obj.y = object_node.attribute("y").as_float();
			obj.width = object_node.attribute("width").as_float();
			obj.height = object_node.attribute("height").as_float();

			obj_group.objects.push_back(obj);
		}

		obj_groups.push_back(obj_group);
	}
}

SDL_Rect& TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_height));
	return rect;
}

MapLayer::~MapLayer()
{
	data.clear();
	properties.clear();
}

inline unsigned int MapLayer::GetID(int x, int y) const
{
	return data[(y*width) + x];
}

int MapLayer::GetProperty(const char* value, int default_value) const
{
	for (std::pair<const char*, int> p : properties)
		if (p.first == value)
			return p.second;

	return default_value;
}
