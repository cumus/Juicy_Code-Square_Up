#include "MapContainer.h"
#include "Application.h"
#include "Render.h"
#include "Input.h"
#include "Scene.h"
#include "TextureManager.h"
#include "Map.h"
#include "Defs.h"
#include "Log.h"

#include "Optick/include/optick.h"

#include <math.h>

bool MapContainer::Load(const char* directory, const char* file)
{

	OPTICK_EVENT("map_container_load");
	OPTICK_THREAD("MApContainerLoad");

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

			LOG("Map XML file: %s%s w/ width: %d, height: %d, tile_width: %d, tile_height: %d",
				dir.c_str(), file_name.c_str(), width, height, tile_width, tile_height);

			if (ParseTilesets(map_node))
			{
				if (ParseLayers(map_node))
				{
					ParseObjectGroups(map_node);
					loaded = true;
				}
				else
					LOG("Error parsing map xml file: Could not load layers.");
			}
			else
				LOG("Error parsing map xml file: Could not load tilesets.");
		}
		else
			LOG("Error parsing map xml file: Cannot find 'map' tag.");
	}
	else
		LOG("ERROR: Could not load map xml file %s. pugi error: %s", file, result.description());

	doc.reset();

	return loaded;
}

bool MapContainer::IsValid() const
{
	return loaded;
}

void MapContainer::Draw() const
{
	if (!loaded) return;

	int mouse_x, mouse_y;
	App->input->GetMousePosition(mouse_x, mouse_y);

	SDL_Rect cam = App->render->GetCameraRect();
	mouse_x += cam.x;
	mouse_y += cam.y;

	std::pair<int, int> up_left = I_WorldToMap(cam.x, cam.y);
	std::pair<int, int> down_right = I_WorldToMap(cam.x + cam.w, cam.y + cam.h);

	if (type == MAPTYPE_ORTHOGONAL)
	{
#ifdef DEBUG
		for (int y = up_left.second; y <= down_right.second; ++y)
		{
			for (int x = up_left.first; x <= down_right.first; ++x)
			{
				std::pair<int, int> pos = I_MapToWorld(x, y);

				// Draw debug spite at render_pos
				if (y == up_left.second || y == down_right.second || x == up_left.first || x == down_right.first) // blue border
					App->render->DrawQuad({ pos.first + 3, pos.second + 3, tile_width - 6, tile_height - 6 }, 0, 100, 250);
				else // white default
					App->render->DrawQuad({ pos.first + 1, pos.second + 1, tile_width - 2, tile_height - 2 }, 250, 250, 250, 60);
			}
		}
#endif // DEBUG

		for (std::vector<MapLayer>::const_iterator it = layers.begin(); it != layers.end(); ++it)
		{
			if (it->drawable)
			{
				for (int y = up_left.second; y <= down_right.second; ++y)
				{
					for (int x = up_left.first; x <= down_right.first; ++x)
					{
						unsigned int tile_id = it->GetID(x, y);
						int tex_id;
						SDL_Rect section;
						if (GetRectAndTexId(tile_id, section, tex_id))
						{
							std::pair<int, int> render_pos = I_MapToWorld(x, y);
							App->render->Blit(tex_id, render_pos.first, render_pos.second, &section);
						}
					}
				}
			}
		}

		// Draw mouse tile debug
		std::pair<int, int> mouse = I_WorldToMap(mouse_x, mouse_y);
		std::pair<int, int> mouse_tile_pos = I_MapToWorld(mouse.first, mouse.second);
		
		// Frist tileset size - green
		App->render->DrawQuad({ mouse_tile_pos.first, mouse_tile_pos.second, tilesets.front().tile_width, tilesets.front().tile_height }, 0, 100, 0, 180);
		
		// Map tile size - blue
		App->render->DrawQuad({ mouse_tile_pos.first, mouse_tile_pos.second, tile_width, tile_height }, 0, 0, 100, 80);
	}
	else if (type == MAPTYPE_ISOMETRIC)
	{
		// Get corner coordinates
		std::pair<int, int> up_right = I_WorldToMap(cam.x + cam.w, cam.y);
		std::pair<int, int> down_left = I_WorldToMap(cam.x, cam.y + cam.h);

		for (std::vector<MapLayer>::const_iterator it = layers.begin(); it != layers.end(); ++it)
		{
			if (it->drawable)
			{
				for (int y = up_right.second - 1; y <= down_left.second; ++y)
				{
					for (int x = up_left.first - 1; x <= down_right.first + 1; ++x)
					{
						unsigned int tile_id = it->GetID(x, y);
						int tex_id;
						SDL_Rect section;
						if (GetRectAndTexId(tile_id, section, tex_id))
						{
							// Draw tileset spite at render_pos
							std::pair<int, int> render_pos = I_MapToWorld(x, y);
							App->render->Blit(tex_id, render_pos.first, render_pos.second, &section);
						}
#ifdef DEBUG
						else
						{
							// Draw debug spite at empty position
							SDL_Rect rect = { 64, 0, 64, 64 };
							std::pair<int, int> render_pos = I_MapToWorld(x, y);
							App->render->Blit(App->scene->id_mouse_tex, render_pos.first, render_pos.second, &rect);
						}
#endif // DEBUG
					}
				}
			}
		}

		// draw mouse tile debug
		std::pair<int, int> tile_base = WorldToTileBase(float(mouse_x), float(mouse_y));
		std::pair<int, int> mouse_tile_pos = I_MapToWorld(tile_base.first, tile_base.second);
		SDL_Rect rect = { 0, 0, 64, 64 };

		// Tile base rhombus
		App->render->Blit(App->scene->id_mouse_tex, mouse_tile_pos.first, mouse_tile_pos.second, &rect);
	}
}

