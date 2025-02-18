#pragma once
#include "SceneBase.h"

/// <summary>
/// オプションシーン
/// </summary>
class SceneOption : public SceneBase
{
private:
	enum Item
	{
		MasterVolume,
		BGMVolume,
		SEVolume,
		Sensitivity,
		FullScreen,
		DrawOperation
	};

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SceneOption();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~SceneOption();

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
	int m_pushCount;
	int m_keyRepeatFrame;

	Item m_nowItem;

	int m_uiArrowHandle;
	float m_angle;

	bool m_isUpdateSound;
private:
	//更新メンバ関数ポインタ
	void (SceneOption::* m_updateFunc)();
	//更新(マスター音量設定)
	void UpdateMasterVolume();
	//更新(BGM音量設定)
	void UpdateBGMVolume();
	//更新(SE音量設定)
	void UpdateSEVolume();
	//更新(感度設定)
	void UpdateSensitivity();
	//更新(フルスクリーン設定)
	void UpdateFullScreen();
	//更新(操作説明を描画するか設定)
	void UpdateDrawOperation();

	//更新メンバ関数ポインタ
	void (SceneOption::* m_drawFunc)(std::vector<unsigned int> color);
	void DrawSound(std::vector<unsigned int> color);
	void DrawOther(std::vector<unsigned int> color);
};