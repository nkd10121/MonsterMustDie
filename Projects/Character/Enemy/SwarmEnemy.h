#pragma once
#include <list>

#include "EnemyBase.h"

/// <summary>
/// 敵の群れを管理するクラス
/// </summary>
class SwarmEnemy : public MyLib::Collidable
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="color">デバッグ用色</param>
	SwarmEnemy(unsigned int color);
	/// <summary>
	/// デストラクタ
	/// </summary>
	~SwarmEnemy();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update(Vec3 start, Vec3 end);
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	/// <summary>
	/// 群れメンバーのモデル座標を更新
	/// </summary>
	void UpdateModelPos();

	/// <summary>
	/// 群れメンバーを追加しきった後に呼ぶ関数
	/// </summary>
	void SetUp();

	/// <summary>
	/// 構成メンバーを追加
	/// </summary>
	/// <param name="add"></param>
	void AddSwarm(std::shared_ptr<EnemyBase> add);

	/// <summary>
	/// メンバーの人数を取得
	/// </summary>
	/// <returns></returns>
	const int CheckMemberNum()const;

	const std::list<Vec3> GetAllEnemyPos();

	/// <summary>
	/// 構成メンバーが存在するかどうかを取得
	/// </summary>
	/// <returns>構成メンバーが存在するかどうか</returns>
	const bool GetIsExistMember()const { return m_isExistMember; }
	/// <summary>
	/// カメラからのレイと敵が当たっているかどうか
	/// </summary>
	/// <returns></returns>
	const bool GetIsCameraRayHit()const { return m_isCameraRayHit; }
	/// <summary>
	/// カメラからのレイとのヒット座標を取得
	/// </summary>
	/// <returns></returns>
	const Vec3 GetCameraRayHitPos()const { return m_cameraRayClosestOnLine; }

	/// <summary>
	/// レイキャストに当たった敵を取得
	/// </summary>
	/// <returns></returns>
	const std::weak_ptr<EnemyBase> GetRayHitEnemy()const { return m_reyHitEnemy; }

	/// <summary>
	/// 敵が何によって殺されたかの数値データを取得
	/// </summary>
	/// <param name="playerKill">プレイヤーのキル数の参照</param>
	/// <param name="trapKill">トラップのキル数の参照</param>
	/// <returns></returns>
	const void GetKilledData(int& playerKill, int& trapKill);


	/// <summary>
	/// 直前に敵が倒されたかどうかを取得する
	/// </summary>
	/// <returns></returns>
	const bool GetIsKilled(Vec3& pos);

	/// <summary>
	/// 最初に生成したメンバーの生成フレームを設定
	/// </summary>
	/// <param name="frame">生成フレーム</param>
	/// <returns></returns>
	const void SetFirstCreateFrame(int frame) { m_firstCreateFrame = frame; }

	/// <summary>
	/// 最初に生成したメンバーの生成フレームを取得
	/// </summary>
	/// <returns></returns>
	const int GetFirstCreateFrame()const { return m_firstCreateFrame; }

	/// <summary>
	/// 押し出し処理を行わないオブジェクトと衝突したとき
	/// </summary>
	void OnTriggerEnter(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol)override;
	/// <summary>
	/// 押し出し処理を行わないオブジェクトと衝突しなくなった時
	/// </summary>
	void OnTriggerExit(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol)override;

	const std::list<int> GetModelHandles()const;
private:
	std::list<std::shared_ptr<EnemyBase>> m_swarm;	//群れの構成員
	int m_firstCreateFrame;		//最初に追加したメンバーの生成フレーム
	bool m_isExistMember;		//メンバーが存在するかどうか

	Vec3 m_swarmCenterPos;	//群れ全体の中心座標
	float m_swarmRadius;	//群れ全体の半径
	float m_maxSearchCollisionRadius;		//群れの個体の中で一番大きい索敵範囲

	bool m_isInPlayer;		//プレイヤーが当たり判定内に入っているかどうか

	bool m_isCameraRayHit;	//メンバーの誰かがカメラからのレイに当たったかどうか
	Vec3 m_cameraRayClosestOnLine;	//カメラからのレイと敵の当たり判定カプセルとのヒット座標]
	std::weak_ptr<EnemyBase> m_reyHitEnemy;	//レイに当たった敵

	int m_killedByTrapNum;		//罠によって倒された敵の数
	int m_killedByPlayerNum;	//プレイヤーによって倒された敵の数

	bool m_isKilled;			//直前に敵が倒されたかどうか
	Vec3 m_killedPos;			//殺された座標

	unsigned int m_memberColor;	//DEBUG用。誰がどの群れなのか見たい
};

