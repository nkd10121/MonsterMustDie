﻿#pragma once
#include "SceneBase.h"

/// <summary>
/// メインメニュー画面
/// </summary>
class SceneMainMenu : public SceneBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SceneMainMenu();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~SceneMainMenu();

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
	virtual void SelectNextSceneUpdate()override;

private:
	//遷移先列挙型
	enum eDestination : int
	{
		Start,

		StageSelect,	//セレクト
		Strengthen,		//強化
		Option,			//オプション
		Ranking,		//ランキング
		Quit,			//やめる

		Last,
	};

	//遷移先
	eDestination m_destinationScene;
};

