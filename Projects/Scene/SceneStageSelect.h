// SceneStageSelect.h
#pragma once
#include "SceneBase.h"
#include "Vec3.h"
#include <vector>
#include <string>

/// <summary>
/// ステージセレクトシーン
/// </summary>
class SceneStageSelect : public SceneBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SceneStageSelect();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~SceneStageSelect();

	/// <summary>
	/// リソースのロード開始
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
	std::vector<std::string> m_stageNames; // ステージ名のリスト
	std::vector<int> m_stageMinimapHandle; // ステージ名のリスト


	bool isNextScene;	// 次のシーンに遷移するかどうか
	int m_nowCursor;	// 現在のカーソル位置
	int m_transitionFrameCount; // 次のシーンに遷移するまでのフレーム数

	int m_smallWindowHandle;	// 小窓のハンドル
	int m_bigWindowHandle;	// 大窓のハンドル

	Vec3 m_cameraPos;	// カメラの位置
	Vec3 m_cameraTarget;	// カメラのターゲット

	float m_cameraMoveDistance; // カメラの移動距離
	float m_angle;	// UIを動かすための角度
};