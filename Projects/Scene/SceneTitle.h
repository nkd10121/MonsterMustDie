#pragma once
#include "SceneBase.h"
#include <memory>

class Crystal;
class EnemyManager;

/// <summary>
/// タイトルシーン
/// </summary>
class SceneTitle : public SceneBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SceneTitle();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~SceneTitle();

	/// <summary>
	/// //リソースのロード開始
	/// </summary>
	virtual void StartLoad()override;
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

		Option,			//オプション
		Select,		//セレクト
		Quit,			//やめる

		Last,
	};

	//遷移先
	eDestination m_destinationScene;

	std::shared_ptr<Crystal> m_pCrystal;			//クリスタルポインタ
	std::shared_ptr<EnemyManager> m_pEnemyManager;	//敵管理クラスポインタ
	std::shared_ptr<MyLib::Physics> m_pPhysics;		//物理クラスポインタ

	bool isNextScene;	//次のシーンに遷移するかどうか
	Vec3 m_cameraTarget;	//カメラのターゲット

	int m_enemyCreateFrame;	//敵生成フレーム

	int m_lightHandle;	//ライトハンドル
	int m_rogoHandle;	//ロゴハンドル
	int m_buttonHandle;	//ボタンハンドル

	float m_angle;	//UIを動かすための角度
};