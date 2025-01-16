#pragma once
#include "MyLib.h"
#include <memory>

//プロトタイプ宣言
class Player;		//プレイヤー
class Camera;		//カメラ
class ObjectBase;	//回復ポーション
class Crystal;		//クリスタル
class EnemyManager;		//敵
class HPBar;		//HPバー

class GameManager
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameManager();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~GameManager();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(int stageIdx);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// オブジェクトを追加する
	/// </summary>
	/// <param name="pAddObject"></param>
	void AddObject(std::shared_ptr<ObjectBase> pAddObject);

	const bool IsEnd(bool& isClear)const { isClear = m_isClear; return m_isEnd; }

private:
	std::shared_ptr<Player> m_pPlayer;			//プレイヤーポインタ
	std::shared_ptr<Camera> m_pCamera;			//カメラポインタ
	std::shared_ptr<Crystal> m_pCrystal;		//クリスタルポインタ
	std::shared_ptr<MyLib::Physics> m_pPhysics;	//物理クラスポインタ
	std::list<std::shared_ptr<ObjectBase>> m_pObjects;	//オブジェクトポインタの配列
	std::shared_ptr<EnemyManager> m_pEnemyManager;

	std::shared_ptr<HPBar> m_pHpUi;

	std::string m_stageId;

	std::list<int> m_phaseNum;
	bool m_isCreateEnemy;
	int m_phaseCount;
	int m_allPhaseCount;

	int m_initTrapPoint;
	int m_allPhaseNum;

	int m_portionCount;
	int m_portionMax;

	int m_slotBgHandle;
	std::vector<int> m_slotIconHandle;

	bool m_isEnd;
	bool m_isClear;

	int m_stageModel;	//モデルハンドル(仮)
	int m_skyBoxHandle;
};