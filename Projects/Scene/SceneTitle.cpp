#include "SceneTitle.h"
#include "SceneStageSelect.h"
#include "SceneOption.h"

#include "Crystal.h"
#include "EnemyManager.h"
#include "EffectManager.h"
#include "Physics.h"

#include "Game.h"

#include "ResourceManager.h"
#include "FontManager.h"
#include "MapManager.h"
#include "SoundManager.h"

namespace
{
//#ifdef _DEBUG	//デバッグ描画
	/*テキスト描画関係*/
	constexpr int kTextX = 64;			//テキスト描画X座標
	constexpr int kTextY = 32;			//テキスト描画Y座標
	constexpr int kTextYInterval = 16;	//テキスト描画Y座標の空白
//#endif

	//ロゴ描画関係
	constexpr float kRogoSize = 0.55f;	//サイズ

	const char* kItemName[] = { "オプション","スタート","やめる" };
}

/// <summary>
/// コンストラクタ
/// </summary>
SceneTitle::SceneTitle():
	SceneBase("SCENE_TITLE"),
	isNextScene(false),
	m_enemyCreateFrame(0),
	m_angle(0.0f)
{

}

/// <summary>
/// デストラクタ
/// </summary>
SceneTitle::~SceneTitle()
{
	//staticクラスのデータのリセット
	ResourceManager::GetInstance().Clear(GetNowSceneName());
}

/// <summary>
/// //リソースのロード開始
/// </summary>
void SceneTitle::StartLoad()
{
	// 非同期読み込みを開始する
	SetUseASyncLoadFlag(true);

	//リソースデータ群をみてリソースのロードを開始する
	ResourceManager::GetInstance().Load(GetNowSceneName());

	// デフォルトに戻す
	SetUseASyncLoadFlag(false);
}

/// <summary>
/// リソースのロードが終了したかどうか
/// </summary>
bool SceneTitle::IsLoaded() const
{
	//TODO:ここでリソースがロード中かどうかを判断する
	if (!ResourceManager::GetInstance().IsLoaded())	return false;

	return true;
}

/// <summary>
/// 初期化
/// </summary>
void SceneTitle::Init()
{
	//TODO:ここで実態の生成などをする
	m_destinationScene = static_cast<eDestination>(static_cast<int>(eDestination::Start) + 2);

	m_rogoHandle = ResourceManager::GetInstance().GetHandle("I_ROGO");

	// ステージ情報をロード
	MapManager::GetInstance().Init();
	MapManager::GetInstance().Load("title");

	m_pCrystal = std::make_shared<Crystal>(999999);
	m_pCrystal->Init();
	m_pCrystal->Set(MapManager::GetInstance().GetCrystalPos());

	// ステージの当たり判定を設定
	MyLib::Physics::GetInstance().SetStageCollisionModel("M_STAGECOLLISIONTITLE", "M_STAGECOLLISIONTITLE");

	// 敵管理クラスの生成
	m_pEnemyManager = std::make_shared<EnemyManager>(false);
	m_pEnemyManager->LoadWayPoint("title");

	SetCameraNearFar(1.0f, 140.0f);
	m_cameraTarget = Vec3(0.0f, 20.0f, 0.0f);
	SetCameraPositionAndTarget_UpVecY(VGet(0.0f, 32.0f, -80.0f), m_cameraTarget.ToVECTOR());
	m_lightHandle = CreateDirLightHandle(VSub(VGet(10.0f, 30.0f, 0.0f), m_cameraTarget.ToVECTOR()));

	m_buttonHandle = ResourceManager::GetInstance().GetHandle("I_BUTTON");

}

/// <summary>
/// 終了
/// </summary>
void SceneTitle::End()
{
	m_pEnemyManager->Finalize();
	m_pCrystal->Finalize();
	MyLib::Physics::GetInstance().Clear();
	EffectManager::GetInstance().AllStopEffect();

	//TODO:ここでリソースのメモリ開放などをする
	DeleteLightHandle(m_lightHandle);
}

/// <summary>
/// 更新
/// </summary>
void SceneTitle::Update()
{
	if (!IsLoaded())	return;

	SoundManager::GetInstance().PlayBGM("S_TITLEBGM", true);

	m_pCrystal->Update();
	m_pEnemyManager->Update(0, Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f));
	m_pEnemyManager->CreateEnemy(0, m_enemyCreateFrame,false);

	// 物理更新
	MyLib::Physics::GetInstance().Update();
	m_pEnemyManager->UpdateModelPos();

	//つぎのシーンに遷移するときにカメラの向きを変える
	if (isNextScene)
	{
		m_cameraTarget.y -= 4.0f;
		SetCameraPositionAndTarget_UpVecY(VGet(0.0f, 32.0f, -80.0f), m_cameraTarget.ToVECTOR());
	}

	// エフェクトの更新
	EffectManager::GetInstance().Update();

	m_enemyCreateFrame++;

	m_angle += 0.05f;
}

