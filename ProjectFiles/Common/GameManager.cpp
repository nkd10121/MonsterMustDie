#include "GameManager.h"

#include "Player.h"
#include "Camera.h"
#include "Crystal.h"
#include "HealPortion.h"
#include "EnemyManager.h"
#include "HPBar.h"

#include "ResourceManager.h"
#include "MapManager.h"
#include "TrapManager.h"
#include "EffectManager.h"
#include "ScoreManager.h"
#include "LoadCSV.h"
#include "Input.h"

#include "Game.h"

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
}

/// <summary>
/// コンストラクタ
/// </summary>
GameManager::GameManager() :
	m_isCreateEnemy(false),
	m_phaseCount(0),
	m_allPhaseCount(0),
	m_portionCount(0),
	m_isEnd(false),
	m_isClear(false)
{

}

/// <summary>
/// デストラクタ
/// </summary>
GameManager::~GameManager()
{
	MV1DeleteModel(m_stageModel);
	MV1DeleteModel(m_skyBoxHandle);

	m_pPlayer->Finalize();

	m_pEnemyManager->Finalize();


	m_pCrystal->Finalize();

	//ポーションの解放
	for (auto& object : m_pObjects)
	{
		object->Finalize();
	}
	m_pObjects.clear();

	MyLib::Physics::GetInstance().Clear();

	DeleteGraph(m_slotBgHandle);
	for (auto& h : m_slotIconHandle)
	{
		DeleteGraph(h);
	}

}

/// <summary>
/// 初期化
/// </summary>
void GameManager::Init(int stageIdx)
{
	auto info = LoadCSV::GetInstance().LoadStageInfo(stageIdx);
	auto stageName = info[0];
	m_stageId = info[1];

	m_allPhaseNum = std::stoi(info[3]);
	for (int i = 1; i < m_allPhaseNum + 1; i++)
	{
		m_phaseNum.push_back(-i);
		m_phaseNum.push_back(i);
	}
	//クリア番号として0を最後に入れておく
	m_phaseNum.push_back(0);

	//ステージの当たり判定モデルを取得する(描画するため)
	m_stageModel = ResourceManager::GetInstance().GetHandle(m_stageId);
	MV1SetScale(m_stageModel, VGet(0.01f, 0.01f, 0.01f));		//サイズの変更
	MV1SetRotationXYZ(m_stageModel, VGet(0.0f, DX_PI_F, 0.0f));	//回転

	//ステージの当たり判定を設定
	MyLib::Physics::GetInstance().SetStageCollisionModel(info[1], info[2]);

	//プレイヤーの生成
	m_pPlayer = std::make_shared<Player>();
	m_pPlayer->Init(m_stageId);

	//敵管理クラスの生成
	m_pEnemyManager = std::make_shared<EnemyManager>();
	m_pEnemyManager->Init(stageName.c_str());
	m_pEnemyManager->LoadWayPoint(stageName.c_str());

	//カメラの生成
	m_pCamera = std::make_shared<Camera>();
	m_pCamera->Init(m_stageModel);

	//ステージ情報をロード
	MapManager::GetInstance().Init();
	MapManager::GetInstance().Load(stageName.c_str());

	//クリスタルの生成
	m_pCrystal = std::make_shared<Crystal>(std::stoi(info[5]));
	m_pCrystal->Init();
	m_pCrystal->Set(MapManager::GetInstance().GetCrystalPos());

	////DEBUG:ポーションを生成
	//m_pObjects.emplace_back(std::make_shared<HealPortion>());
	//m_pObjects.back()->Init();
	//m_pObjects.back()->SetPosition(Vec3(0.0f, 0.0f, -10.0f));

	m_portionMax = stoi(info[7]);

	m_skyBoxHandle = ResourceManager::GetInstance().GetHandle("M_SKYBOX");
	float scale = 0.6f;
	MV1SetScale(m_skyBoxHandle, VGet(scale, scale, scale));

	m_slotBgHandle = ResourceManager::GetInstance().GetHandle("I_SLOTBG");
	m_slotIconHandle.push_back(ResourceManager::GetInstance().GetHandle("I_SLOTCROSSBOW"));
	m_slotIconHandle.push_back(ResourceManager::GetInstance().GetHandle("I_SPIKE"));
	m_slotIconHandle.push_back(ResourceManager::GetInstance().GetHandle("I_SLOTARROWWALL"));
	m_slotIconHandle.push_back(ResourceManager::GetInstance().GetHandle("I_CRYSTALBG"));
	m_slotIconHandle.push_back(ResourceManager::GetInstance().GetHandle("I_IRONUI"));
	m_slotIconHandle.push_back(ResourceManager::GetInstance().GetHandle("I_MINIMAPBG"));

	m_pHpUi = std::make_shared<HPBar>();
	m_pHpUi->Init(m_pPlayer->GetHp());

	m_initTrapPoint = std::stoi(info[4]);
	TrapManager::GetInstance().Load(stageName.c_str());
	TrapManager::GetInstance().SetUp(m_initTrapPoint);

	//目標クリアタイムの設定
	ScoreManager::GetInstance().SetTargetClearTime(std::stoi(info[6]));
}

