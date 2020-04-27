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

#include <vector>

std::vector<std::vector<bool> > FogOfWarManager::fogMap;

FogOfWarManager::FogOfWarManager()
{

}

FogOfWarManager::~FogOfWarManager()
{}


bool FogOfWarManager::Init()
{
	bool ret = true;

	width = Map::GetMapSize_I().first;
	height = Map::GetMapSize_I().second;
	LOG("Height %d/ Width %d",height,width);
	debugMode = false;
	foWMapNeedsRefresh = false;

	smoothTexID = App->tex.Load("Assets/textures/fogTiles60.png");
	debugTexID = App->tex.Load("Assets/textures/fogTilesDebug.png");

	if (smoothTexID == -1 || debugTexID == -1) ret = false;

	//---------Initialize the map being used to translate bits to texture ID---------//
	//Straight-forward cases
	bitToTextureTable.insert(std::pair<unsigned short, int>(fow_ALL, 0));
	bitToTextureTable.insert(std::pair<unsigned short, int>(fow_NNN, 1));
	bitToTextureTable.insert(std::pair<unsigned short, int>(fow_WWW, 2));
	bitToTextureTable.insert(std::pair<unsigned short, int>(fow_EEE, 3));
	bitToTextureTable.insert(std::pair<unsigned short, int>(fow_SSS, 4));
	bitToTextureTable.insert(std::pair<unsigned short, int>(fow_CNW, 5));
	bitToTextureTable.insert(std::pair<unsigned short, int>(fow_CSE, 6));
	bitToTextureTable.insert(std::pair<unsigned short, int>(fow_CNE, 7));
	bitToTextureTable.insert(std::pair<unsigned short, int>(fow_CSW, 8));
	bitToTextureTable.insert(std::pair<unsigned short, int>(fow_JNE, 9));
	bitToTextureTable.insert(std::pair<unsigned short, int>(fow_JSW, 10));
	bitToTextureTable.insert(std::pair<unsigned short, int>(fow_JNW, 11));
	bitToTextureTable.insert(std::pair<unsigned short, int>(fow_JSE, 12));

	//more complicated cases (combinations)
	//diagonals
	bitToTextureTable.insert(std::pair<unsigned short, int>(20, 9));
	bitToTextureTable.insert(std::pair<unsigned short, int>(80, 10));
	bitToTextureTable.insert(std::pair<unsigned short, int>(17, 11));
	bitToTextureTable.insert(std::pair<unsigned short, int>(272, 12));
	bitToTextureTable.insert(std::pair<unsigned short, int>(273, 13));
	bitToTextureTable.insert(std::pair<unsigned short, int>(84, 14));
	//lines
	bitToTextureTable.insert(std::pair<unsigned short, int>(23, 1));
	bitToTextureTable.insert(std::pair<unsigned short, int>(308, 3));
	bitToTextureTable.insert(std::pair<unsigned short, int>(89, 2));
	bitToTextureTable.insert(std::pair<unsigned short, int>(464, 4));
	//joints
	bitToTextureTable.insert(std::pair<unsigned short, int>(6, 9));
	bitToTextureTable.insert(std::pair<unsigned short, int>(36, 9));
	bitToTextureTable.insert(std::pair<unsigned short, int>(72, 10));
	bitToTextureTable.insert(std::pair<unsigned short, int>(192, 10));
	bitToTextureTable.insert(std::pair<unsigned short, int>(3, 11));
	bitToTextureTable.insert(std::pair<unsigned short, int>(9, 11));
	bitToTextureTable.insert(std::pair<unsigned short, int>(384, 12));
	bitToTextureTable.insert(std::pair<unsigned short, int>(288, 12));
	//corners
	bitToTextureTable.insert(std::pair<unsigned short, int>(4, 9));
	bitToTextureTable.insert(std::pair<unsigned short, int>(64, 10));
	bitToTextureTable.insert(std::pair<unsigned short, int>(1, 11));
	bitToTextureTable.insert(std::pair<unsigned short, int>(256, 12));
	//------------------------end of map initialization------------------------//
	CreateFoWMap();
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
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
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

			std::pair<int,int> worldDrawPos = Map::I_MapToWorld(x, y);

			int displayFogTexID;
			if (debugMode) displayFogTexID = debugTexID;			
			else displayFogTexID = smoothTexID;

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
			if (!debugMode && !fogMap[x][y])
			{
				SDL_Rect r = { 0,0,64,64 }; //this rect crops the desired fog Id texture from the fogTiles spritesheet
				App->render->Blit(displayFogTexID, worldDrawPos.first, worldDrawPos.second, &r, FOG_OF_WAR);
			}
		
			//LOG("Tile X:%d/Y:%d",x,y);
		}
	}
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
