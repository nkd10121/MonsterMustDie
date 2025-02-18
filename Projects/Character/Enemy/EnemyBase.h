#pragma once
#include <list>
#include <string>

#include "CharacterBase.h"
#include "EnemyManager.h"

/// <summary>
/// 敵の基底クラス
/// </summary>
class EnemyBase : public CharacterBase
{
public:
	//種類
	enum EnemyKind
	{
		EnemyNormal,
	};
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	EnemyBase();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~EnemyBase();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="physics">物理クラスポインタ</param>
	virtual void Init() {};
	/// <summary>
	/// 終了
	/// </summary>
	/// <param name="physics">物理クラスポインタ</param>
	virtual void Finalize() {};
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() {};
	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() {};

	/// <summary>
	/// 移動ルートを設定する
	/// </summary>
	/// <param name="wayPoints">移動ルート</param>
	void SetRoute(const std::vector<EnemyManager::WayPoint> wayPoints);

	/// <summary>
	/// 座標を設定
	/// </summary>
	/// <param name="pos">設定したい座標</param>
	void SetPos(Vec3 pos);

	/// <summary>
	/// 座標を取得
	/// </summary>
	/// <returns></returns>
	const Vec3 GetPos()const { return rigidbody->GetPos(); }

	/// <summary>
	/// 終了(他から呼ぶ)
	/// </summary>
	void End() { m_isExist = false; }

	/// <summary>
	/// ほかのオブジェクトと衝突したときに呼ばれる
	/// </summary>
	/// <param name="colider">当たったオブジェクト</param>
	virtual void OnCollideEnter(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol)override;
	/// <summary>
	/// ほかのオブジェクトと衝突したときに呼ばれる
	/// </summary>
	/// <param name="colider">当たったオブジェクト</param>
	virtual void OnTriggerEnter(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol)override;
	virtual void OnTriggerStay(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol)override;


	/// <summary>
	/// 3Dモデルの座標更新
	/// </summary>
	virtual void UpdateModelPos() {};

	/// <summary>
	/// 当たり判定のサイズを取得
	/// </summary>
	/// <returns></returns>
	const float GetCollisionSize()const { return m_collisionSize; }
	/// <summary>
	/// 当たり判定の半径を取得
	/// </summary>
	/// <returns></returns>
	const float GetCollisionRadius()const { return m_collisionRadius; }

	/// <summary>
	/// 存在フラグを取得
	/// </summary>
	/// <returns>存在フラグ</returns>
	const bool GetIsExist()const { return m_isExist; }
	/// <summary>
	/// 死亡アニメーションを開始しているかどうかを取得
	/// </summary>
	/// <returns></returns>
	const bool GetIsStartDeathAnim()const { return m_isStartDeathAnimation; }
	/// <summary>
	/// 攻撃力を取得
	/// </summary>
	/// <returns>攻撃力</returns>
	const int GetAttackPower()const { return m_status.atk; }
	/// <summary>
	/// 索敵判定内にプレイヤーが存在しているかを取得
	/// </summary>
	/// <returns>true:存在している,false:存在していない</returns>
	const bool GetIsSearchInPlayer()const { return m_isSearchInPlayer; }
	/// <summary>
	/// プレイヤーの座標を取得
	/// </summary>
	/// <returns>プレイヤーの座標</returns>
	const Vec3 GetPlayerPos()const { return m_playerPos; }

	/// <summary>
	/// ドロップする罠ポイントを取得
	/// </summary>
	/// <returns></returns>
	const int GetDropPoint()const;

	const int GetCrystalDamage()const { return m_status.crystalDamage; }

	/// <summary>
	/// 索敵判定の半径を取得
	/// </summary>
	/// <returns>索敵判定の半径</returns>
	virtual const float GetSearchCollisionRadius()const = 0;
	/// <summary>
	/// モデルの向きを設定
	/// </summary>
	/// <param name="rot">向きベクトル</param>
	const void SetModelRotation(Vec3 rot)const;

