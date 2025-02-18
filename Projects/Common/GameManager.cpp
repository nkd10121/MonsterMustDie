#include "GameManager.h"

#include "Player.h"
#include "Camera.h"
#include "Crystal.h"
#include "HealPortion.h"
#include "EnemyManager.h"
#include "HPBar.h"
#include "MiniMap.h"

#include "ResourceManager.h"
#include "MapManager.h"
#include "TrapManager.h"
#include "EffectManager.h"
#include "ScoreManager.h"
#include "SoundManager.h"
#include "LoadCSV.h"
#include "Input.h"
#include "Setting.h"

#include "DrawUI.h"
#include "FontManager.h"

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

	// ステージモデルのスケール
	constexpr float kStageModelScale = 0.01f;
	// スカイボックスのスケール
	constexpr float kSkyBoxScale = 0.6f;
	// 初期フェーズ
	constexpr int kInitialPhase = -1;
	// フェーズ遷移時間（10秒）
	constexpr int kPhaseTransitionTime = 600;
	// プレイヤー復活時のクリスタルHP閾値
	constexpr int kPlayerReviveHpThreshold = 5;

	// 装備スロットの描画位置とスケール
	constexpr int kSlotBgX = 362;
	constexpr int kSlotBgY = 655;
	constexpr int kSlotBgOffset = 85;
	constexpr float kSlotBgScale = 0.08f;
	constexpr float kSlotIconScale = 0.5f;
	constexpr int kSlotBoxSize = 35;




	// フェーズ番号の描画位置
	constexpr int kPhaseNumX = 1180;
	constexpr int kPhaseNumY = 230;

	// クロスヘアの描画位置とサイズ
	constexpr int kCrosshairWidth = 14;
	constexpr int kCrosshairHeight = 2;

	// ゲーム開始メッセージの描画位置
	constexpr int kStartMessageX = 580;
	constexpr int kStartMessageY = 40;

	// フェーズメッセージの描画位置
	constexpr int kPhaseMessageX = 580;
	constexpr int kPhaseMessageY = 20;

	// デバッグ描画位置
	constexpr int kDebugPhaseNumX = 640;
	constexpr int kDebugPhaseNumY = 0;
	constexpr int kDebugPhaseCountX = 640;
	constexpr int kDebugPhaseCountY = 16;
	constexpr int kDebugAllPhaseCountX = 640;
	constexpr int kDebugAllPhaseCountY = 32;
}

/// <summary>
/// コンストラクタ
/// </summary>
GameManager::GameManager() :
	m_stageId(""), // ステージIDの初期化
	m_isCreateEnemy(false),
	m_phaseCount(0),
	m_allPhaseCount(0),
	m_initTrapPoint(0),
	m_allPhaseNum(0),
	m_portionCount(0),
	m_portionMax(0),
	m_slotBgHandle(-1), 
	m_isEnd(false),
	m_isClear(false),
	m_stageModel(-1),
	m_skyBoxHandle(-1),
	m_alpha(0),
	m_playerDeathCount(0)
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


	TrapManager::GetInstance().Clear();

}

