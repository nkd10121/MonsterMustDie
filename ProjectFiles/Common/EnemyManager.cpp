#include "EnemyManager.h"

#include "EnemyNormal.h"
#include "SwarmEnemy.h"

#include "TrapManager.h"

#include "Game.h"

#include "LoadCSV.h"
#include "ScoreManager.h"
#include "FontManager.h"

#include <cassert>

namespace
{
	const unsigned int kColor[6]
	{
		0xffffff,
		0x000000,
		0xff0000,
		0x00ff00,
		0x0000ff,
		0xffff00
	};

	const std::string kStageDataPathFront = "data/stageData/";
	const std::string kStageDataPathBack = ".Way";

	constexpr int kKillStreakResetTime = 60 * 4;

	//ポーションがドロップする確率
	constexpr int kPortionDropPercent = 12;	//12%
}

EnemyManager::EnemyManager():
	m_killStreakCount(0),
	m_killStreakTime(0),
	m_deadEnemyNum(0),
	m_killedByPlayerNum(0),
	m_killedByTrapNum(0),
	m_createPortionPos(),
	m_isCreatePortion(false)
{
}

EnemyManager::~EnemyManager()
{

}

void EnemyManager::Init(std::string stageName)
{
	auto info = LoadCSV::GetInstance().LoadEnemyCreateData(stageName);
	for (auto& inf : info)
	{
		SeparateData(inf);
	}

	for (auto& inf : m_createEnemyInfo)
	{
		m_enemyNum[inf.first] = static_cast<int>(inf.second.size());
	}
}

bool EnemyManager::Update(int phase,Vec3 cameraPos ,Vec3 angle)
{
	auto endPos = cameraPos + angle * 100000.0f;
	Vec3 returnPos;
	float length = 10000.0f;

	int preKillStreakCount = m_killStreakCount;

	//敵の更新
	for (auto& enemy : m_pEnemies)
	{
		enemy->Update(cameraPos,endPos);

		//上のUpdate内で敵が死んだかどうかを取得する
		if (enemy->GetIsKilled(m_createPortionPos))
		{
			auto rand = GetRand(100);
			if (rand < kPortionDropPercent)
			{
				m_isCreatePortion = true;
			}

			//死んでいたらキルストリークカウントを更新する
			m_killStreakCount++;
		}

		//メンバーが誰も存在していない群れがあったらメンバーが何によって倒されたか取得して次に行く
		if (!enemy->GetIsExistMember())
		{
			enemy->GetKilledData(m_killedByPlayerNum, m_killedByTrapNum);
			continue;
		}

		if (enemy->GetIsCameraRayHit())
		{
			if (returnPos.Length() == 0.0f)
			{
				returnPos = enemy->GetCameraRayHitPos();
			}
			else
			{
				if (returnPos.Length() > enemy->GetCameraRayHitPos().Length())
				{
					returnPos = enemy->GetCameraRayHitPos();
				}
			}
		}
	}
	//isExistがfalseのオブジェクトを削除
	{
		auto it = std::remove_if(m_pEnemies.begin(), m_pEnemies.end(), [](auto& v)
			{
				return v->GetIsExistMember() == false;
			});
		m_pEnemies.erase(it, m_pEnemies.end());
	}

	m_rayCastRetPos = returnPos;

	//もし連続キルカウントが0以上なら
	if (m_killStreakCount > 0)
	{
		//もしキルカウントが更新されていたらカウントをリセットする
		if (preKillStreakCount != m_killStreakCount)
		{
			m_killStreakTime = 0;
		}

		auto limitTime = kKillStreakResetTime - (10 * m_killStreakCount);

		//キルカウントの受付時間を超えたら
		if (m_killStreakTime > limitTime)
		{
			//キルストリークカウントの2乗をポイントとして取得する
			TrapManager::GetInstance().AddTrapPoint(m_killStreakCount * m_killStreakCount);

			//値をリセットする
			m_killStreakCount = 0;
			m_killStreakTime = 0;
		}
		//受付時間を超えていなかったら
		else
		{
			//カウントを更新する
			m_killStreakTime++;
		}

	}


	//もし群れの数が0になった(敵が全滅した)ら、次のフェーズに行く
	if (m_pEnemies.size() == 0)
	{
		return true;
	}

	return false;
}

void EnemyManager::Draw()
{
	for (auto& enemy : m_pEnemies)
	{
		enemy->Draw();
	}

	if (m_killStreakCount)
	{
		FontManager::GetInstance().DrawCenteredText(180, 350, "連続キル", 0xffffff, 24);
		FontManager::GetInstance().DrawCenteredText(180, 380, "x" + std::to_string(m_killStreakCount), 0xffffff, 24);
	}

#ifdef _DEBUG
	DrawFormatString(0, 296, 0xffffff, "連続キルカウント:%d", m_killStreakCount);
	DrawFormatString(0, 312, 0xffffff, "連続キルCD:%d", m_killStreakTime);
#endif
}

