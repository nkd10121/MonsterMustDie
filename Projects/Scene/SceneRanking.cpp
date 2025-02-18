#include "SceneRanking.h"

/// <summary>
/// コンストラクタ
/// </summary>
SceneRanking::SceneRanking():
	SceneBase("SCENE_RANKING")
{
}

/// <summary>
/// デストラクタ
/// </summary>
SceneRanking::~SceneRanking()
{
}

/// <summary>
/// //リソースのロード開始
/// </summary>
void SceneRanking::StartLoad()
{
}

/// <summary>
/// リソースのロードが終了したかどうか
/// </summary>
bool SceneRanking::IsLoaded() const
{
	return true;
}

/// <summary>
/// 初期化
/// </summary>
void SceneRanking::Init()
{
	//フェードアウトをスキップする
	//SkipFadeOut();
}

/// <summary>
/// 終了
/// </summary>
void SceneRanking::End()
{
}

/// <summary>
/// 更新
/// </summary>
void SceneRanking::Update()
{
}

/// <summary>
/// 描画
/// </summary>
void SceneRanking::Draw()
{
#ifdef _DEBUG	//デバッグ描画
	DrawFormatString(0, 32, 0xffffff, "%s", GetNowSceneName());
#endif
}

/// <summary>
/// 次のシーンを選択する更新処理
/// </summary>
void SceneRanking::SelectNextSceneUpdate()
{
	//Bボタンを押したら元のシーンに戻る
	if (Input::GetInstance().IsTriggered("CANCEL"))
	{
		EndThisScene();
		return;
	}
}
