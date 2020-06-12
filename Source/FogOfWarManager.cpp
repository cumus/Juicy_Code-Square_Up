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
		//LOG("Height %d/ Width %d",height,width);

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
	/*if (!fowMap.empty())
	{
		for (int w = 0; w < width; w++)
		{
			for (int h = 0; h < height; h++)
			{
				fowMap[w][h].tileShroudBits = fow_ALL;
				fowMap[w][h].tileFogBits = fow_ALL;
			}
		}
	}*/

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
	{
		ret = fowMap[mapPos.x][mapPos.y];
	}

	return ret;
}


bool FogOfWarManager::CheckFoWTileBoundaries(iPoint mapPos)
{
	bool ret = false;
	if (mapPos.x >= 0 && mapPos.x < width && mapPos.y >= 0 && mapPos.y < height) ret  = true;
	return ret;
}

void FogOfWarManager::CreateFoWMap()
{
	/*std::vector<FoWDataStruct> vec(height); //height
	fowMap.resize(width); //width
	for (int x = 0; x < width; x++)
	{
		fowMap[x] = vec;
	}*/
	std::vector<bool> vec(height); //height
	fogMap.resize(width); //width
	for (int x = 0; x < width; x++)
	{
		fogMap[x] = vec;
	}
	
	ResetFoWMap();
	//MapNeedsUpdate();
}


void FogOfWarManager::DeleteFoWMap()
{
	if (!fowMap.empty())
	{
		fowMap.clear();
	}
}

void FogOfWarManager::Update()
{
	ResetFoWMap();
	/*if (foWMapNeedsRefresh)
	{
		LOG("Called refresh");
		UpdateFoWMap();
		for (std::map<double, Behaviour*>::iterator it = Behaviour::b_map.begin(); it != Behaviour::b_map.end(); ++it)
		{
			Event::Push(CHECK_FOW, it->second,debugMode);
		}
		foWMapNeedsRefresh = false;
	}*/
}

void FogOfWarManager::UpdateFoWMap()
{
	LOG("Update fog map");
	if (!fowMap.empty())
	{
		LOG("Fog map not empty");
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
				if (JMath::PointInsideRect(render_pos.first, render_pos.second, cam_area))
				{
					if (x >= 0 && y >= 0 && x < width && y < height)
					{
						if (!debugMode && !fogMap[x][y])
						{
							// Draw tileset spite at render_pos
							App->render->Blit(displayFogTexID, render_pos.first, render_pos.second, &r, FOG_OF_WAR);
						}
					}
				}
			}
		}
	}


	//for (int x = 0; x < width; x++)
	//{
		//for (int y = 0; y < height; y++)
		//{
			/*FoWDataStruct tileInfo = GetFoWTileState({ x, y });
			int fogId = -1;
			int shroudId = -1;

			if (tileInfo.tileFogBits != -1 && tileInfo.tileShroudBits != -1)
			{
				if (bitToTextureTable.find(tileInfo.tileFogBits) != bitToTextureTable.end())
				{
					fogId = bitToTextureTable[tileInfo.tileFogBits];
					LOG("Fog id:%d",fogId);
				}

				if (bitToTextureTable.find(tileInfo.tileShroudBits) != bitToTextureTable.end())
				{
					shroudId = bitToTextureTable[tileInfo.tileShroudBits];
					LOG("Shroud id:%d", shroudId);
				}

			}*/

			/*std::pair<int,int> worldDrawPos = Map::I_MapToWorld(x, y);

			int displayFogTexID;
			if (debugMode) displayFogTexID = debugTexID;
			else displayFogTexID = smoothTexID;*/

			//draw fog
			/*if (fogId != -1)
			{
				//App->tex.SetTextureAlpha(displayFogTexID, 128);//set the alpha of the texture to half to reproduce fog
				SDL_Rect r = { fogId * 64,0,64,64 }; //this rect crops the desired fog Id texture from the fogTiles spritesheet
				App->render->Blit(displayFogTexID, x, y, &r, FOG_OF_WAR);
			}
			if (shroudId != -1)
			{
				//App->tex.SetTextureAlpha(displayFogTexID, 255);//set the alpha to white again
				SDL_Rect r = { shroudId * 64,0,64,64 }; //this rect crops the desired fog Id texture from the fogTiles spritesheet
				App->render->Blit(displayFogTexID, x, y, &r, FOG_OF_WAR);
			}*/

			//draw fog
			/*if (!debugMode && !fogMap[x][y])
			{
				SDL_Rect r = { 0,0,64,64 }; //this rect crops the desired fog Id texture from the fogTiles spritesheet
				App->render->Blit(displayFogTexID, worldDrawPos.first, worldDrawPos.second, &r, FOG_OF_WAR);
			}*/

			//LOG("Tile X:%d/Y:%d",x,y);
		//}
	//}	
}

bool FogOfWarManager::CheckTileVisibility(iPoint mapPos)
{
	bool ret = false;
	//First check if the entity is inside the map
	//& get the tile fog information,its state, to check if is visible. 
	//Note that the function that you need does both things for you, it is recommended to check and understand what the needed function does

	FoWDataStruct tileState = GetFoWTileState(mapPos);

	if (tileState.tileFogBits != -1 && tileState.tileShroudBits != -1)
	{
		//Entity will only be visible in visible areas (no fog nor shroud)
		if (tileState.tileFogBits != fow_ALL)
			ret = true;
	}

	return ret;
}

void FogOfWarManager::MapNeedsUpdate()
{
	if (!foWMapNeedsRefresh) foWMapNeedsRefresh = true;
}