void MapContainer::CleanUp()
{
	tilesets.clear();
	layers.clear();
	obj_groups.clear();
}

bool MapContainer::GetTilesetFromTileId(int id, TileSet& set) const
{
	bool ret = false;

	if (id >= tilesets.front().firstgid)
	{
		for (std::vector<TileSet>::const_iterator it = tilesets.begin(); it != tilesets.end(); ++it)
		{
			if (id <= it->firstgid + it->tilecount)
			{
				set = *it;
				ret = true;
				break;
			}
		}
	}

	return ret;
}

bool MapContainer::GetRectAndTexId(int tile_id, SDL_Rect & section, int& text_id) const
{
	bool ret = false;

	if (tile_id >= tilesets.front().firstgid)
	{
		for (std::vector<TileSet>::const_iterator it = tilesets.begin(); it != tilesets.end(); ++it)
		{
			if (tile_id <= it->firstgid + it->tilecount || it->tilecount < 0)
			{
				int relative_id = tile_id - it->firstgid;
				section.w = it->tile_width;
				section.h = it->tile_height;
				section.x = it->margin + ((it->tile_width + it->spacing) * (relative_id % it->num_tiles_width));
				section.y = it->margin + ((it->tile_height + it->spacing) * (relative_id / it->num_tiles_width));
				text_id = it->texture_id;
				ret = true;
				break;
			}
		}
	}

	return ret;
}

std::pair<int, int> MapContainer::I_MapToWorld(int x, int y) const
{
	std::pair<int, int> size = { int(float(tile_width) * scale), int(float(tile_height) * scale) };

	switch (type)
	{
	case MAPTYPE_ISOMETRIC: return {
		(x - y) * size.first / 2,
		(x + y) * size.second / 2 };
	case MAPTYPE_ORTHOGONAL: return {
		x * size.first,
		y * size.second };
	default: return { x, y };
	}
}

std::pair<int, int> MapContainer::I_WorldToMap(int x, int y) const
{
	std::pair<int, int> size = { int(float(tile_width) * scale), int(float(tile_height) * scale) };

	switch (type)
	{
	case MAPTYPE_ISOMETRIC: return {
		(x / size.first) + (y / size.second),
		(y / size.second) - (x / size.first) };
	case MAPTYPE_ORTHOGONAL: return {
		(float(x) / size.first < 0) ? (x / size.first) - 1 : x / size.first,
		(float(y) / size.second < 0) ? (y / size.second) - 1 : y / size.second };
	default: return { x, y };
	}
}

std::pair<float, float> MapContainer::F_MapToWorld(float x, float y) const
{
	std::pair<float, float> size = { float(tile_width) * scale, float(tile_height) * scale };

	switch (type)
	{
	case MAPTYPE_ISOMETRIC: return {
		(x - y) * size.first * 0.5f,
		(x + y) * size.second * 0.5f };
	case MAPTYPE_ORTHOGONAL: return {
		x * size.first,
		y * size.second };
	default: return { x, y };
	}
}

std::pair<float, float> MapContainer::F_WorldToMap(float x, float y) const
{
	std::pair<float, float> size = { float(tile_width) * scale, float(tile_height) * scale };

	switch (type)
	{
	case MAPTYPE_ISOMETRIC: return {
		(x / size.first) + (y / size.second),
		(y / size.second) - (x / size.first) };
	case MAPTYPE_ORTHOGONAL: return {
		x / size.first,
		y / size.second };
	default: return { x, y };
	}
}

