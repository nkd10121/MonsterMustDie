#include "MiniMap.h"
#include "ResourceManager.h"
#include "DrawUI.h"

namespace
{
	// 右上のUI描画位置とスケール
	constexpr int kRightUiX = 1180;
	constexpr int kRightUiY1 = 120;
	constexpr int kOffsetY = 80;
	constexpr int kRightUiY2 = 45;
	constexpr int kRightUiY3 = 40;
	constexpr float kRightUiScale1 = 0.9f;
	constexpr float kRightUiScale2 = 0.75f;

	constexpr int kMiniMapWidth = 180;
	constexpr int kMiniMapHeight = 180;

	const Vec3 kMapOrigine[] =
	{
		Vec3(0.0f,0.0f,72.0f),
		Vec3(0.0f,0.0f,84.0f),
		Vec3(0.0f,0.0f,34.0f),
	};

	const float kScale[] =
	{
		0.42f,
		0.25f,
		0.28f,
	};
}

MiniMap::MiniMap(int stageIdx)
{
	m_stageIdx = stageIdx;

	m_ironHandle = ResourceManager::GetInstance().GetHandle("I_IRONUI");
	m_bgHandle = ResourceManager::GetInstance().GetHandle("I_MINIMAPBG");
}

MiniMap::~MiniMap()
{
}

void MiniMap::Init(int mapHandle, Vec3 crystalPos)
{
	m_minimapHandle = mapHandle;

	auto mapOrigin = kMapOrigine[m_stageIdx];
	// スケーリングを適用
	float x = (crystalPos.x - mapOrigin.x) * kScale[m_stageIdx];
	float y = (crystalPos.z - mapOrigin.z) * kScale[m_stageIdx];

	// オフセットを適用
	m_crystalPosOnMinimap = ConvertToMiniMapCoords(crystalPos, mapOrigin, kScale[m_stageIdx], Vec2(kRightUiX, kRightUiY1));
}

void MiniMap::Update(Vec3 playerPos, std::list<Vec3> enemyPoss)
{

	auto mapOrigin = kMapOrigine[m_stageIdx];

	m_playerPosOnMinimap = ConvertToMiniMapCoords(playerPos, mapOrigin, kScale[m_stageIdx], Vec2(kRightUiX, kRightUiY1));

	m_enemyPosOnMinimap.clear();
	for (auto& pos : enemyPoss)
	{
		m_enemyPosOnMinimap.emplace_back(ConvertToMiniMapCoords(pos, mapOrigin, kScale[m_stageIdx], Vec2(kRightUiX, kRightUiY1)));
	}
}

void MiniMap::Draw()
{
	// 操作説明UIの描画
	DrawUI::GetInstance().RegisterDrawRequest([=]()
	{
		// 右上のUI描画
		DrawRotaGraph(kRightUiX, kRightUiY1, kRightUiScale1, 0.0f, m_bgHandle, true);
		//DrawRotaGraph(kRightUiX, kRightUiY2, kRightUiScale2, 0.0f, m_ironHandle, true);
		DrawRotaGraph(kRightUiX, kRightUiY1, 0.72f, 0.0f, m_minimapHandle, true);

		DrawCircle(static_cast<int>(m_crystalPosOnMinimap.x), static_cast<int>(m_crystalPosOnMinimap.y), 4, 0x0000ff, true);
		DrawCircle(static_cast<int>(m_playerPosOnMinimap.x), static_cast<int>(m_playerPosOnMinimap.y), 2, 0x00ff00, true);

		for (auto& enemyPos : m_enemyPosOnMinimap)
		{
			if (enemyPos.y < kRightUiY1 - kOffsetY)	continue;

			DrawCircle(static_cast<int>(enemyPos.x), static_cast<int>(enemyPos.y), 2, 0xff0000, true);
		}
	}, 0);

}

Vec2 MiniMap::ConvertToMiniMapCoords(const Vec3& worldPos, const Vec3& mapOrigin, float scale, const Vec2& miniMapCenter)
{
	Vec2 ret;

	// スケーリングを適用
	float x = (worldPos.x - mapOrigin.x) * scale;
	float y = (worldPos.z - mapOrigin.z) * scale;

	// オフセットを適用
	ret.x = x + miniMapCenter.x;
	ret.y = -y + miniMapCenter.y;

	return ret;
}