#include "EnemyManager.h"

#include "EnemyNormal.h"
#include "EnemyFast.h"
#include "EnemyBig.h"
#include "SwarmEnemy.h"

#include "TrapManager.h"

#include "Game.h"

#include "LoadCSV.h"
#include "ScoreManager.h"
#include "ResourceManager.h"
#include "FontManager.h"
#include "DrawUI.h"

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

	const std::vector<std::string> kImageName =
	{
		"I_ENEMY_HPGAUGEBG",
		"I_ENEMY_HPGAUGE",
		"I_ENEMY_HPFRAME",
	};

	//HPバーの描画サイズ
	constexpr float kHpBarUISize = 0.2f;

	//連続キルの罠ポイント倍率
	constexpr int kTrapPointMag = 11;

	constexpr int kDrawKillStreakUIX = 100;
	constexpr int kDrawKillStreakUIIntervalX = 80;

	constexpr int kDrawKillStreakUIY = 300;
	constexpr int kDrawKillStreakUIIntervalY = 30;
	constexpr int kDrawKillStreakUIBoxSize = 20;

}

EnemyManager::EnemyManager(bool isGame) :
	m_isGame(isGame),
	m_isRayHit(false),
	m_rayHitEnemyNowHP(0),
	m_rayHitEnemyMaxHP(0),
	m_killStreakCount(0),
	m_highestKillStreakCount(0),
	m_killStreakTime(0),
	m_killStreakPoint(0),
	m_isDrawKillStreakPoint(false),
	m_drawKillStreakPointCount(0),
	m_deadEnemyNum(0),
	m_killedByPlayerNum(0),
	m_killedByTrapNum(0),
	m_createPortionPos(),
	m_isCreatePortion(false)
{
	//ゲームシーンならロードする
	if (m_isGame)
	{
		for (auto& name : kImageName)
		{
			m_enemyHpHandle.push_back(ResourceManager::GetInstance().GetHandle(name));

			if (name == "I_ENEMY_HPGAUGE")
			{
				//HPバーの画像サイズを取得する
				GetGraphSize(m_enemyHpHandle.back(), &m_gaugeWidth, &m_gaugeHeight);
			}
		}
	}
	else
	{
		auto enemyName = LoadCSV::GetInstance().GetAllEnemyName();

		m_createEnemyInfo[0] = std::list<EnemyCreateInfo>();
		for (int i = 0; i < 14; i++)
		{
			EnemyCreateInfo add;
			add.appearFrame = i;
			add.isCreated = false;

			//add.enemyName = "EnemyNormal";
			add.enemyName = enemyName[GetRand(enemyName.size() - 1)];
			m_createEnemyInfo[0].push_back(add);
		}
	}
}

EnemyManager::~EnemyManager()
{
	if (m_isGame)
	{
		for (auto& h : m_enemyHpHandle)
		{
			DeleteGraph(h);
		}
		m_enemyHpHandle.clear();
	}
}

void EnemyManager::LoadCreateData(std::string stageName)
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

