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
class MiniMap;		//ミニマップ

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

	const std::string GetStageName()const { return m_stageName; }

private:
	std::shared_ptr<Player> m_pPlayer;			//プレイヤーポインタ
	std::shared_ptr<Camera> m_pCamera;			//カメラポインタ
	std::shared_ptr<Crystal> m_pCrystal;		//クリスタルポインタ
	std::shared_ptr<MyLib::Physics> m_pPhysics;	//物理クラスポインタ
	std::list<std::shared_ptr<ObjectBase>> m_pObjects;	//オブジェクトポインタの配列
	std::shared_ptr<EnemyManager> m_pEnemyManager;		//敵管理クラスポインタ

	std::shared_ptr<HPBar> m_pHpUi;	//HPバー
	std::shared_ptr<MiniMap> m_pMiniMap;	//ミニマップ

	std::string m_stageId;	//ステージID
	std::string m_stageName;	//ステージ名

	std::list<int> m_phaseNum;	//フェーズ番号
	bool m_isCreateEnemy;	//敵生成フラグ
	int m_phaseCount;	//フェーズカウント
	int m_allPhaseCount;	//全フェーズカウント

	int m_initTrapPoint;	//初期トラップポイント
	int m_allPhaseNum;	//全フェーズ数

	int m_portionCount;	//ポーション数
	int m_portionMax;	//ポーション最大数

	int m_slotBgHandle;	//スロット背景ハンドル
	std::vector<int> m_slotIconHandle;	//スロットアイコンハンドル

	bool m_isEnd;
	bool m_isClear;

	int m_operationHandle;

	int m_stageModel;	//モデルハンドル(仮)
	int m_skyBoxHandle;

	int m_alpha;
	int m_playerDeathCount;
};