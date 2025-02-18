#include "SceneStrengthen.h"

#include "ResourceManager.h"

/// <summary>
/// コンストラクタ
/// </summary>
SceneStrengthen::SceneStrengthen():
	SceneBase("SCENE_STRENGTHEN")
{
}

/// <summary>
/// デストラクタ
/// </summary>
SceneStrengthen::~SceneStrengthen()
{
	ResourceManager::GetInstance().Clear(GetNowSceneName());
}

/// <summary>
/// //リソースのロード開始
/// </summary>
void SceneStrengthen::StartLoad()
{
}

/// <summary>
/// リソースのロードが終了したかどうか
/// </summary>
bool SceneStrengthen::IsLoaded() const
{
	return true;
}

/// <summary>
/// 初期化
/// </summary>
void SceneStrengthen::Init()
{
	//フェードアウトをスキップする
	//SkipFadeOut();
}

/// <summary>
/// 終了
/// </summary>
void SceneStrengthen::End()
{
}

/// <summary>
/// 更新
/// </summary>
void SceneStrengthen::Update()
{
}

/// <summary>
/// 描画
/// </summary>
void SceneStrengthen::Draw()
{
#ifdef _DEBUG	//デバッグ描画
	DrawFormatString(0, 32, 0xffffff, "%s", GetNowSceneName());
#endif
}

/// <summary>
/// 次のシーンを選択する更新処理
/// </summary>
void SceneStrengthen::SelectNextSceneUpdate()
{
	//Bボタンを押したら元のシーンに戻る
	if (Input::GetInstance().IsTriggered("CANCEL"))
	{
		EndThisScene();
		return;
	}
}