std::pair<int, int> MapContainer::WorldToTileBase(float x, float y) const
{
	std::pair<float, float> ret = F_WorldToMap(x, y);
	std::pair<float, float> tile_position = F_MapToWorld(ret.first, ret.second);
	float base_offset = float(tile_width) * scale / (2.0f * sin(60.0f * DEGTORAD));

	if (PointInsideTriangle({ x , y - scale },
		{ tile_position.first, tile_position.second },
		{ tile_position.first + float(tile_width) * scale, tile_position.second },
		{ tile_position.first, tile_position.second + base_offset }))
		ret.first--;

	if (PointInsideTriangle({ x , y - scale },
		{ tile_position.first, tile_position.second },
		{ tile_position.first + float(tile_width) * scale, tile_position.second },
		{ tile_position.first + float(tile_width) * scale, tile_position.second + base_offset }))
		ret.second--;

	// Fix decimal clipping
	ret.first--;
	if (ret.first < 0.0f) ret.first--;
	if (ret.second < 0.0f) ret.second--;

	return { int(ret.first), int(ret.second) };
}

void MapContainer::ParseHeader(pugi::xml_node & node)
{
	width = node.attribute("width").as_int();
	height = node.attribute("height").as_int();
	tile_width = node.attribute("tilewidth").as_int();
	tile_height = node.attribute("tileheight").as_int();

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
		// Load Tileset Header
		TileSet tileset;
		tileset.firstgid = tileset_node.attribute("firstgid").as_int();
		tileset.name = tileset_node.attribute("name").as_string();
		tileset.tile_width = tileset_node.attribute("tilewidth").as_int();
		tileset.tile_height = tileset_node.attribute("tileheight").as_int();

		tileset.spacing = tileset_node.attribute("spacing").as_int();
		tileset.margin = tileset_node.attribute("margin").as_int();
		tileset.tilecount = tileset_node.attribute("tilecount").as_int(-1);
		//tileset.columns = tileset_node.attribute("columns").as_int();

		pugi::xml_node offset_node = tileset_node.child("tileoffset");
		tileset.offset_x = offset_node ? offset_node.attribute("x").as_int() : 0;
		tileset.offset_y = offset_node ? offset_node.attribute("y").as_int() : 0;

		// Load Tileset Image
		pugi::xml_node image_node = tileset_node.child("image");
		if (image_node)
		{
			std::string tex_path = dir;
			tex_path += image_node.attribute("source").as_string();
			tileset.texture_id = App->tex->Load(tex_path.c_str());

			TextureData tex_data;
			if (App->tex->GetTextureData(tileset.texture_id, tex_data))
			{
				// not needed as we query texture size on loading
				//tileset.tex_width = image_node.attribute("width").as_int();
				//tileset.tex_height = image_node.attribute("height").as_int();

				tileset.num_tiles_width = tex_data.width / tileset.tile_width;
				tileset.num_tiles_height = tex_data.height / tileset.tile_height;

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

		if (ret)
		{

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
		static MapLayer layer;
		layer.name = layer_node.attribute("name").as_string();
		layer.width = layer_node.attribute("width").as_int();
		layer.height = layer_node.attribute("height").as_int();

		// Load Layer Properties
		if (!layer.ParseProperties(layer_node.child("properties")))
			LOG("WARNING parsing map xml file: Cannot find 'layer/properties' tag.");

		// Load Layer Data
		if (layer.ParseData(layer_node.child("data")))
		{
			LOG("Loaded Layer Data - %s", layer.name.c_str());
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
	MapObjectGroup obj_group;
	for (pugi::xml_node obj_group_node = node.child("objectgroup"); obj_group_node; obj_group_node = obj_group_node.next_sibling("objectgroup"))
	{
		// Load Map Object Group
		obj_group.name = obj_group_node.attribute("name").as_string();
		//obj_group.color = obj_group_node.attribute("color").as_string();

		// Load objects
		for (pugi::xml_node object_node = obj_group_node.child("object"); object_node; object_node = object_node.next_sibling("object"))
		{
			MapObject obj;
			obj.id = object_node.attribute("id").as_uint();
			obj.x = object_node.attribute("x").as_float();
			obj.y = object_node.attribute("y").as_float();
			obj.width = object_node.attribute("width").as_float();
			obj.height = object_node.attribute("height").as_float();

			obj_group.objects.push_back(obj);
		}

		obj_groups.push_back(obj_group);
	}
}

float MapContainer::TriangleArea(const std::pair<float, float> a, const std::pair<float, float> b, const std::pair<float, float> c)
{
	return abs((
		a.first * (b.second - c.second)
		+ b.first * (c.second - a.second)
		+ c.first * (a.second - b.second)) / 2.0f);
}

bool MapContainer::PointInsideTriangle(const std::pair<float, float> p, const std::pair<float, float> a, const std::pair<float, float> b, const std::pair<float, float> c)
{
	float abc = TriangleArea(a, b, c);
	float pbc = TriangleArea(p, c, b);
	float apc = TriangleArea(a, p, c);
	float abp = TriangleArea(a, b, p);

	return (abc == pbc + apc + abp);
}

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
	//data.shrink_to_fit();

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

unsigned int MapLayer::GetID(int x, int y) const
{
	unsigned int ret = 0;

	if (x >= 0 && x <= width &&
		y >= 0 && y <= height)
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