/// <summary>
/// 描画
/// </summary>
void SceneTitle::Draw()
{
	// リソースのロードが終わるまでは描画しないのがよさそう
	// (どちらにしろフェード仕切っているので何も見えないはず)
	if (!IsLoaded())	return;
	if (!IsInitialized())	return;

	// ステージの描画
	MapManager::GetInstance().Draw();
	m_pCrystal->Draw();
	m_pEnemyManager->Draw();

	// エフェクトの描画
	EffectManager::GetInstance().Draw();

#ifdef _DEBUG	//デバッグ描画
	DrawFormatString(0, 0, 0xffffff, "%s", GetNowSceneName());
	DrawString(kTextX, kTextY, "ゲームを始める", 0xffffff);
	DrawString(kTextX, kTextY + kTextYInterval, "オプション", 0xffffff);
	DrawString(kTextX, kTextY + kTextYInterval * 2, "やめる", 0xffffff);
	DrawString(kTextX - 24, kTextY + kTextYInterval * (m_destinationScene - 1), "→", 0xff0000);
#endif


	auto addSize = sinf(m_angle) / 16;

	for (int i = 0; i < 3; i++)
	{
		float graphSize = 1.0f;
		int fontSize = 40;
		float fontExtendRate = 1.0f;
		if (i == m_destinationScene-1)
		{
			graphSize = 1.2f + addSize;
			fontSize = 48;
			fontExtendRate += addSize;
		}
		DrawRotaGraph(Game::kWindowWidth / 5 + 378 * i, Game::kWindowHeight / 2 + 260, graphSize, 0.0f, m_buttonHandle, true);
		FontManager::GetInstance().DrawCenteredExtendText(Game::kWindowWidth / 5 + 378 * i, Game::kWindowHeight / 2 + 260 -5, kItemName[i], 0xffffff, fontSize, 0x000000, fontExtendRate);
	}

	//DrawRotaGraph(Game::kWindowWidth / 5 + 378 * (m_destinationScene - 1), Game::kWindowHeight / 2 + 260 + 40, 1.0f, 0.0f, ResourceManager::GetInstance().GetHandle("I_UIA"), true);

	//ロゴの描画
	DrawRotaGraph(Game::kWindowWidth / 2, Game::kWindowHeight / 4, kRogoSize, 0.0f, m_rogoHandle, true);
}

/// <summary>
/// 次のシーンを選択する更新処理
/// </summary>
void SceneTitle::SelectNextSceneUpdate()
{
	//上を入力したら
	if (Input::GetInstance().IsTriggered("LEFT"))
	{
		//現在選択している項目から一個上にずらす
		m_destinationScene = static_cast<eDestination>(static_cast<int>(m_destinationScene) - 1);

		//もし一番上の項目を選択している状態になっていたら
		if (m_destinationScene == eDestination::Start)
		{
			//一個下にずらす
			m_destinationScene = static_cast<eDestination>(static_cast<int>(m_destinationScene) + 1);
		}
		else
		{
			//SEを流す
			SoundManager::GetInstance().PlaySE("S_CURSORMOVE");
		}
	}

	//下を入力したら
	if (Input::GetInstance().IsTriggered("RIGHT"))
	{
		//現在選択している項目から一個下にずらす
		m_destinationScene = static_cast<eDestination>(static_cast<int>(m_destinationScene) + 1);

		//もし一番下の項目を選択している状態になっていたら
		if (m_destinationScene == eDestination::Last)
		{
			//一個上にずらす
			m_destinationScene = static_cast<eDestination>(static_cast<int>(m_destinationScene) - 1);
		}
		else
		{
			//SEを流す
			SoundManager::GetInstance().PlaySE("S_CURSORMOVE");
		}
	}

	//決定ボタンを押したら現在選択しているシーンに遷移する
	if (Input::GetInstance().IsTriggered("OK"))
	{
		//SEを流す
		SoundManager::GetInstance().PlaySE("S_CURSORENTER");

		//ゲームシーンに遷移する
		if (m_destinationScene == eDestination::Select)
		{
			isNextScene = true;
			SceneManager::GetInstance().SetNextScene(std::make_shared<SceneStageSelect>());
			EndThisScene();
			return;
		}
		else if (m_destinationScene == eDestination::Option)
		{
			SceneManager::GetInstance().PushScene(std::make_shared<SceneOption>());
			//EndThisScene(true);
			return;
		}
		else if (m_destinationScene == eDestination::Quit)
		{
			SetIsGameEnd();
			return;
		}
	}
}
