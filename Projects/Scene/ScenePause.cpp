#include "ScenePause.h"
#include "Game.h"

#include "SceneGame.h"
#include "SceneOption.h"
#include "SceneStageSelect.h"

#include "FontManager.h"
#include "SoundManager.h"

#include <vector>

namespace
{
//#ifdef _DEBUG	//デバッグ描画
	/*テキスト描画関係*/
	constexpr int kTextX = 64;			//テキスト描画X座標
	constexpr int kTextY = 32;			//テキスト描画Y座標
	constexpr int kTextYInterval = 16;	//テキスト描画Y座標の空白
//#endif

	const std::vector<std::string> kItemText =
	{
		"ゲームに戻る",
		"リスタート",
		"オプション",
		"ステージセレクトに戻る"
	};
}

/// <summary>
/// コンストラクタ
/// </summary>
ScenePause::ScenePause():
	SceneBase("SCENE_PAUSE"),
	m_angle(0.0f)
{
}

/// <summary>
/// デストラクタ
/// </summary>
ScenePause::~ScenePause()
{
}

/// <summary>
/// //リソースのロード開始
/// </summary>
void ScenePause::StartLoad()
{
	//何もしない
}

/// <summary>
/// リソースのロードが終了したかどうか
/// </summary>
bool ScenePause::IsLoaded() const
{
	//このシーンはロード関係をInitでする
	return true;
}

/// <summary>
/// 初期化
/// </summary>
void ScenePause::Init()
{
	//フェードアウトをスキップする
	SkipFadeOut();

	m_destinationScene = static_cast<eDestination>(static_cast<int>(eDestination::Start) + 1);
}

/// <summary>
/// 終了
/// </summary>
void ScenePause::End()
{
}

/// <summary>
/// 更新
/// </summary>
void ScenePause::Update()
{
	m_angle += 0.05f;
}

/// <summary>
/// 描画
/// </summary>
void ScenePause::Draw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
	DrawBox(0 , 0 , Game::kWindowWidth , Game::kWindowHeight , 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawBox(0 , 0 , Game::kWindowWidth , Game::kWindowHeight , 0xffffff, false);

	auto addSize = sinf(m_angle) / 16;

	FontManager::GetInstance().DrawCenteredText(Game::kWindowWidth / 2, Game::kWindowHeight / 6, "ポーズ", 0xffffff, 64, 0x000000);

	for (int i = 0; i < kItemText.size(); i++)
	{
		float rate = 1.0f;
		if (m_destinationScene - 1 == i)
		{
			rate += addSize;
		}
		FontManager::GetInstance().DrawCenteredExtendText(Game::kWindowWidth / 2, Game::kWindowHeight / 2 + 80 * i, kItemText[i], 0xffffff, 40, 0x000000, rate);
	}

#ifdef _DEBUG	//デバッグ描画
	DrawFormatString(0, 16, 0xffffff, "%s", GetNowSceneName());

	DrawString(kTextX - 24, kTextY + kTextYInterval * (m_destinationScene - 1), "→", 0xff0000);

	DrawString(kTextX, kTextY, "ゲームに戻る", 0xffffff);
	DrawString(kTextX, kTextY + kTextYInterval, "リスタート", 0xffffff);
	DrawString(kTextX, kTextY + kTextYInterval * 2, "オプション", 0xffffff);
	DrawString(kTextX, kTextY + kTextYInterval * 3, "セレクトに戻る", 0xffffff);
#endif
}

/// <summary>
/// 次のシーンを選択する更新処理
/// </summary>
void ScenePause::SelectNextSceneUpdate()
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
		else
		{
			//SEを流す
			SoundManager::GetInstance().PlaySE("S_CURSORMOVE");
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

		//タイトルシーンに遷移する
		if (m_destinationScene == eDestination::InGame)
		{
			SceneManager::GetInstance().PopScene();
			SkipFadeIn();
			return;
		}
		//ゲームシーンに遷移する
		else if (m_destinationScene == eDestination::ReStart)
		{
			SceneManager::GetInstance().SetNextScene(std::make_shared<SceneGame>());
			EndThisScene();
			return;
		}
		//ゲームシーンに遷移する
		else if (m_destinationScene == eDestination::Option)
		{
			SceneManager::GetInstance().PushScene(std::make_shared<SceneOption>());
			//EndThisScene(true);
			return;
		}
		//ポーズシーンを上に表示する
		else if (m_destinationScene == eDestination::StageSelect)
		{
			SceneManager::GetInstance().SetNextScene(std::make_shared<SceneStageSelect>());
			EndThisScene();
			return;
		}
	}

	if (Input::GetInstance().IsTriggered("PAUSE"))
	{
		//タイトルシーンに遷移する
		if (m_destinationScene == eDestination::InGame)
		{
			SceneManager::GetInstance().PopScene();
			SkipFadeIn();
			return;
		}
	}
}
