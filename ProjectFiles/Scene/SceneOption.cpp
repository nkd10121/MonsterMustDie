#include "SceneOption.h"

namespace
{
}

/// <summary>
/// コンストラクタ
/// </summary>
SceneOption::SceneOption():
	SceneBase("SCENE_OPTION")
{
}

/// <summary>
/// デストラクタ
/// </summary>
SceneOption::~SceneOption()
{
}

/// <summary>
/// //リソースのロード開始
/// </summary>
void SceneOption::StartLoad()
{
}

/// <summary>
/// リソースのロードが終了したかどうか
/// </summary>
bool SceneOption::IsLoaded() const
{
	return true;
}

/// <summary>
/// 初期化
/// </summary>
void SceneOption::Init()
{
	//フェードアウトをスキップする
	//SkipFadeOut();
}

/// <summary>
/// 終了
/// </summary>
void SceneOption::End()
{
}

/// <summary>
/// 更新
/// </summary>
void SceneOption::Update()
{
}

/// <summary>
/// 描画
/// </summary>
void SceneOption::Draw()
{
#ifdef _DEBUG	//デバッグ描画
	DrawFormatString(0, 32, 0xffffff, "%s", GetNowSceneName());
#endif
}

/// <summary>
/// 次のシーンを選択する更新処理
/// </summary>
void SceneOption::SelectNextSceneUpdate()
{
	//Bボタンを押したら元のシーンに戻る
	if (Input::GetInstance().IsTriggered("CANCEL"))
	{
		EndThisScene();
		return;
	}
}
