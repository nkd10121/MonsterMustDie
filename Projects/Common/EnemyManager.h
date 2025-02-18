#pragma once
#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <string>

#include "Vec3.h"

class SwarmEnemy;	//敵

class EnemyManager
{
public:
	/// <summary>
	/// 敵生成情報
	/// </summary>
	struct EnemyCreateInfo
	{
		std::string enemyName;		//敵の名前
		float appearFrame;			//出現フレーム
		//int appearPos;			//生成場所(現在は未使用)

		bool isCreated = false;		//生成済みかどうか
	};

	struct WayPoint
	{
		std::string name;
		Vec3 pos;
		std::vector<std::string> nextPointName;
	};

public:
	EnemyManager(bool isGame = true);
	virtual ~EnemyManager();

	void LoadCreateData(std::string stageName);
	/// <summary>
	/// ウェイポイントをロード
	/// </summary>
	/// <param name="stageName">ステージ名</param>
	void LoadWayPoint(const char* stageName);
	bool Update(int phase, Vec3 cameraPos, Vec3 angle);
	void Draw();

	std::vector<WayPoint> GetRoute();

	void Finalize();

	void UpdateModelPos();

	void CreateEnemy(int phaseNum, int count, bool isInGame = true);

	void SeparateData(std::vector<std::string> data);

	const Vec3 GetRayCastRetPos()const { return m_rayCastRetPosOnLine; }

	const void SetScoreData()const;

	const bool GetIsCreatePortion(Vec3& createPos);

	const std::list<Vec3> GetEnemyPos();

	const int GetHighestKillStreakCount()const { return m_highestKillStreakCount; }

private:
	bool m_isGame;

	std::list<std::shared_ptr<SwarmEnemy>> m_pEnemies;	//敵ポインタの配列
	std::map<int, std::list<EnemyCreateInfo>> m_createEnemyInfo;	//敵生成情報
	std::unordered_map<int, int> m_enemyNum;	//それぞれのフェーズで何体の敵を生成する予定か保存

	std::list<WayPoint> m_wayPoints;

	Vec3 m_rayCastRetPosOnLine;
	Vec3 m_rayHitEnemyPos;
	bool m_isRayHit;
	int m_rayHitEnemyNowHP;
	int m_rayHitEnemyMaxHP;
	float m_rayHitEnemySize;

	std::vector<int> m_enemyHpHandle;
	int m_gaugeWidth;
	int m_gaugeHeight;

	int m_killStreakCount;
	int m_highestKillStreakCount;
	int m_killStreakTime;
	int m_killStreakPoint;
	bool m_isDrawKillStreakPoint;
	int m_drawKillStreakPointCount;

	int m_deadEnemyNum;
	int m_killedByPlayerNum;
	int m_killedByTrapNum;

	Vec3 m_createPortionPos;
	bool m_isCreatePortion;
};