/// <summary>
/// 更新
/// </summary>
void GameManager::Update()
{
	//Yボタンを押した時かつ最初のフェーズの時(コメント化している部分はデバッグでフェーズをスキップするためにされている。)
	if (Input::GetInstance().IsTriggered("Y") && m_phaseNum.front() == -1)
	{
		//次のフェーズに進む
		m_phaseNum.pop_front();
	}

	m_pCrystal->Update();


	//カメラの更新
	m_pCamera->SetPlayerPos(m_pPlayer->GetPos());
	m_pCamera->Update();
	auto hitPos = m_pCamera->GetMapHitPosition();

	//現在のフェーズが0以上(戦闘フェーズ)の時、敵を生成していなかったら敵を生成する
	if (m_phaseNum.front() >= 0)
	{
		m_pEnemyManager->CreateEnemy(m_phaseNum.front(), m_phaseCount);
	}
	//0以下(準備フェーズ)の時
	else
	{
		//敵生成フラグをリセットする
		m_isCreateEnemy = false;
	}

	//カメラの座標と向きを取得
	TrapManager::GetInstance().SetCameraInfo(m_pCamera->GetCameraPos(), m_pCamera->GetDirection());

	//敵の更新処理
	auto isNextPhase = m_pEnemyManager->Update(m_phaseNum.front(), m_pCamera->GetCameraPos(), m_pCamera->GetDirection());

	//レイキャストの結果変数
	Vec3 rayCastRet;
	//カメラから敵までの距離
	auto cameraToEnemy = (m_pEnemyManager->GetRayCastRetPos() - m_pCamera->GetCameraPos()).Length();
	//カメラから地形までの距離
	auto cameraToMap = (m_pCamera->GetMapHitPosition() - m_pCamera->GetCameraPos()).Length();
	//もし、カメラから地形までの距離よりカメラから敵までの距離が大きかったら
	if (cameraToEnemy > cameraToMap)
	{
		//近い方(地形とのヒット座標)を保存する
		rayCastRet = m_pCamera->GetMapHitPosition();
	}
	else
	{
		//近い方(敵とのヒット座標)を保存するが、
		//そもそも敵と当たっているかどうかを判断する

		//敵と当たっていたら
		if (m_pEnemyManager->GetRayCastRetPos().Length() != 0.0f)
		{
			//敵とのヒット座標を保存する
			rayCastRet = m_pEnemyManager->GetRayCastRetPos();
		}
		//当たっていなかったら
		else
		{
			//地形とのヒット座標を保存する
			rayCastRet = m_pCamera->GetMapHitPosition();
		}
	}

	//ポーションの生成
	Vec3 createPos;
	if (m_pEnemyManager->GetIsCreatePortion(createPos))
	{
		if (m_portionCount < m_portionMax)
		{
			m_pObjects.emplace_back(std::make_shared<HealPortion>());
			m_pObjects.back()->Init();
			m_pObjects.back()->SetPosition(createPos);
		}
		m_portionCount++;
	}

	//プレイヤーの更新
	m_pPlayer->SetCameraAngle(m_pCamera->GetDirection());
	m_pPlayer->Update(this, rayCastRet);
	if (m_pPlayer->GetIsDeath())
	{
		//クリスタルの残りHPが5以上あればクリスタルHPを消費してプレイヤーを復活させる
		//TODO:画面を黒にフェードさせたい
		if (m_pCrystal->GetHp() > 5)
		{
			//クリスタルのHPを減らす
			m_pCrystal->PlayerDead();

			//プレイヤーの生成
			m_pPlayer = std::make_shared<Player>();
			m_pPlayer->Init(m_stageId);
			m_pHpUi->Init(m_pPlayer->GetHp());
		}
		//クリスタルの残りHPが5以下ならゲームオーバーにする
		else
		{
			m_isEnd = true;
			m_isClear = false;
			return;
		}
	}

	//敵が全滅した時、次のフェーズに進む
	if (isNextPhase)
	{
		//念のため、現在が戦闘フェーズであるか確認
		if (m_phaseNum.front() > 0)
		{
			//フェーズカウントをリセット
			m_allPhaseCount += m_phaseCount;
			m_phaseCount = 0;

			auto addPoint = m_initTrapPoint / m_allPhaseNum * (m_phaseNum.front());
			TrapManager::GetInstance().AddTrapPoint(addPoint);

			//次の準備フェーズに進む
			m_phaseNum.pop_front();
		}
	}

	//最初の準備フレームでなければフェーズカウントを進める
	if (m_phaseNum.front() != -1)
	{
		m_phaseCount++;
	}


	//10秒経ったら次のフェーズに進める
	if (m_phaseNum.front() < 0 && m_phaseNum.front() != -1 && m_phaseCount >= 600)
	{
		m_phaseNum.pop_front();
		//フェーズカウントをリセット
		m_allPhaseCount += m_phaseCount;
		m_phaseCount = 0;
	}

	//ポーションの更新
	for (auto& object : m_pObjects)
	{
		object->Update();

		if (!object->GetIsExist())
		{
			object->Finalize();
		}
	}
	//isExistがfalseのオブジェクトを削除
	{
		auto it = std::remove_if(m_pObjects.begin(), m_pObjects.end(), [](auto& v)
			{
				return v->GetIsExist() == false;
			});
		m_pObjects.erase(it, m_pObjects.end());
	}

	TrapManager::GetInstance().SetIsPrePhase(m_phaseNum.front() < 0);
	TrapManager::GetInstance().Update();

	//物理更新
	MyLib::Physics::GetInstance().Update();

	//モデル座標の更新
	m_pPlayer->UpdateModelPos();

	m_pEnemyManager->UpdateModelPos();

	m_pHpUi->Update(m_pPlayer->GetHp());

	//エフェクトの更新
	EffectManager::GetInstance().Update();

	MV1SetPosition(m_skyBoxHandle, m_pPlayer->GetPos().ToVECTOR());

	if (m_pCrystal->GetHp() <= 0)
	{
		m_isClear = false;
		m_isEnd = true;
	}

	if (m_phaseNum.front() == 0)
	{
		m_pPlayer->SetClearState();

		if (m_pPlayer->GetNowAnimEndFrame() - 1 == m_pPlayer->GetAnimNowFrame())
		{
			//クリスタルの残りHPをスコア計算用に保存
			ScoreManager::GetInstance().SetCrystalHp(m_pCrystal->GetHp());
			ScoreManager::GetInstance().SetClearTime(m_allPhaseCount);
			m_pEnemyManager->SetScoreData();
			//スコアを計算
			ScoreManager::GetInstance().CalculationScore();
			m_isClear = true;
			m_isEnd = true;
		}
	}

}