/// <summary>
/// 初期化
/// </summary>
void GameManager::Init(int stageIdx)
{
	// ステージ情報をCSVから読み込む
	auto info = LoadCSV::GetInstance().LoadStageInfo(stageIdx);
	auto stageName = info[0];
	m_stageId = info[1];

	// 全フェーズ数を設定
	m_allPhaseNum = std::stoi(info[3]);
	for (int i = 1; i < m_allPhaseNum + 1; i++)
	{
		m_phaseNum.push_back(-i);
		m_phaseNum.push_back(i);
	}
	// クリア番号として0を最後に入れておく
	m_phaseNum.push_back(0);

	// ステージの当たり判定モデルを取得する(描画するため)
	m_stageModel = ResourceManager::GetInstance().GetHandle(m_stageId);
	MV1SetScale(m_stageModel, VGet(kStageModelScale, kStageModelScale, kStageModelScale)); // サイズの変更
	MV1SetRotationXYZ(m_stageModel, VGet(0.0f, DX_PI_F, 0.0f)); // 回転

	// ステージの当たり判定を設定
	MyLib::Physics::GetInstance().SetStageCollisionModel(info[1], info[2]);

	// プレイヤーの生成
	m_pPlayer = std::make_shared<Player>();
	m_pPlayer->Init(m_stageId);

	// 敵管理クラスの生成
	m_pEnemyManager = std::make_shared<EnemyManager>();
	m_pEnemyManager->LoadCreateData(stageName.c_str());
	m_pEnemyManager->LoadWayPoint(stageName.c_str());

	// カメラの生成
	m_pCamera = std::make_shared<Camera>();
	m_pCamera->Init(m_stageModel);

	// ステージ情報をロード
	MapManager::GetInstance().Init();
	MapManager::GetInstance().Load(stageName.c_str());

	// クリスタルの生成
	m_pCrystal = std::make_shared<Crystal>(std::stoi(info[5]));
	//m_pCrystal = std::make_shared<Crystal>(1);
	m_pCrystal->Init();
	m_pCrystal->Set(MapManager::GetInstance().GetCrystalPos());

	// ポーションの最大数を設定
	m_portionMax = std::stoi(info[7]);

	// スカイボックスの設定
	m_skyBoxHandle = ResourceManager::GetInstance().GetHandle("M_SKYBOX");
	MV1SetScale(m_skyBoxHandle, VGet(kSkyBoxScale, kSkyBoxScale, kSkyBoxScale));

	// スロット背景とアイコンの設定
	m_slotBgHandle = ResourceManager::GetInstance().GetHandle("I_SLOTBG");
	m_slotIconHandle.push_back(ResourceManager::GetInstance().GetHandle("I_SLOTCROSSBOW"));
	m_slotIconHandle.push_back(ResourceManager::GetInstance().GetHandle("I_SPIKE"));
	m_slotIconHandle.push_back(ResourceManager::GetInstance().GetHandle("I_SLOTARROWWALL"));
	m_slotIconHandle.push_back(ResourceManager::GetInstance().GetHandle("I_IRONUI"));
	m_slotIconHandle.push_back(ResourceManager::GetInstance().GetHandle("I_MINIMAPBG"));

	// HPバーのUIを設定
	m_pHpUi = std::make_shared<HPBar>();
	m_pHpUi->Init(m_pPlayer->GetHp());

	m_pMiniMap = std::make_shared<MiniMap>(stageIdx);
	m_pMiniMap->Init(ResourceManager::GetInstance().GetHandle(info[9]), MapManager::GetInstance().GetCrystalPos());

	// トラップポイントの初期値を設定
	m_initTrapPoint = std::stoi(info[4]);
	TrapManager::GetInstance().Load(stageName.c_str());
	TrapManager::GetInstance().SetUp(m_initTrapPoint);

	// 目標クリアタイムの設定
	ScoreManager::GetInstance().SetTargetClearTime(std::stoi(info[6]));

	m_stageName = info[8];

	m_operationHandle = ResourceManager::GetInstance().GetHandle("I_OPERATIONINGAME");

}

