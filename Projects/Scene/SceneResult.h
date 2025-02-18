#pragma once
#include "SceneBase.h"
#include "Vec2.h"

class SceneResult : public SceneBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SceneResult();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~SceneResult();

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
	/// クリアしたかどうかを取得する
	/// </summary>
	/// <param name="isClear"></param>
	/// <returns></returns>
	const void SetIsClear(const bool isClear) { m_isClear = isClear; }

	const void SetStageName(std::string stageName);

	/// <summary>
	/// 次のシーンを選択する更新処理
	/// </summary>
	virtual void SelectNextSceneUpdate()override;
private:
	//遷移先列挙型
	enum eDestination : int
	{
		Start,

		InGame,			//インゲーム(次のステージ)
		ScoreDetail,	//スコア詳細
		Select,			//セレクト

		Last,
	};

	//遷移先
	eDestination m_destinationScene;

	bool m_isClear;		//クリアしたかどうか
	int m_score;		//スコア
	int m_drawScore;	//描画スコア

	int m_count;		//フレームカウント

	int m_alpha;		//α値。背景用

	int m_windowHandle;		//ウィンドウハンドル
	Vec2 m_windowDrawPos;	//ウィンドウの描画座標

	float m_resultTextAngle;		//
	int m_textAlpha;	//テキストのα値
	float m_textAngle;	//テキストの拡大用

	bool m_isChangeNextScene;
private:
	//更新メンバ関数ポインタ
	void (SceneResult::* m_updateFunc)();
	void UpdateNormal();
	void UpdateScoreDetail();

	//描画メンバ関数ポインタ
	void (SceneResult::* m_drawFunc)();
	void DrawNormal();
	void DrawScoreDetail();
};

