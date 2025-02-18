#pragma once
#include <DxLib.h>
#include "Vec3.h"

namespace MyLib
{

/// <summary>
/// 物理、衝突判定に必要なデータの塊
/// </summary>
class Rigidbody
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Rigidbody();
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="m_useGravity">重力を与えるかどうか　false : 与えない, true : 与える</param>
	void Init(bool m_useGravity = false);	

	/// <summary>
	/// velocityに力を加える
	/// </summary>
	/// <param name="force">加える力</param>
	void AddForce(const Vec3& force);

	/// <summary>
	/// 現在座標を取得
	/// </summary>
	/// <returns>現在座標</returns>
	const Vec3& GetPos() const { return m_pos; }
	/// <summary>
	/// 移動予定座標を取得
	/// </summary>
	/// <returns>移動予定座標</returns>
	const Vec3& GetNextPos() const { return m_nextPos; }
	/// <summary>
	/// 向きを取得
	/// </summary>
	/// <returns>向き</returns>
	const Vec3& GetDir() const { return m_dir; }
	/// <summary>
	/// 移動速度を取得
	/// </summary>
	/// <returns>移動速度</returns>
	const Vec3& GetVelocity() const { return m_velocity; }

	/// <summary>
	/// 現在座標を取得
	/// </summary>
	/// <returns>現在座標</returns>
	const VECTOR GetPosVECTOR() { return m_pos.ToVECTOR(); }
	/// <summary>
	/// 移動予定座標を取得
	/// </summary>
	/// <returns>移動予定座標</returns>
	const VECTOR GetNextPosVECTOR() { return m_nextPos.ToVECTOR(); }
	/// <summary>
	/// 向きを取得
	/// </summary>
	/// <returns>向き</returns>
	const VECTOR GetDirVECTOR() { return m_dir.ToVECTOR(); }
	/// <summary>
	/// 移動速度を取得
	/// </summary>
	/// <returns>移動速度</returns>
	const VECTOR GetVelocityVECTOR(){ return m_velocity.ToVECTOR(); }
	/// <summary>
	/// 重力を与えるかどうかを取得
	/// </summary>
	/// <returns>重力フラグ</returns>
	bool GetUseGravity() const { return m_useGravity; }		

	/// <summary>
	/// 現在座標を設定
	/// </summary>
	/// <param name="set">設定する座標</param>
	void SetPos(const Vec3& set) { m_pos = set; }
	/// <summary>
	/// 移動予定座標を設定
	/// </summary>
	/// <param name="set">設定する移動予定座標</param>
	void SetNextPos(const Vec3& set) { m_nextPos = set; }
	/// <summary>
	/// 向きを設定
	/// </summary>
	/// <param name="set">設定する向き</param>
	/// <param name="mul">大きさ</param>
	void SetVelocity(Vec3 set,float mul = 1.0f);
	/// <summary>
	/// 移動速度を設定
	/// </summary>
	/// <param name="set">設定する移動速度</param>
	void SetUseGravity(bool set) { m_useGravity = set; }

private:
	Vec3 m_pos;			//現在座標
	Vec3 m_nextPos;		//移動予定座標
	Vec3 m_dir;			//向き
	Vec3 m_velocity;	//移動速度
	bool m_useGravity;	//重力を与えるかどうか
};
}