bool EnemyManager::Update(int phase, Vec3 cameraPos, Vec3 angle)
{
	//初期化
	m_rayCastRetPosOnLine = Vec3();
	m_isRayHit = false;


	auto endPos = cameraPos + angle * 100000.0f;
	float length = 10000.0f;

	int preKillStreakCount = m_killStreakCount;

	std::weak_ptr<EnemyBase> retRayHitEnemy;

	//敵の更新
	for (auto& enemy : m_pEnemies)
	{
		enemy->Update(cameraPos, endPos);

		//上のUpdate内で敵が死んだかどうかを取得する
		if (enemy->GetIsKilled(m_createPortionPos))
		{
			auto rand = GetRand(100);
			if (rand < kPortionDropPercent)
			{
				m_isCreatePortion = true;
			}

			m_deadEnemyNum++;

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
			auto hitPos = enemy->GetCameraRayHitPos();

			if (m_isRayHit)
			{
				if ((m_rayCastRetPosOnLine - cameraPos).Length() > (hitPos - cameraPos).Length())
				{
					m_rayCastRetPosOnLine = hitPos;
					retRayHitEnemy = enemy->GetRayHitEnemy();
				}
			}
			else
			{
				m_rayCastRetPosOnLine = hitPos;
				retRayHitEnemy = enemy->GetRayHitEnemy();
			}

			m_isRayHit = true;
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

	//レイに当たっている敵のHPをその敵の上らへんに描画したい

	//レイに敵が当たっていたら
	if (m_isRayHit)
	{
		m_rayHitEnemyNowHP = retRayHitEnemy.lock()->GetHp();
		m_rayHitEnemyMaxHP = retRayHitEnemy.lock()->GetMaxHp();
		m_rayHitEnemyPos = retRayHitEnemy.lock()->GetPos();
		m_rayHitEnemySize = retRayHitEnemy.lock()->GetCollisionSize();
	}

	if (m_isDrawKillStreakPoint)
	{
		m_drawKillStreakPointCount++;
		if (m_drawKillStreakPointCount > 90)
		{
			m_isDrawKillStreakPoint = false;
			m_killStreakPoint = 0;
			m_drawKillStreakPointCount = 0;
		}
	}

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
			m_killStreakPoint = m_killStreakCount * kTrapPointMag;
			m_isDrawKillStreakPoint = true;
			//キルストリークカウント*11をポイントとして取得する
			TrapManager::GetInstance().AddTrapPoint(m_killStreakPoint);

			m_highestKillStreakCount = max(m_highestKillStreakCount, m_killStreakCount);
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
	//if (m_deadEnemyNum == m_enemyNum[phase])
	//{
	//	return true;
	//}

	return false;
}

void EnemyManager::Draw()
{
	for (auto& enemy : m_pEnemies)
	{
		enemy->Draw();
	}

	if (m_isRayHit)
	{
		if (m_isGame)
		{
			auto screenPos = ConvWorldPosToScreenPos(m_rayHitEnemyPos.ToVECTOR());
			screenPos.y -= m_rayHitEnemySize * 16;

			//HPの割合を計算する
			float per = static_cast<float>(m_rayHitEnemyNowHP) / static_cast<float>(m_rayHitEnemyMaxHP);

			DrawUI::GetInstance().RegisterDrawRequest([=]()
			{
				DrawRotaGraph(
					static_cast<int>(screenPos.x),	//X座標
					static_cast<int>(screenPos.y),	//Y座標
					0.2f, 0.0f,						//拡大率、回転
					m_enemyHpHandle[0],				//ハンドル
					true);							//背景透明化
			}, 0);									//レイヤー番号

			DrawUI::GetInstance().RegisterDrawRequest([=]()
			{
				DrawRectRotaGraph(
					static_cast<int>(screenPos.x - (m_gaugeWidth - m_gaugeWidth * per) * 0.5f * kHpBarUISize),
					static_cast<int>(screenPos.y),
					0, 0,
					static_cast<int>(m_gaugeWidth * per), m_gaugeHeight,
					kHpBarUISize,
					0.0f,
					m_enemyHpHandle[1],
					true);
			}, 1);

			DrawUI::GetInstance().RegisterDrawRequest([=]()
			{
				DrawRotaGraph(
					static_cast<int>(screenPos.x),	//X座標
					static_cast<int>(screenPos.y),	//Y座標
					0.2f, 0.0f,						//拡大率、回転
					m_enemyHpHandle[2],				//ハンドル
					true);							//背景透明化
			}, 2);									//レイヤー番号
		}
	}


	if (m_killStreakCount)
	{
		DrawUI::GetInstance().RegisterDrawRequest([=]()
		{
			FontManager::GetInstance().DrawCenteredText(kDrawKillStreakUIX + kDrawKillStreakUIIntervalX, kDrawKillStreakUIY, "連続キル!", 0xfebe41, 32, 0xcc0000);
			FontManager::GetInstance().DrawCenteredText(kDrawKillStreakUIX + kDrawKillStreakUIIntervalX, kDrawKillStreakUIY + kDrawKillStreakUIIntervalY, "x" + std::to_string(m_killStreakCount), 0xffffff, 32, 0x0000cc);

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
			DrawBox(kDrawKillStreakUIX, kDrawKillStreakUIY + kDrawKillStreakUIIntervalY * 2, kDrawKillStreakUIX + kDrawKillStreakUIIntervalX * 2, kDrawKillStreakUIY + kDrawKillStreakUIIntervalY * 2 + kDrawKillStreakUIBoxSize, 0x1a1a1a, true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			DrawBox(kDrawKillStreakUIX, kDrawKillStreakUIY + kDrawKillStreakUIIntervalY * 2, kDrawKillStreakUIX + kDrawKillStreakUIIntervalX * 2, kDrawKillStreakUIY + kDrawKillStreakUIIntervalY * 2 + kDrawKillStreakUIBoxSize, 0xffffff, false);
			auto limitTime = kKillStreakResetTime - (10 * m_killStreakCount);
			auto per = static_cast<float>(m_killStreakTime) / static_cast<float>(limitTime);
			DrawBox(kDrawKillStreakUIX, kDrawKillStreakUIY + kDrawKillStreakUIIntervalY * 2, kDrawKillStreakUIX + kDrawKillStreakUIIntervalX * 2 * (1.0f - per), kDrawKillStreakUIY + kDrawKillStreakUIIntervalY * 2 + kDrawKillStreakUIBoxSize, 0x91cdd9, true);
		}, 2);
	}

	if (m_isDrawKillStreakPoint)
	{
		DrawUI::GetInstance().RegisterDrawRequest([=]()
		{
			auto moveX = max(m_drawKillStreakPointCount - 60, 0) * 4;

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - moveX * 4);
			DrawRotaGraph(kDrawKillStreakUIX - moveX, kDrawKillStreakUIY + kDrawKillStreakUIIntervalY, 0.8f * max(static_cast<float>((90 - m_drawKillStreakPointCount * 3.0f) / 50.0f), 1.0f), 0.0f, ResourceManager::GetInstance().GetHandle("I_TRAPICON"), true);
			FontManager::GetInstance().DrawCenteredExtendText(kDrawKillStreakUIX + kDrawKillStreakUIIntervalX - moveX, kDrawKillStreakUIY + kDrawKillStreakUIIntervalY, std::to_string(m_killStreakPoint), 0x91cdd9, 40, 0x395f62, 1.0f);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}, 2);
	}

#ifdef _DEBUG
	DrawFormatString(0, 296, 0xffffff, "連続キルカウント:%d", m_killStreakCount);
	DrawFormatString(0, 312, 0xffffff, "連続キルCD:%d", m_killStreakTime);
#endif
}

void EnemyManager::LoadWayPoint(const char* stageName)
{
	//開くファイルのハンドルを取得
	auto fileName = (kStageDataPathFront + stageName + kStageDataPathBack);
	int handle = FileRead_open(fileName.c_str());
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

void EnemyManager::CreateEnemy(int phaseNum, int count, bool isInGame)
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

				if (isNewCreateSwarm)
				{
					addSwarm->SetFirstCreateFrame(static_cast<int>(data.appearFrame * 60));
					addSwarm->AddSwarm(add);

					isAdd = true;
				}
				data.isCreated = true;

				if (!isInGame)
				{
					EnemyCreateInfo add;
					add.appearFrame = count / 60 + GetRand(2) + 12;
					add.isCreated = false;
					add.enemyName = "EnemyNormal";
					m_createEnemyInfo[0].push_back(add);
				}
			}
			else if (data.enemyName == "EnemyFast" && !data.isCreated)
			{
				auto add = std::make_shared<EnemyFast>();
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

				if (isNewCreateSwarm)
				{
					addSwarm->SetFirstCreateFrame(static_cast<int>(data.appearFrame * 60));
					addSwarm->AddSwarm(add);

					isAdd = true;
				}
				data.isCreated = true;

				if (!isInGame)
				{
					EnemyCreateInfo add;
					add.appearFrame = count / 60 + GetRand(2) + 12;
					add.isCreated = false;
					add.enemyName = "EnemyFast";
					m_createEnemyInfo[0].push_back(add);
				}
			}
			else if (data.enemyName == "EnemyBig" && !data.isCreated)
			{
				auto add = std::make_shared<EnemyBig>();
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

				if (isNewCreateSwarm)
				{
					addSwarm->SetFirstCreateFrame(static_cast<int>(data.appearFrame * 60));
					addSwarm->AddSwarm(add);

					isAdd = true;
				}
				data.isCreated = true;

				if (!isInGame)
				{
					EnemyCreateInfo add;
					add.appearFrame = count / 60 + GetRand(2) + 12;
					add.isCreated = false;
					add.enemyName = "EnemyBig";
					m_createEnemyInfo[0].push_back(add);
				}
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

const std::list<Vec3> EnemyManager::GetEnemyPos()
{
	std::list<Vec3>ret;

	for (auto& enemy : m_pEnemies)
	{
		auto pos = enemy->GetAllEnemyPos();

		for (auto& p : pos)
		{
			ret.emplace_back(p);
		}
	}

	return ret;
}
