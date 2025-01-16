#include "SceneStageSelect.h"

#include "SceneGame.h"
#include "SceneMainMenu.h"

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
SceneStageSelect::SceneStageSelect():
	SceneBase("SCENE_STAGESELECT"),
	m_nowCursor(0)
{
}

/// <summary>
/// デストラクタ
/// </summary>
SceneStageSelect::~SceneStageSelect()
{
}

/// <summary>
/// //リソースのロード開始
/// </summary>
void SceneStageSelect::StartLoad()
{
}

/// <summary>
/// リソースのロードが終了したかどうか
/// </summary>
bool SceneStageSelect::IsLoaded() const
{
	return true;
}

/// <summary>
/// 初期化
/// </summary>
void SceneStageSelect::Init()
{
	m_stageNames = LoadCSV::GetInstance().GetAllStageName();
}

/// <summary>
/// 終了
/// </summary>
void SceneStageSelect::End()
{
}

/// <summary>
/// 更新
/// </summary>
void SceneStageSelect::Update()
{
}

/// <summary>
/// 描画
/// </summary>
void SceneStageSelect::Draw()
{
#ifdef _DEBUG	//デバッグ描画
	DrawFormatString(0, 0, 0xffffff, "%s", GetNowSceneName());
#endif
	
	DrawString(kTextX - 24, kTextY + kTextYInterval * m_nowCursor, "→", 0xff0000);

	for (int i = 0;i < m_stageNames.size();i++)
	{
		DrawFormatString(kTextX,kTextY + kTextYInterval*i,0xffffff,"%s",m_stageNames[i].c_str());
	}
}

/// <summary>
/// 次のシーンを選択する更新処理
/// </summary>
void SceneStageSelect::SelectNextSceneUpdate()
{
	//上を入力したら
	if (Input::GetInstance().IsTriggered("UP"))
	{
		//現在選択している項目から一個上にずらす
		m_nowCursor--;

		//もし一番上の項目を選択している状態になっていたら
		if (m_nowCursor < 0)
		{
			//一個下にずらす
			m_nowCursor++;
		}
	}

	//下を入力したら
	if (Input::GetInstance().IsTriggered("DOWN"))
	{
		//現在選択している項目から一個下にずらす
		m_nowCursor++;

		//もし一番下の項目を選択している状態になっていたら
		if (m_nowCursor == m_stageNames.size())
		{
			//一個上にずらす
			m_nowCursor--;
		}
	}

	//決定ボタンを押したら現在選択しているシーンに遷移する
	if (Input::GetInstance().IsTriggered("OK"))
	{
		//ゲームシーンに遷移する
		SceneManager::GetInstance().SetStageIdx(m_nowCursor);
		SceneManager::GetInstance().SetNextScene(std::make_shared<SceneGame>());
		EndThisScene();
		return;
	}

	//Bボタンを押したらセレクトシーンに戻る
	if (Input::GetInstance().IsTriggered("CANCEL"))
	{
		SceneManager::GetInstance().SetNextScene(std::make_shared<SceneMainMenu>());
		EndThisScene();
		return;
	}
}