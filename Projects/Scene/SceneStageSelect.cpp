// Scene/SceneStageSelect.cpp
#include "SceneStageSelect.h"

#include "SceneGame.h"
#include "SceneTitle.h"

#include "MapManager.h"
#include "EffectManager.h"
#include "ResourceManager.h"
#include "FontManager.h"
#include "ScoreManager.h"
#include "SoundManager.h"
#include "Game.h"

namespace
{
	// テキスト描画のX座標
	constexpr int kTextX = 64;
	// テキスト描画のY座標
	constexpr int kTextY = 32;
	// テキスト描画のY座標の空白
	constexpr int kTextYInterval = 16;
	// カメラの初期位置
	const Vec3 kInitialCameraPos = Vec3(0.0f, 32.0f, -80.0f);
	// カメラのターゲット初期位置
	const Vec3 kInitialCameraTargetPos = Vec3(0.0f, 0.0f, 0.0f);
	// カメラのターゲットY座標の最大値
	constexpr float kMaxCameraTargetY = 20.0f;
	// カメラの移動速度
	constexpr float kCameraMoveSpeed = 2.4f;
	// カメラのターゲットY座標の減少量
	constexpr float kCameraTargetYDecrease = 4.0f;
	// カメラ移動距離の定数
	constexpr float kCameraMoveDistanceFactor = 80.0f;
}

SceneStageSelect::SceneStageSelect() :
	SceneBase("SCENE_STAGESELECT"),
	isNextScene(false),
	m_nowCursor(0),
	m_transitionFrameCount(0),
	m_cameraPos(kInitialCameraPos),
	m_cameraTarget(kInitialCameraTargetPos),
	m_cameraMoveDistance(0.0f),
	m_angle(0.0f)
{
}

SceneStageSelect::~SceneStageSelect()
{
	// staticクラスのデータのリセット
	ResourceManager::GetInstance().Clear(GetNowSceneName());
}

void SceneStageSelect::StartLoad()
{
	// リソースのロード開始処理

	// 非同期読み込みを開始する
	SetUseASyncLoadFlag(true);

	// リソースデータ群をみてリソースのロードを開始する
	ResourceManager::GetInstance().Load(GetNowSceneName());

	// デフォルトに戻す
	SetUseASyncLoadFlag(false);
}

bool SceneStageSelect::IsLoaded() const
{
	// リソースがロード中かどうかを判断する
	return ResourceManager::GetInstance().IsLoaded();
}

void SceneStageSelect::Init()
{
	// ステージ名のロード
	m_stageNames = LoadCSV::GetInstance().GetAllStageName();

	for (int i = 0; i < m_stageNames.size(); i++)
	{
		auto info = LoadCSV::GetInstance().LoadStageInfo(i);
		m_stageMinimapHandle.push_back(ResourceManager::GetInstance().GetHandle(info[9]));
	}

	// マップマネージャーの初期化とロード
	MapManager::GetInstance().Init();
	MapManager::GetInstance().Load("StageSelect");

	// カメラの初期化
	SetCameraPositionAndTarget_UpVecY(m_cameraPos.ToVECTOR(), m_cameraTarget.ToVECTOR());

	// リソースハンドルの取得
	m_bigWindowHandle = ResourceManager::GetInstance().GetHandle("I_BIGWINDOW");
	m_smallWindowHandle = ResourceManager::GetInstance().GetHandle("I_SMALLWINDOW");
}

void SceneStageSelect::End()
{
	// エフェクトの停止
	EffectManager::GetInstance().AllStopEffect();
}

void SceneStageSelect::Update()
{
	SoundManager::GetInstance().PlayBGM("S_TITLEBGM", true);

	// カメラのターゲットY座標の更新
	if (IsLoaded() && m_cameraTarget.y < kMaxCameraTargetY)
	{
		m_cameraTarget.y += 1.0f;
		SetCameraPositionAndTarget_UpVecY(m_cameraPos.ToVECTOR(), m_cameraTarget.ToVECTOR());
	}

	// 次のシーンへの遷移処理
	if (isNextScene)
	{

		if (m_nowCursor >= 0)
		{
			auto vec = m_cameraTarget - m_cameraPos;
			vec = vec.Normalize() * kCameraMoveSpeed;
			m_cameraPos += vec;
			SetCameraPositionAndTarget_UpVecY(m_cameraPos.ToVECTOR(), m_cameraTarget.ToVECTOR());

			m_cameraMoveDistance += vec.Length();

			SoundManager::GetInstance().FadeOutBGM("S_TITLEBGM", 10);
		}
		else
		{
			m_cameraTarget.y -= kCameraTargetYDecrease;
			SetCameraPositionAndTarget_UpVecY(m_cameraPos.ToVECTOR(), m_cameraTarget.ToVECTOR());
		}

		// 遷移フレームカウントの更新
		m_transitionFrameCount++;
	}

	// エフェクトの更新
	EffectManager::GetInstance().Update();

	// UIの角度の更新
	m_angle += 0.05f;
}

