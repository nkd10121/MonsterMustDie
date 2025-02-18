#pragma once
#include "Vec3.h"
#include "Vec2.h"
#include <list>

class MiniMap
{
public:
	MiniMap(int stageIdx);
	virtual ~MiniMap();

	void Init(int mapHandle,Vec3 crystalPos);
	void Update(Vec3 playerPos, std::list<Vec3> enemyPoss);
	void Draw();

private:
	Vec2 ConvertToMiniMapCoords(const Vec3& worldPos, const Vec3& mapOrigin, float scale, const Vec2& miniMapCenter);
private:
	int m_stageIdx;

	int m_minimapHandle;
	int m_bgHandle;
	int m_ironHandle;

	Vec2 m_playerPosOnMinimap;
	Vec2 m_crystalPosOnMinimap;
	std::list<Vec2> m_enemyPosOnMinimap;
};

