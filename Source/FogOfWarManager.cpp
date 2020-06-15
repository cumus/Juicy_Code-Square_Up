#include "FogOfWarManager.h"
#include "Application.h"
#include "TextureManager.h"
#include "Map.h"
#include "Render.h"
#include "Input.h"
#include "Gameobject.h"
#include "Transform.h"
#include "Behaviour.h"
#include "Log.h"
#include "JuicyMath.h"

#include <vector>

std::vector<std::vector<bool> > FogOfWarManager::fogMap;

FogOfWarManager::FogOfWarManager()
{
	initiated = false;
}

FogOfWarManager::~FogOfWarManager()
{}

bool FogOfWarManager::Init()
{
	bool ret = true;

	if (!initiated)
	{
		width = Map::GetMapSize_I().first;
		height = Map::GetMapSize_I().second;

		debugMode = false;
		foWMapNeedsRefresh = false;

		smoothTexID = App->tex.Load("textures/fogTiles60.png");
		debugTexID = App->tex.Load("textures/fogTiles.png");

		if (smoothTexID == -1 || debugTexID == -1) ret = false;

		CreateFoWMap();
		initiated = true;
	}

	return ret;
}

bool FogOfWarManager::CleanUp()
{
	bool ret = true;
	DeleteFoWMap();

	fowMap.clear();

	if (debugTexID != -1)
	{
		App->tex.Remove(debugTexID);
		debugTexID = -1;
	}
	if (smoothTexID != -1)
	{
		App->tex.Remove(smoothTexID);
		smoothTexID = -1;
	}

	initiated = false;

	return ret;
}

void FogOfWarManager::ResetFoWMap()
{
	if (!fogMap.empty())
	{
		for (int w = 0; w < width; w++)
		{
			for (int h = 0; h < height; h++)
			{
				fogMap[w][h] = false;
			}
		}
	}
}

FoWDataStruct FogOfWarManager::GetFoWTileState(iPoint mapPos)
{
	FoWDataStruct ret;
	ret.tileFogBits = -1;
	ret.tileShroudBits = -1;

	if (CheckFoWTileBoundaries(mapPos) && !fowMap.empty())
		ret = fowMap[mapPos.x][mapPos.y];

	return ret;
}


bool FogOfWarManager::CheckFoWTileBoundaries(iPoint mapPos)
{
	return (mapPos.x >= 0 && mapPos.x < width&& mapPos.y >= 0 && mapPos.y < height);
}

void FogOfWarManager::CreateFoWMap()
{
	std::vector<bool> vec(height); //height
	fogMap.resize(width); //width

	for (int x = 0; x < width; x++)
		fogMap[x] = vec;
	
	ResetFoWMap();
}


void FogOfWarManager::DeleteFoWMap()
{
	if (!fowMap.empty())
		fowMap.clear();
}

void FogOfWarManager::Update()
{
	ResetFoWMap();
}

void FogOfWarManager::UpdateFoWMap()
{
	if (!fowMap.empty())
	{
		for (int w = 0; w < width; w++)
		{
			for (int h = 0; h < height; h++)
			{
				fowMap[w][h].tileFogBits = fow_ALL;
			}
		}
	}
}

void FogOfWarManager::DrawFoWMap()
{
	if (initiated)
	{
		// Get corner coordinates
		SDL_Rect cam = App->render->GetCameraRect();
		std::pair<int, int> up_left = Map::I_WorldToMap(cam.x, cam.y);
		std::pair<int, int> down_right = Map::I_WorldToMap(cam.x + cam.w, cam.y + cam.h);
		std::pair<int, int> up_right = Map::I_WorldToMap(cam.x + cam.w, cam.y);
		std::pair<int, int> down_left = Map::I_WorldToMap(cam.x, cam.y + cam.h);
		SDL_Rect cam_area = { cam.x - Map::GetTileSize_I().first, cam.y - (Map::GetTileSize_I().second * 2), cam.w + Map::GetTileSize_I().first, cam.h + (Map::GetTileSize_I().second * 2) };

		int displayFogTexID;
		if (debugMode) displayFogTexID = debugTexID;
		else displayFogTexID = smoothTexID;
		SDL_Rect r = { 0,0,64,64 };

		for (int y = up_right.second - 2; y <= down_left.second; ++y)
		{
			for (int x = up_left.first - 2; x <= down_right.first; ++x)
			{
				std::pair<int, int> render_pos = Map::I_MapToWorld(x, y);
				if (JMath::PointInsideRect(render_pos.first, render_pos.second, cam_area)
					&& x >= 0 && y >= 0 && x < width && y < height
					&& !debugMode && !fogMap[x][y])
				{
					// Draw tileset spite at render_pos
					App->render->Blit(displayFogTexID, render_pos.first, render_pos.second, &r, FOG_OF_WAR);
				}
			}
		}
	}
}

bool FogOfWarManager::CheckTileVisibility(iPoint mapPos)
{
	FoWDataStruct tileState = GetFoWTileState(mapPos);
	return (tileState.tileFogBits != -1 && tileState.tileShroudBits != -1 && tileState.tileFogBits != fow_ALL);
}

void FogOfWarManager::MapNeedsUpdate()
{
	if (!foWMapNeedsRefresh)
		foWMapNeedsRefresh = true;
}
