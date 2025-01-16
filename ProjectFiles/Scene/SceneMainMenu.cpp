#include "SceneMainMenu.h"

#include "SceneTitle.h"
#include "SceneStageSelect.h"
#include "SceneStrengthen.h"
#include "SceneOption.h"
#include "SceneRanking.h"

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
SceneMainMenu::SceneMainMenu() :
	SceneBase("SCENE_MAINMENU")
{
}

/// <summary>
/// デストラクタ
/// </summary>
SceneMainMenu::~SceneMainMenu()
{
}

/// <summary>
/// //リソースのロード開始
/// </summary>
void SceneMainMenu::StartLoad()
{
}

/// <summary>
/// リソースのロードが終了したかどうか
/// </summary>
bool SceneMainMenu::IsLoaded() const
{
	return true;
}

/// <summary>
/// 初期化
/// </summary>
void SceneMainMenu::Init()
{
	m_destinationScene = static_cast<eDestination>(static_cast<int>(eDestination::Start) + 1);
}

/// <summary>
/// 終了
/// </summary>
void SceneMainMenu::End()
{
}

/// <summary>
/// 更新
/// </summary>
void SceneMainMenu::Update()
{
}

/// <summary>
/// 描画
/// </summary>
void SceneMainMenu::Draw()
{
#ifdef _DEBUG	//デバッグ描画
	DrawFormatString(0, 0, 0xffffff, "%s", GetNowSceneName());
#endif


	DrawString(kTextX - 24, kTextY + kTextYInterval * (m_destinationScene - 1), "→", 0xff0000);

	DrawString(kTextX, kTextY, "ステージセレクト", 0xffffff);
	DrawString(kTextX, kTextY + kTextYInterval, "強化", 0xffffff);
	DrawString(kTextX, kTextY + kTextYInterval * 2, "オプション", 0xffffff);
	DrawString(kTextX, kTextY + kTextYInterval * 3, "ランキング", 0xffffff);
	DrawString(kTextX, kTextY + kTextYInterval * 4, "やめる", 0xffffff);
}

/// <summary>
/// 次のシーンを選択する更新処理
/// </summary>
void SceneMainMenu::SelectNextSceneUpdate()
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
		if (m_destinationScene == eDestination::StageSelect)
		{
			SceneManager::GetInstance().SetNextScene(std::make_shared<SceneStageSelect>());
			EndThisScene();
			return;
		}
		else if(m_destinationScene == eDestination::Strengthen)
		{
			SceneManager::GetInstance().SetNextScene(std::make_shared<SceneStrengthen>());
			EndThisScene(true);
			return;
		}
		else if (m_destinationScene == eDestination::Option)
		{
			SceneManager::GetInstance().SetNextScene(std::make_shared<SceneOption>());
			EndThisScene(true);
			return;
		}
		else if (m_destinationScene == eDestination::Ranking)
		{
			SceneManager::GetInstance().SetNextScene(std::make_shared<SceneRanking>());
			EndThisScene(true);
			return;
		}
		else if (m_destinationScene == eDestination::Quit)
		{
			SetIsGameEnd();
			return;
		}
	}

	//Bボタンを押したらタイトルに戻る
	if (Input::GetInstance().IsTriggered("CANCEL"))
	{
		SceneManager::GetInstance().SetNextScene(std::make_shared<SceneTitle>());
		EndThisScene();
		return;
	}
}
