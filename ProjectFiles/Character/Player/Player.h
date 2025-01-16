#pragma once
#include "Vec3.h"
#include "CharacterBase.h"

#include <string>

class GameManager;

/// <summary>
/// プレイヤークラス
/// </summary>
class Player : public CharacterBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Player();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="physics">物理クラスポインタ</param>
	void Init(std::string stageId);
	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update(GameManager* pGameManager, Vec3 cameraRayCastRet);
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// モデル座標の更新
	/// </summary>
	void UpdateModelPos();

	/// <summary>
	/// プレイヤーの現在座標を取得する
	/// </summary>
	/// <returns>プレイヤーの現在座標</returns>
	const Vec3 GetPos()const { return rigidbody->GetPos(); }
	/// <summary>
	/// カメラの方向ベクトルを設定する
	/// </summary>
	/// <param name="m_dir">カメラの方向ベクトル</param>
	void SetCameraAngle(Vec3 m_dir) { m_cameraDirection = m_dir; }

	const void SetClearState();

	/// <summary>
	/// カメラの方向ベクトルを取得する
	/// </summary>
	/// <returns>カメラの方向ベクトル</returns>
	const Vec3 GetCameraDirecton()const { return m_cameraDirection; }

	/// <summary>
	/// 当たり判定のカプセルのサイズを取得
	/// </summary>
	/// <returns>当たり判定のカプセルのサイズ</returns>
	const float GetCollisionSize()const { return m_collisionSize; }
	/// <summary>
	/// 当たり判定のカプセルの半径を取得
	/// </summary>
	/// <returns>当たり判定のカプセルの半径</returns>
	const float GetCollisionRadius()const { return m_collisionRadius; }

	/// <summary>
	/// プレイヤーが死亡したかどうかを取得
	/// </summary>
	/// <returns></returns>
	const bool GetIsDeath()const { return m_isDeath; }

	/// <summary>
	/// 現在選択しているスロット番号を取得
	/// </summary>
	/// <returns>スロット番号</returns>
	const int GetNowSlotNumber()const { return m_slotNum; }

	/// <summary>
	/// 押し出し処理を行うオブジェクトと衝突したとき
	/// </summary>
	/// <param name="colider">衝突したオブジェクト</param>
	void OnCollideEnter(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol)override;
	/// <summary>
	/// 押し出し処理を行わないオブジェクトと衝突したとき
	/// </summary>
	/// <param name="colider">衝突したオブジェクト</param>
	void OnTriggerEnter(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol)override;

private:
	int m_crossbowHandle;

	std::string m_stageId;

	Vec3 m_pos;				//プレイヤーの描画座標
	Vec3 temp_moveVec;		//入力情報を確認するためのデバッグ用
	Vec3 m_cameraDirection;	//カメラの方向ベクトル
	Vec3 m_rot;				//角度

	Vec3 m_crossbowPos;

	float m_collisionSize;
	float m_collisionRadius;

	float m_cameraAngle;		//カメラの角度
	float m_angle;				//プレイヤーが向いている方向

	int m_attackButtonPushCount;
	int m_attackIntervalCount;		//攻撃するたびにこのカウントを増やす。カウントの大きさに比例して弾の拡散をする用。


	bool m_isStartDeathAnimation;
	bool m_isDeath;

	int m_slotNum;
};