/// <summary>
/// 更新
/// </summary>
void GameManager::Update()
{
	SoundManager::GetInstance().PlayBGM("S_INGAMEBGM", true);

	// Yボタンを押した時かつ最初のフェーズの時
	if (Input::GetInstance().IsTriggered("Y") && m_phaseNum.front() == kInitialPhase)
	{
		// 次のフェーズに進む
		m_phaseNum.pop_front();

#ifdef _DEBUG
		if (m_phaseNum.front() == 0)
		{
			SoundManager::GetInstance().PlaySE("S_CLEAR");
		}
#endif
		//サウンドを流す
		SoundManager::GetInstance().PlaySE("S_ENEMY_APPEAR");
	}

	// クリスタルの更新
	m_pCrystal->Update();
	m_pCrystal->CheckCameraRayHit(m_pCamera->GetCameraPos(),m_pCamera->GetTargetPos());

	// カメラの更新
	m_pCamera->SetPlayerPos(m_pPlayer->GetPos());
	m_pCamera->Update();
	auto hitPos = m_pCamera->GetMapHitPosition();

	// 現在のフェーズが0以上(戦闘フェーズ)の時、敵を生成していなかったら敵を生成する
	if (m_phaseNum.front() >= 0)
	{
		m_pEnemyManager->CreateEnemy(m_phaseNum.front(), m_phaseCount);
	}
	// 0以下(準備フェーズ)の時
	else
	{
		// 敵生成フラグをリセットする
		m_isCreateEnemy = false;
	}

	// カメラの座標と向きを取得
	TrapManager::GetInstance().SetCameraInfo(m_pCamera->GetCameraPos(), m_pCamera->GetDirection());

	// 敵の更新処理
	auto isNextPhase = m_pEnemyManager->Update(m_phaseNum.front(), m_pCamera->GetCameraPos(), m_pCamera->GetDirection());

	// レイキャストの結果変数
	Vec3 rayCastRet;
	// カメラから敵までの距離
	auto cameraToEnemy = (m_pEnemyManager->GetRayCastRetPos() - m_pCamera->GetCameraPos()).Length();
	// カメラから地形までの距離
	auto cameraToMap = (m_pCamera->GetMapHitPosition() - m_pCamera->GetCameraPos()).Length();
	// もし、カメラから地形までの距離よりカメラから敵までの距離が大きかったら
	if (cameraToEnemy > cameraToMap)
	{
		// 近い方(地形とのヒット座標)を保存する
		rayCastRet = m_pCamera->GetMapHitPosition();
	}
	else
	{
		// 近い方(敵とのヒット座標)を保存するが、
		// そもそも敵と当たっているかどうかを判断する

		// 敵と当たっていたら
		if (m_pEnemyManager->GetRayCastRetPos().Length() != 0.0f)
		{
			// 敵とのヒット座標を保存する
			rayCastRet = m_pEnemyManager->GetRayCastRetPos();
		}
		// 当たっていなかったら
		else
		{
			// 地形とのヒット座標を保存する
			rayCastRet = m_pCamera->GetMapHitPosition();
		}
	}

	// ポーションの生成
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

	// プレイヤーの更新
	m_pPlayer->SetCameraAngle(m_pCamera->GetDirection());
	m_pPlayer->Update(this, rayCastRet);
	if (m_pPlayer->GetIsDeath())
	{
		// クリスタルの残りHPが5以上あればクリスタルHPを消費してプレイヤーを復活させる
		if (m_pCrystal->GetHp() > kPlayerReviveHpThreshold)
		{
			// クリスタルのHPを減らす
			m_pCrystal->PlayerDead();

			//復活SEを流す
			SoundManager::GetInstance().PlaySE("S_PLAYERREVIVAL");

			// プレイヤーの生成
			m_pPlayer = std::make_shared<Player>();
			m_pPlayer->Init(m_stageId);
			m_pHpUi->Init(m_pPlayer->GetHp());
		}
		// クリスタルの残りHPが5以下ならゲームオーバーにする
		else
		{
			m_isEnd = true;
			m_isClear = false;
			return;
		}
	}

	if (m_pPlayer->GetIsStartDeathAnim())
	{
		if (m_playerDeathCount > 220)
		{
			m_alpha = min(m_alpha + 26, 255);
		}

		m_playerDeathCount++;
	}
	else
	{
		m_playerDeathCount = 0;
		m_alpha = max(m_alpha - 26, 0);
	}

	// 敵が全滅した時、次のフェーズに進む
	if (isNextPhase)
	{
		// 念のため、現在が戦闘フェーズであるか確認
		if (m_phaseNum.front() > 0)
		{
			// フェーズカウントをリセット
			m_allPhaseCount += m_phaseCount;
			m_phaseCount = 0;

			auto addPoint = m_initTrapPoint / m_allPhaseNum * (m_phaseNum.front());
			TrapManager::GetInstance().AddTrapPoint(addPoint);

			// 次の準備フェーズに進む
			m_phaseNum.pop_front();

			if (m_phaseNum.front() == 0)
			{
				SoundManager::GetInstance().PlaySE("S_CLEAR");
			}
		}
	}

	// 最初の準備フレームでなければフェーズカウントを進める
	if (m_phaseNum.front() != kInitialPhase)
	{
		m_phaseCount++;
	}

	// 10秒経ったら次のフェーズに進める
	if (m_phaseNum.front() < 0 && m_phaseNum.front() != kInitialPhase && m_phaseCount >= kPhaseTransitionTime)
	{
		m_phaseNum.pop_front();
		// フェーズカウントをリセット
		m_allPhaseCount += m_phaseCount;
		m_phaseCount = 0;

		//サウンドを流す
		SoundManager::GetInstance().PlaySE("S_ENEMY_APPEAR");
	}

	// ポーションの更新
	for (auto& object : m_pObjects)
	{
		object->Update();

		if (!object->GetIsExist())
		{
			object->Finalize();
		}
	}
	// isExistがfalseのオブジェクトを削除
	{
		auto it = std::remove_if(m_pObjects.begin(), m_pObjects.end(), [](auto& v)
		{
			return v->GetIsExist() == false;
		});
		m_pObjects.erase(it, m_pObjects.end());
	}

	// トラップマネージャーの更新
	TrapManager::GetInstance().SetIsPrePhase(m_phaseNum.front() < 0);
	TrapManager::GetInstance().Update();

	// 物理更新
	MyLib::Physics::GetInstance().Update();

	// モデル座標の更新
	m_pPlayer->UpdateModelPos();
	m_pEnemyManager->UpdateModelPos();
	m_pHpUi->Update(m_pPlayer->GetHp());

	m_pMiniMap->Update(m_pPlayer->GetPos(),m_pEnemyManager->GetEnemyPos());

	// エフェクトの更新
	EffectManager::GetInstance().Update();

	// スカイボックスの位置をプレイヤーの位置に合わせる
	MV1SetPosition(m_skyBoxHandle, m_pPlayer->GetPos().ToVECTOR());

	// クリスタルのHPが0以下ならゲームオーバー
	if (m_pCrystal->GetHp() <= 0)
	{
		m_isClear = false;
		m_isEnd = true;
	}

	// フェーズが0ならゲームクリア
	if (m_phaseNum.front() == 0)
	{
		m_pPlayer->SetClearState();
		SoundManager::GetInstance().StopBGM("S_INGAMEBGM");


		if (m_pPlayer->GetNowAnimEndFrame() - 1 == m_pPlayer->GetAnimNowFrame())
		{
			// クリスタルの残りHPをスコア計算用に保存
			ScoreManager::GetInstance().SetCrystalHp(m_pCrystal->GetHp());
			ScoreManager::GetInstance().SetClearTime(m_allPhaseCount);
			ScoreManager::GetInstance().SetMaxComboNum(m_pEnemyManager->GetHighestKillStreakCount());
			m_pEnemyManager->SetScoreData();
			// スコアを計算
			ScoreManager::GetInstance().CalculationScore(m_stageName);
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
	// スカイボックスの描画
	MV1DrawModel(m_skyBoxHandle);

	// ステージの描画
	MapManager::GetInstance().Draw();

#ifdef _DEBUG	// デバッグ描画
	// ステージモデルのデバッグ描画
	// MV1DrawModel(m_stageModel);
	MyLib::DebugDraw::Draw3D();
#endif


	// プレイヤーの描画
	m_pPlayer->Draw();

	// 敵の描画
	m_pEnemyManager->Draw();

	// ポーションの描画
	for (auto& object : m_pObjects)
	{
		object->Draw();
	}

	// トラップの描画
	TrapManager::GetInstance().Draw();
	TrapManager::GetInstance().PreviewDraw();


	// クリスタルの描画
	m_pCrystal->Draw();
	m_pCrystal->DrawHP();

	// エフェクトの描画
	EffectManager::GetInstance().Draw();

	// TODO: UIクラスみたいなのを作ってそこに移動させる
	// 装備スロットの描画
	for (int i = 0; i < 1; i++)
	{
		int x = kSlotBgX + i * kSlotBgOffset;
		int y = kSlotBgY;
		DrawRotaGraph(x, y, kSlotBgScale, 0.0f, m_slotBgHandle, true);
		DrawRotaGraph(x, y, kSlotIconScale, 0.0f, m_slotIconHandle[i], true);
	}

	m_pMiniMap->Draw();


	if (Setting::GetInstance().GetIsDrawOperation())
	{
		// 操作説明UIの描画
		DrawUI::GetInstance().RegisterDrawRequest([=]()
		{
				DrawRotaGraph(Game::kWindowWidth - 150, Game::kWindowHeight - 130, 1.0f, 0.0f, m_operationHandle, true);
		}, 2);
	}


	// フェーズ番号の描画
	FontManager::GetInstance().DrawCenteredText(kPhaseNumX, kPhaseNumY - 12, "ウェーブ", 0xffffff, 24, 0x000000);
	FontManager::GetInstance().DrawCenteredText(kPhaseNumX, kPhaseNumY + 12, std::to_string(abs(m_phaseNum.front())) + "/" + std::to_string(m_allPhaseNum), 0xffffff, 24, 0x000000);


	// HPバーの描画
	m_pHpUi->Draw();

	// クロスヘアの描画
#if false
	auto centerX = Game::kWindowWidth / 2;
	auto centerY = Game::kWindowHeight / 2;
	DrawBox(centerX - kCrosshairWidth, centerY - kCrosshairHeight, centerX + kCrosshairWidth, centerY + kCrosshairHeight, 0xaaaaaa, true);
	DrawBox(centerX - kCrosshairHeight, centerY - kCrosshairWidth, centerX + kCrosshairHeight, centerY + kCrosshairWidth, 0xaaaaaa, true);
#endif

	auto offset = m_pPlayer->GetAttackIntervalCount() / 10;
	DrawRotaGraph(Game::kWindowWidth / 2, Game::kWindowHeight / 2, 0.5f, 0.0f, ResourceManager::GetInstance().GetHandle("I_CROSSHAIR"), true);
	DrawRotaGraph(Game::kWindowWidth / 2 + offset, Game::kWindowHeight / 2, 0.5f, 0.0f, ResourceManager::GetInstance().GetHandle("I_CROSSHAIRPARTS"), true);
	DrawRotaGraph(Game::kWindowWidth / 2, Game::kWindowHeight / 2 + offset, 0.5f, DX_PI/2, ResourceManager::GetInstance().GetHandle("I_CROSSHAIRPARTS"), true);
	DrawRotaGraph(Game::kWindowWidth / 2 - offset, Game::kWindowHeight / 2, 0.5f, DX_PI, ResourceManager::GetInstance().GetHandle("I_CROSSHAIRPARTS"), true);
	DrawRotaGraph(Game::kWindowWidth / 2, Game::kWindowHeight / 2 - offset, 0.5f, DX_PI/2*3, ResourceManager::GetInstance().GetHandle("I_CROSSHAIRPARTS"), true);

	// ゲーム開始メッセージの描画
	if (m_phaseNum.front() == kInitialPhase)
	{
		FontManager::GetInstance().DrawCenteredText(Game::kWindowWidth / 2, kStartMessageY, "準備ができたら", 0xffff4d, 24, 0x000000);
		FontManager::GetInstance().DrawLeftText(Game::kWindowWidth / 2 - 50, kStartMessageY + 36, "で迎え撃とう!", 0xffff4d, 24, 0x000000);
		DrawRotaGraph(Game::kWindowWidth / 2 - 80, kStartMessageY + 36, 0.5f, 0.0f, ResourceManager::GetInstance().GetHandle("I_Y"), true);
	}

	// 準備フェーズなら次のフェーズまでの時間を描画
	if (m_phaseNum.front() < 0 && m_phaseNum.front() != kInitialPhase)
	{
		FontManager::GetInstance().DrawCenteredText(Game::kWindowWidth / 2, kStartMessageY, "次のフェーズまで " + std::to_string((660 - m_phaseCount) / 60), 0xffffff, 24, 0x000000);
	}

#ifdef _DEBUG	// デバッグ描画
	// フェーズ番号のデバッグ描画
	DrawFormatString(kDebugPhaseNumX, kDebugPhaseNumY, 0xffffff, "フェーズ番号:%d", m_phaseNum.front());
	// 現在のフェーズの経過フレームのデバッグ描画
	DrawFormatString(kDebugPhaseCountX, kDebugPhaseCountY, 0xffffff, "現在のフェーズの経過フレーム:%d", m_phaseCount);
	// すべてのフェーズの経過フレームのデバッグ描画
	DrawFormatString(kDebugAllPhaseCountX, kDebugAllPhaseCountY, 0xffffff, "すべてのフェーズの経過フレーム:%d", m_allPhaseCount);
#endif

	// UIの描画
	DrawUI::GetInstance().Draw();

	//プレイヤー死亡時の暗転用
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_alpha);
	DrawBox(0, 0, Game::kWindowWidth, Game::kWindowHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

/// <summary>
/// オブジェクトを追加する
/// </summary>
void GameManager::AddObject(std::shared_ptr<ObjectBase> pAddObject)
{
	m_pObjects.emplace_back(pAddObject);
}