	const void SetHeadCollisionFrontVec(Vec3 vec)const;

	/// <summary>
	/// 索敵判定の作成
	/// </summary>
	virtual void CreateSearchCollision() {};
	/// <summary>
	/// 索敵判定の削除
	/// </summary>
	virtual void DeleteSearchCollision() {};
	/// <summary>
	/// 攻撃判定を作成
	/// </summary>
	/// <param name="vec">攻撃判定を出す方向ベクトル</param>
	const void CreateAttackCollision(Vec3 vec);
	/// <summary>
	/// 攻撃判定を削除
	/// </summary>
	const void DeleteAttackCollision();

	/// <summary>
	/// 最後に攻撃してきたオブジェクトのタグを取得
	/// </summary>
	/// <returns></returns>
	const GameObjectTag GetLastAttackTag()const { return m_lastAttackTag; }

	/// <summary>
	/// ルートインデックスを更新する
	/// </summary>
	/// <returns></returns>
	const void AddRouteIdx() { m_routeIdx++; }
	/// <summary>
	/// ルートインデックスのウェイポイント座標を取得する
	/// </summary>
	/// <returns></returns>
	const Vec3 GetNextPos()const { return m_route[m_routeIdx].pos; }

	/// <summary>
	/// モデルハンドルを取得
	/// カメラとのレイキャストをするため
	/// </summary>
	/// <returns></returns>
	const int GetModelHandle()const { return m_modelHandle; }

	/// <summary>
	/// 攻撃してきたオブジェクト名を全削除
	/// </summary>
	/// <returns></returns>
	const void ClearAttackerName() { m_attackerName.clear(); }
	/// <summary>
	/// 攻撃してきたオブジェクト名の数を取得
	/// </summary>
	/// <returns></returns>
	const int GetAttackerNaneNum() { return static_cast<int>(m_attackerName.size()); }

	const bool GetIsOffensive()const { return m_isOffensive; }

	const float GetMoveDebuff()const { return m_moveDebuff; }

protected:
	/// <summary>
	/// 指定した当たり判定タグが存在するかチェック
	/// </summary>
	/// <param name="tag"></param>
	/// <returns></returns>
	const bool CheckIsExistCollisionTag(MyLib::ColliderBase::CollisionTag tag)const;

	/// <summary>
	/// 攻撃してきたオブジェクト名を追加する
	/// </summary>
	/// <param name="name">攻撃してきたオブジェクト名</param>
	/// <returns></returns>
	const void AddAttackerName(std::string name);

protected:
	EnemyKind kind;		//種類
	bool m_isExist;		//存在フラグ
	bool m_isStartDeathAnimation;	//死亡アニメーションを開始するか

	std::vector<EnemyManager::WayPoint> m_route;	//移動ルート
	int m_routeIdx;
	
	float m_moveDebuff;

	float m_collisionRadius;	//当たり判定の半径
	float m_collisionSize;		//当たり判定のサイズ

	Vec3 m_drawPos;		//描画座標

	bool m_isSearchInPlayer;		//索敵範囲内にプレイヤーがいるか
	Vec3 m_playerPos;				//プレイヤーの座標

	GameObjectTag m_hitObjectTag;	//当たったオブジェクトタグ
	GameObjectTag m_lastAttackTag;	//最後に攻撃してきたオブジェクトタグ

	bool m_isOffensive;	//攻撃的かどうか
	bool m_isAttack;	//攻撃しているかどうか

	std::list<std::string> m_attackerName;	//攻撃してきたオブジェクトの名前
	int m_attackerNameClearCount;			//攻撃してきたオブジェクト名の配列をクリア(全消去)するまでのカウント
	int m_attackerNameClearLimit;			//攻撃してきたオブジェクト名の配列をクリア(全消去)するまでの時間
};