#include "SceneDebug.h"

#include "SceneTitle.h"
#include "SceneMainMenu.h"
#include "SceneStageSelect.h"
#include "SceneStrengthen.h"
#include "SceneRanking.h"
#include "SceneGame.h"
#include "SceneResult.h"
#include "ScenePause.h"
#include "SceneOption.h"

#include "SoundManager.h"
#include "LoadCSV.h"

namespace
{
	const std::string kSceneName[] =
	{
		"SceneTitle",
		"SceneStageSelect",
		"SceneGame",
		"SceneResult",
		"ScenePause",
		"SceneOption",

		"SceneMainMenu",
		"SceneRanking",
		"SceneStrengthen",
	};

	/*テキスト描画関係*/
	constexpr int kTextX = 64;			//テキスト描画X座標
	constexpr int kTextY = 32;			//テキスト描画Y座標
	constexpr int kTextYInterval = 16;	//テキスト描画Y座標の空白
}

/// <summary>
/// コンストラクタ
/// </summary>
SceneDebug::SceneDebug() :
	SceneBase("SCENE_DEBUG"),
	m_selectingStageIdx(0)
{
}

/// <summary>
/// デストラクタ
/// </summary>
SceneDebug::~SceneDebug()
{
}

/// <summary>
/// //リソースのロード開始
/// </summary>
void SceneDebug::StartLoad()
{
	//このシーンでロードするべきリソースのパスを取得
	auto loadResourceData = LoadCSV::GetInstance().GetLoadResourcePath(GetNowSceneName());

	// 非同期読み込みを開始する
	SetUseASyncLoadFlag(true);

	////リソースデータ群をみてリソースのロードを開始する
	//AssortAndLoadResourse(loadResourceData);

	// デフォルトに戻す
	SetUseASyncLoadFlag(false);
}

/// <summary>
/// リソースのロードが終了したかどうか
/// </summary>
bool SceneDebug::IsLoaded() const
{
	return true;
}

/// <summary>
/// 初期化
/// </summary>
void SceneDebug::Init()
{
	m_destinationScene = static_cast<eDestination>(static_cast<int>(eDestination::Start) + 1);

	m_stageNames = LoadCSV::GetInstance().GetAllStageName();

	//通常状態に設定しておく
	m_updateFunc = &SceneDebug::UpdateItemSelect;
	m_drawFunc = &SceneDebug::DrawNormal;

	// シーン遷移のマップを初期化
	m_sceneTransitionMap = {
		{eDestination::Title, []() { return std::make_shared<SceneTitle>(); }},
		{eDestination::Select, []() { return std::make_shared<SceneMainMenu>(); }},
		{eDestination::StageSelect, []() { return std::make_shared<SceneStageSelect>(); }},
		{eDestination::Strengthen, []() { return std::make_shared<SceneStrengthen>(); }},
		{eDestination::Ranking, []() { return std::make_shared<SceneRanking>(); }},
		{eDestination::InGame, [this]() { 
			m_updateFunc = &SceneDebug::UpdateStageSelect;
			m_drawFunc = &SceneDebug::DrawStageName;
			return nullptr; 
		}},
		{eDestination::Result, []() { return std::make_shared<SceneResult>(); }},
		{eDestination::Pause, []() { return std::make_shared<ScenePause>(); }},
		{eDestination::Option, []() { return std::make_shared<SceneOption>(); }},
	};
}

/// <summary>
/// 終了
/// </summary>
void SceneDebug::End()
{
}

/// <summary>
/// 更新
/// </summary>
void SceneDebug::Update()
{
}

/// <summary>
/// 描画
/// </summary>
void SceneDebug::Draw()
{
	int y = kTextY;
	for (auto& name : kSceneName)
	{
		DrawString(kTextX, y, name.c_str(), 0xffffff);
		y += kTextYInterval;
	}

	//状態の更新
	(this->*m_drawFunc)();

#ifdef _DEBUG	//デバッグ描画
	DrawFormatString(0, 0, 0xffffff, "%s", GetNowSceneName());
#endif
}