/// <summary>
/// 描画
/// </summary>
void GameManager::Draw()
{
	MV1DrawModel(m_skyBoxHandle);
	//ステージの描画
	MapManager::GetInstance().Draw();

#ifdef _DEBUG	//デバッグ描画
	//MV1DrawModel(m_stageModel);
	MyLib::DebugDraw::Draw3D();
#endif

	m_pCrystal->Draw();

	EffectManager::GetInstance().Draw();

	//プレイヤーの描画
	m_pPlayer->Draw();

	m_pEnemyManager->Draw();

	//ポーションの描画
	for (auto& object : m_pObjects)
	{
		object->Draw();
	}

	TrapManager::GetInstance().Draw();
	TrapManager::GetInstance().PreviewDraw();

	//TODO:UIクラスみたいなのを作ってそこに移動させる
	//装備スロットの描画
	for (int i = 0; i < 3; i++)
	{
		int x = 362 + i * 85;
		int y = 655;
		//DrawBox(x - 30, y - 30, x + 30, y + 30, 0xffffff, false);
		DrawRotaGraph(x, y, 0.08f, 0.0f, m_slotBgHandle, true);
		//if (i == 0)
		{
			DrawRotaGraph(x, y, 0.5f, 0.0f, m_slotIconHandle[i], true);
		}
	}

	//現在選択しているスロット枠の描画
	DrawBox(362 + m_pPlayer->GetNowSlotNumber() * 85 - 35, 655 - 35, 362 + m_pPlayer->GetNowSlotNumber() * 85 + 35, 655 + 35, 0xff0000, false);

	//右上のUI描画
	DrawRotaGraph(1180, 150, 0.9f, 0.0f, m_slotIconHandle[5], true);
	DrawRotaGraph(1180, 45, 0.75f, 0.0f, m_slotIconHandle[4], true);
	DrawRotaGraph(1180, 40, 0.65f, 0.0f, m_slotIconHandle[3], true);

	DrawFormatString(1160, 240, 0xffffff, "%d / 3", abs(m_phaseNum.front()));

	m_pHpUi->Draw();

	//クロスヘアの描画
	auto centerX = Game::kWindowWidth / 2;
	auto centerY = Game::kWindowHeight / 2;
	auto wid = 14;
	auto hei = 2;
	DrawBox(centerX - wid, centerY - hei, centerX + wid, centerY + hei, 0xaaaaaa, true);
	DrawBox(centerX - hei, centerY - wid, centerX + hei, centerY + wid, 0xaaaaaa, true);

	if (m_phaseNum.front() == -1)
	{
		DrawString(580, 20, "Yボタンでゲーム開始", 0xffffff);
	}

	//準備フェーズなら
	if (m_phaseNum.front() < 0 && m_phaseNum.front() != -1)
	{
		DrawFormatString(580, 20, 0xffffff, "次のフェーズまで %d", (660 - m_phaseCount) / 60);
	}

#ifdef _DEBUG	//デバッグ描画
	DrawFormatString(640, 0, 0xffffff, "フェーズ番号:%d", m_phaseNum.front());
	DrawFormatString(640, 16, 0xffffff, "現在のフェーズの経過フレーム:%d", m_phaseCount);
	DrawFormatString(640, 32, 0xffffff, "すべてのフェーズの経過フレーム:%d", m_allPhaseCount);
#endif
	DrawFormatString(1172, 32, 0xffffff, "%d", m_pCrystal->GetHp());
}

/// <summary>
/// オブジェクトを追加する
/// </summary>
void GameManager::AddObject(std::shared_ptr<ObjectBase> pAddObject)
{
	m_pObjects.emplace_back(pAddObject);
}