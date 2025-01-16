#pragma once
#include "DxLib.h"
#include "Vec3.h"
#include <memory>

/// <summary>
/// カメラクラス
/// </summary>
class Camera
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Camera();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Camera();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(int& handle);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// カメラが向いている方向ベクトルを取得
	/// </summary>
	/// <returns>カメラが向いている方向ベクトル</returns>
	const Vec3 GetDirection()const;

	const Vec3 GetCameraPos()const { return m_cameraPos; }

	/// <summary>
	/// プレイヤーの座標を設定する
	/// </summary>
	/// <param name="playerPos">プレイヤーの座標</param>
	void SetPlayerPos(Vec3 playerPos) { m_playerPos = playerPos; }

	const Vec3 GetMapHitPosition()const;

private:
	float m_cameraAngleX;		//水平角度
	float m_cameraAngleY;		//垂直角度
	Vec3 m_cameraPos;			//カメラの座標
	Vec3 m_aimPos;				//注視点
	Vec3 m_playerPos;			//プレイヤーの座標
	float m_angleMoveScale;		//視点移動速度
	int m_lightHandle;			//ライトハンドル

	int m_stageHandle;
};

