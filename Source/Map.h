#ifndef __MAP_H__
#define __MAP_H__

#include "Module.h"
#include "MapContainer.h"

class Map : public Module
{
public:

	Map();
	~Map();

	bool Awake(pugi::xml_node& conf) override;
	bool PostUpdate() override;
	bool CleanUp() override;

	bool LoadFromFile(const char* file_name);

	void Draw() const;
	void SwapMapType();
	void SetMapScale(float scale);

	const MapContainer* GetMap() const;

	std::pair<int, int> I_MapToWorld(int x, int y) const;
	std::pair<int, int> I_WorldToMap(int x, int y) const;

	std::pair<float, float> F_MapToWorld(float x, float y) const;
	std::pair<float, float> F_WorldToMap(float x, float y) const;

	std::pair<int, int> WorldToTileBase(int x, int y) const;
	std::pair<int, int> WorldToTileBase(float x, float y) const;

private:

	MapContainer map;
};

#endif // __MAP_H__