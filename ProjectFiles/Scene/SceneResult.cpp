#include "SceneResult.h"

#include "SceneGame.h"
#include "SceneStrengthen.h"
#include "SceneRanking.h"
#include "SceneMainMenu.h"

#include "ScoreManager.h"

#include "Game.h"
namespace
{
//#ifdef _DEBUG	//デバッグ描画
	/*テキスト描画関係*/
	constexpr int kTextX = 64;			//テキスト描画X座標
	constexpr int kTextY = 32;			//テキスト描画Y座標
	constexpr int kTextYInterval = 16;	//テキスト描画Y座標の空白
//#endif
}

/// <summary>
/// コンストラクタ
/// </summary>
SceneResult::SceneResult():
	SceneBase("SCENE_RESULT"),
	m_isClear(false)
{
}

/// <summary>
/// デストラクタ
/// </summary>
SceneResult::~SceneResult()
{
}

/// <summary>
/// //リソースのロード開始
/// </summary>
void SceneResult::StartLoad()
{
}

/// <summary>
/// リソースのロードが終了したかどうか
/// </summary>
bool SceneResult::IsLoaded() const
{
	return true;
}

/// <summary>
/// 初期化
/// </summary>
void SceneResult::Init()
{
	//フェードアウトをスキップする
	SkipFadeOut();
	//最初は一番上の項目を選んでいる状態にする
	m_destinationScene = static_cast<eDestination>(static_cast<int>(eDestination::Start) + 1);
}

/// <summary>
/// 終了
/// </summary>
void SceneResult::End()
{
	ScoreManager::GetInstance().Clear();
}

/// <summary>
/// 更新
/// </summary>
void SceneResult::Update()
{
}

/// <summary>
/// 描画
/// </summary>
void SceneResult::Draw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
	DrawBox(0 + 20, 0 + 20, Game::kWindowWidth - 20, Game::kWindowHeight - 20, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawBox(0 + 20, 0 + 20, Game::kWindowWidth - 20, Game::kWindowHeight - 20, 0xffffff, false);

	if (m_isClear)
	{
		DrawString(240, 224,"クリア!",0xffffff);
	}
	else
	{
		DrawString(240, 224, "ゲームオーバー...", 0xffffff);
	}

	DrawFormatString(240,240,0xffffff,"Score:%d",ScoreManager::GetInstance().GetScore());

#ifdef _DEBUG	//デバッグ描画	
	DrawFormatString(0, 0, 0xffffff, "%s", GetNowSceneName());
#endif

	DrawString(kTextX - 24, kTextY + kTextYInterval * (m_destinationScene - 1), "→", 0xff0000);

	DrawString(kTextX, kTextY, "次のステージへ", 0xffffff);
	DrawString(kTextX, kTextY + kTextYInterval, "スコア詳細へ", 0xffffff);
	DrawString(kTextX, kTextY + kTextYInterval * 2, "強化へ", 0xffffff);
	DrawString(kTextX, kTextY + kTextYInterval * 3, "ランキングへ", 0xffffff);
	DrawString(kTextX, kTextY + kTextYInterval * 4, "メインメニューに戻る", 0xffffff);
}

/// <summary>
/// 次のシーンを選択する更新処理
/// </summary>
void SceneResult::SelectNextSceneUpdate()
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
		//ゲームシーンに遷移する
		if (m_destinationScene == eDestination::InGame)
		{
			SceneManager::GetInstance().SetNextScene(std::make_shared<SceneGame>());
			SceneManager::GetInstance().SetStageIdx(SceneManager::GetInstance().GetStageIdx());
			EndThisScene();
			return;
		}
		//スコア詳細が出てくる
		else if (m_destinationScene == eDestination::ScoreDetail)
		{

		}
		//強化シーンに遷移する
		else if (m_destinationScene == eDestination::Strengthen)
		{
			SceneManager::GetInstance().SetNextScene(std::make_shared<SceneStrengthen>());
			EndThisScene(true);
			return;
		}
		//ランキングシーンに遷移する
		else if (m_destinationScene == eDestination::Ranking)
		{
			SceneManager::GetInstance().SetNextScene(std::make_shared<SceneRanking>());
			EndThisScene(true);
			return;
		}
		//セレクトシーンに遷移する
		else if (m_destinationScene == eDestination::Select)
		{
			SceneManager::GetInstance().SetNextScene(std::make_shared<SceneMainMenu>());
			EndThisScene();
			return;
		}
	}
}