/// <summary>
/// 次のシーンを選択する更新処理
/// </summary>
void SceneDebug::SelectNextSceneUpdate()
{
	//状態の更新
	(this->*m_updateFunc)();
}

void SceneDebug::UpdateItemSelect()
{
	//上を入力したら
	if (Input::GetInstance().IsTriggered("UP"))
	{
		//現在選択している項目から一個上にずらす
		m_destinationScene = static_cast<eDestination>(static_cast<int>(m_destinationScene) - 1);

		//もし一番上の項目を選択している状態になっていたら
		if (m_destinationScene == eDestination::Start)
		{
			//一個下にずらす
			m_destinationScene = static_cast<eDestination>(static_cast<int>(m_destinationScene) + 1);
		}
	}

	//下を入力したら
	if (Input::GetInstance().IsTriggered("DOWN"))
	{
		//現在選択している項目から一個下にずらす
		m_destinationScene = static_cast<eDestination>(static_cast<int>(m_destinationScene) + 1);

		//もし一番下の項目を選択している状態になっていたら
		if (m_destinationScene == eDestination::Last)
		{
			//一個上にずらす
			m_destinationScene = static_cast<eDestination>(static_cast<int>(m_destinationScene) - 1);
		}
	}

	//決定ボタンを押したら現在選択しているシーンに遷移する
	if (Input::GetInstance().IsTriggered("OK"))
	{
		auto it = m_sceneTransitionMap.find(m_destinationScene);
		if (it != m_sceneTransitionMap.end())
		{
			auto nextScene = it->second();
			if (nextScene)
			{
				SceneManager::GetInstance().SetNextScene(nextScene);
				EndThisScene();
			}
		}
	}
}

void SceneDebug::UpdateStageSelect()
{
	//上を入力したら
	if (Input::GetInstance().IsTriggered("UP"))
	{
		//現在選択している項目から一個上にずらす
		m_selectingStageIdx--;

		//もし一番上の項目を選択している状態になっていたら
		if (m_selectingStageIdx < 0)
		{
			//一個下にずらす
			m_selectingStageIdx++;
		}
	}

	//下を入力したら
	if (Input::GetInstance().IsTriggered("DOWN"))
	{
		//現在選択している項目から一個下にずらす
		m_selectingStageIdx++;

		//もし一番下の項目を選択している状態になっていたら
		if (m_selectingStageIdx == m_stageNames.size())
		{
			//一個上にずらす
			m_selectingStageIdx--;
		}
	}

	//キャンセルボタンを押したら項目を選ぶ状態に戻る
	if (Input::GetInstance().IsTriggered("CANCEL"))
	{
		m_updateFunc = &SceneDebug::UpdateItemSelect;
		m_drawFunc = &SceneDebug::DrawNormal;
	}

	//決定ボタンを押したら現在選択しているシーンに遷移する
	if (Input::GetInstance().IsTriggered("OK"))
	{
		SceneManager::GetInstance().SetStageIdx(m_selectingStageIdx);
		SceneManager::GetInstance().SetNextScene(std::make_shared<SceneGame>());
		EndThisScene();
		return;
	}
}

void SceneDebug::DrawNormal()
{
	DrawString(kTextX - 24, kTextY + kTextYInterval * (m_destinationScene - 1), "→", 0xff0000);
}

void SceneDebug::DrawStageName()
{
	DrawString(kTextX * 4 - 24, kTextY + kTextYInterval * m_selectingStageIdx, "→", 0xff0000);

	for (int i = 0; i < m_stageNames.size(); i++)
	{
		DrawFormatString(kTextX * 4, kTextY + kTextYInterval * i, 0xffffff, "%s", m_stageNames[i].c_str());
	}
}