void SceneStageSelect::Draw()
{
	// リソースのロードが終わるまでは描画しない
	if (!IsLoaded() || !IsInitialized()) return;

	// ステージの描画
	MapManager::GetInstance().Draw();

	// エフェクトの描画
	EffectManager::GetInstance().Draw();

#ifdef _DEBUG    // デバッグ描画
	DrawFormatString(0, 0, 0xffffff, "%s", GetNowSceneName());

	// カーソルの描画
	DrawString(kTextX - 24, kTextY + kTextYInterval * m_nowCursor, "→", 0xff0000);

	// ステージ名の描画
	for (int i = 0; i < m_stageNames.size(); i++)
	{
		DrawFormatString(kTextX, kTextY + kTextYInterval * i, 0xffffff, "%s", m_stageNames[i].c_str());
	}
#endif



	auto addSize = sinf(m_angle) / 16;

	DrawRotaGraph(1000 + m_cameraMoveDistance * 10, 386, 1.0f + m_cameraMoveDistance / kCameraMoveDistanceFactor, 0.0f, m_bigWindowHandle, true);
		if (m_nowCursor >= 0)
	{
		FontManager::GetInstance().DrawCenteredExtendText(1000 + m_cameraMoveDistance * 10, 540 + m_cameraMoveDistance * 2, "ハイスコア:" + std::to_string(ScoreManager::GetInstance().GetScore(m_stageNames[m_nowCursor])), 0xffffff, 48, 0x000000, 1.0f + m_cameraMoveDistance / kCameraMoveDistanceFactor);
	}

	for (int i = 0; i < 3; i++)
	{
		float graphSize = 1.0f;
		int fontSize = 40;
		float fontExtendRate = 1.0f;
		if (i == m_nowCursor)
		{
			graphSize = 1.2f + addSize;
			fontSize = 48;
			fontExtendRate += addSize;
			DrawRotaGraph(1000 + m_cameraMoveDistance * 10, 346, 1.2f + m_cameraMoveDistance / kCameraMoveDistanceFactor, 0.0f, m_stageMinimapHandle[m_nowCursor], true);
		}
		DrawRotaGraph(300 - m_cameraMoveDistance * 10, 220 + 165 * i + m_cameraMoveDistance * 2 * (i - 1), graphSize + m_cameraMoveDistance / kCameraMoveDistanceFactor, 0.0f, m_smallWindowHandle, true);
		FontManager::GetInstance().DrawCenteredExtendText(300 - m_cameraMoveDistance * 10, 220 + 165 * i - 5 + m_cameraMoveDistance * 2 * (i - 1), m_stageNames[i], 0xffffff, fontSize, 0x000000, fontExtendRate + m_cameraMoveDistance / kCameraMoveDistanceFactor);
	}

	DrawRotaGraph(36, Game::kWindowHeight - 36, 0.5f, 0.0f, ResourceManager::GetInstance().GetHandle("I_B"), true);
	FontManager::GetInstance().DrawLeftText(36 + 24, Game::kWindowHeight - 36 - 6, ":戻る", 0xffffff, 32, 0xffffff);
}

void SceneStageSelect::SelectNextSceneUpdate()
{
	// 上キーが押された場合の処理
	if (Input::GetInstance().IsTriggered("UP"))
	{
		m_nowCursor = max(0, m_nowCursor - 1);
		//SEを流す
		SoundManager::GetInstance().PlaySE("S_CURSORMOVE");
	}

	// 下キーが押された場合の処理
	if (Input::GetInstance().IsTriggered("DOWN"))
	{
		m_nowCursor = min(static_cast<int>(m_stageNames.size()) - 1, m_nowCursor + 1);
		//SEを流す
		SoundManager::GetInstance().PlaySE("S_CURSORMOVE");

	}

	// OKキーが押された場合の処理
	if (Input::GetInstance().IsTriggered("OK"))
	{
		//SEを流す
		SoundManager::GetInstance().PlaySE("S_CURSORENTER");

		isNextScene = true;
		m_transitionFrameCount = 0; // 遷移フレームカウントのリセット
		SceneManager::GetInstance().SetStageIdx(m_nowCursor);
		SceneManager::GetInstance().SetNextScene(std::make_shared<SceneGame>());
		EndThisScene();
		return;
	}

	// キャンセルキーが押された場合の処理
	if (Input::GetInstance().IsTriggered("CANCEL"))
	{
		m_nowCursor = -1;
		isNextScene = true;
		m_transitionFrameCount = 0; // 遷移フレームカウントのリセット
		SceneManager::GetInstance().SetNextScene(std::make_shared<SceneTitle>());
		EndThisScene();
		return;
	}
}