#pragma once
#include "SceneBase.h"
#include <functional>
#include <unordered_map>

class SceneDebug : public SceneBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SceneDebug();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~SceneDebug();

	/// <summary>
	/// //リソースのロード開始
	/// </summary>
	virtual void StartLoad() override;
	/// <summary>
	/// リソースのロードが終了したかどうか
	/// </summary>
	/// <returns>true : 終了済み, false : 終了していない</returns>
	virtual bool IsLoaded() const override;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() override;
	/// <summary>
	/// 終了
	/// </summary>
	virtual void End() override;
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() override;
	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() override;

	/// <summary>
	/// 次のシーンを選択する更新処理
	/// </summary>
	virtual void SelectNextSceneUpdate() override;

private:
	//遷移先列挙型
	enum eDestination : int
	{
		Start,

		Title,			//タイトル
		StageSelect,	//ステージセレクト
		InGame,			//インゲーム
		Result,			//リザルト
		Pause,			//ポーズ
		Option,			//オプション

		Select,			//セレクト
		Ranking,		//ランキング
		Strengthen,		//強化

		Last,
	};

	//遷移先
	eDestination m_destinationScene;

	int m_selectingStageIdx;
	std::vector<std::string> m_stageNames;

	//状態遷移のためのメンバ関数
	using UpdateFunc_t = void (SceneDebug::*)();
	using DrawFunc_t = void (SceneDebug::*)();
	UpdateFunc_t m_updateFunc;
	DrawFunc_t m_drawFunc;

	// シーン遷移のマップ
	std::unordered_map<eDestination, std::function<std::shared_ptr<SceneBase>()>> m_sceneTransitionMap;

	/*状態関数*/
	void UpdateItemSelect();
	void UpdateStageSelect();

	void DrawNormal();
	void DrawStageName();
};