void EnemyManager::LoadWayPoint(const char* stageName)
{
	//開くファイルのハンドルを取得
	int handle = FileRead_open((kStageDataPathFront + stageName + kStageDataPathBack).c_str());
#ifdef _DEBUG
	assert(handle != 0 && "ウェイポイントデータファイルが開けませんでした");
#endif

	//読み込むオブジェクト数が何個あるか取得
	int dataCnt = 0;
	FileRead_read(&dataCnt, sizeof(dataCnt), handle);
	//読み込むオブジェクト数分の配列に変更する
	m_wayPoints.resize(dataCnt);

	for (auto& wp : m_wayPoints)
	{
		//名前のバイト数を取得する
		byte nameCnt = 0;
		FileRead_read(&nameCnt, sizeof(nameCnt), handle);
		//名前のサイズを変更する
		wp.name.resize(nameCnt);
		//名前を取得する
		FileRead_read(wp.name.data(), sizeof(char) * static_cast<int>(wp.name.size()), handle);
		//座標を取得する
		FileRead_read(&wp.pos, sizeof(wp.pos), handle);

		//次のウェイポイント候補数が何個あるか取得
		int nextPointCnt = 0;
		FileRead_read(&nextPointCnt, sizeof(nextPointCnt), handle);
		//候補数分のサイズに変更する
		wp.nextPointName.resize(nextPointCnt);

		for (auto& nextWp : wp.nextPointName)
		{
			//名前のバイト数を取得する
			byte nextWpNameCnt = 0;
			FileRead_read(&nextWpNameCnt, sizeof(nextWpNameCnt), handle);
			//名前のサイズを変更する
			nextWp.resize(nextWpNameCnt);
			//名前を取得する
			FileRead_read(nextWp.data(), sizeof(char) * static_cast<int>(nextWp.size()), handle);
		}
	}
}

std::vector<EnemyManager::WayPoint> EnemyManager::GetRoute()
{
	//最終的に返す値
	std::vector<WayPoint> ret;

	//startと名前の付いたWayPointを開始地点とする
	std::vector<WayPoint> startWp;
	for (auto& wp : m_wayPoints)
	{
		if (wp.name == "start")
		{
			startWp.push_back(wp);
		}
	}

	//念のためstartが見つからなかったらエラーが出るようにしておく
#ifdef _DEBUG
	assert(startWp.size() != 0 && "スタートウェイポイントが見つかっていません");
#endif
	//TODO:おそらく複数ある場合が存在するためstartWpの中から一つ選ぶ必要がある

	int r = GetRand(static_cast<int>(startWp.size()) - 1);
	//スタートウェイポイントを追加する
	ret.push_back(startWp[r]);


	int idx = 0;
	//
	while (true)
	{
		//もし最後に入れたウェイポイントの接続先がなかったら抜けるようにする
		if (ret[idx].nextPointName.size() == 0)
		{
			break;
		}

		//接続先の中からランダムに選ぶ
		int rand = GetRand(static_cast<int>(ret[idx].nextPointName.size()) - 1);
		auto addWpName = ret[idx].nextPointName[rand];

		for (auto& wp : m_wayPoints)
		{
			if (wp.name == addWpName)
			{
				ret.push_back(wp);
				break;
			}
		}

		idx++;
	}

	return ret;
}

void EnemyManager::Finalize()
{

	for (auto& enemy : m_pEnemies)
	{
		enemy->Finalize();
	}
	m_pEnemies.clear();
}

void EnemyManager::UpdateModelPos()
{
	for (auto& enemy : m_pEnemies)
	{
		enemy->UpdateModelPos();
	}
}

void EnemyManager::CreateEnemy(int phaseNum,int count)
{
	int i = 0;
	auto addSwarm = std::make_shared<SwarmEnemy>(kColor[i]);

	bool isAdd = false;

	for (auto& data : m_createEnemyInfo[phaseNum])
	{
		if (abs(data.appearFrame * 60 - count) < 0.01f)
		{
			if (data.enemyName == "EnemyNormal" && !data.isCreated)
			{
				auto add = std::make_shared<EnemyNormal>();
				add->SetRoute(GetRoute());
				add->Init();

				bool isNewCreateSwarm = false;

				//前に生成した群れクラスの最初に追加した生成フレームとの差が180以下ならその群れクラスに追加する
				if (m_pEnemies.size() > 0)
				{
					if (abs(m_pEnemies.back()->GetFirstCreateFrame() - count) < 60 * 3)
					{
						m_pEnemies.back()->AddSwarm(add);
					}
					else
					{
						isNewCreateSwarm = true;
					}
				}
				else
				{
					isNewCreateSwarm = true;
				}

				if(isNewCreateSwarm)
				{
					addSwarm->SetFirstCreateFrame(static_cast<int>(data.appearFrame * 60));
					addSwarm->AddSwarm(add);

					isAdd = true;
				}
				data.isCreated = true;
			}
		}

		i++;
	}

	if (!isAdd)	return;

	addSwarm->SetUp();
	m_pEnemies.emplace_back(addSwarm);
}

void EnemyManager::SeparateData(std::vector<std::string> data)
{
	EnemyCreateInfo add;
	add.enemyName = data[1];
	add.appearFrame = stof(data[2]);
	add.isCreated = false;

	m_createEnemyInfo[std::stoi(data[0])].push_back(add);
}

const void EnemyManager::SetScoreData() const
{
	ScoreManager::GetInstance().SetKillData(m_killedByPlayerNum, m_killedByTrapNum);
}

const bool EnemyManager::GetIsCreatePortion(Vec3& createPos)
{
	if (!m_isCreatePortion)
	{
		return false;
	}
	else
	{
		auto ret = m_isCreatePortion;
		m_isCreatePortion = false;

		createPos = m_createPortionPos;

		return ret;
	}
}
