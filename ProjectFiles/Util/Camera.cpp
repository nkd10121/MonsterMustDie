#include "Camera.h"
#include "DxLib.h"
#include "Input.h"
#include <cmath>

namespace
{
	constexpr float kCameraDist = 30.0f;
	constexpr float kCameraHeight = 10.0f;

	constexpr float kCameraNear = 1.0f;
	constexpr float kCameraFar = 360.0f*2;

	constexpr float kCameraRadius = kCameraNear + 0.2f;

	constexpr float kAngleMoveScaleMax = 3.2f;

	constexpr float kAnalogInputMax = 1000.0f;	//アナログスティックから入力されるベクトルの最大

	// カメラの旋回スピード
	constexpr float kCameraAngleSpeedX = 0.1f;
	constexpr float kCameraAngleSpeedY = 0.05f;

	//カメラの角度制限
	constexpr float kCameraAngleVLimitMin = -DX_PI_F / 2.0f + 0.6f;
	constexpr float kCameraAngleVLimitMax = DX_PI_F / 2.0f - 0.6f;

	// カメラからプレイヤーまでの最大距離
	constexpr float kCameraToPlayerLengthMax = 175.0f;
}

/// <summary>
/// コンストラクタ
/// </summary>
Camera::Camera() :
	m_cameraAngleX(0.0f),
	m_cameraAngleY(0.0f),
	m_cameraPos(),
	m_aimPos(),
	m_playerPos(),
	m_angleMoveScale(kAngleMoveScaleMax),
	m_lightHandle(-1)
{
	SetCameraNearFar(kCameraNear, kCameraFar);
	m_lightHandle = CreateDirLightHandle((m_aimPos - m_cameraPos).ToVECTOR());
}

/// <summary>
/// デストラクタ
/// </summary>
Camera::~Camera()
{
	DeleteLightHandle(m_lightHandle);
}

/// <summary>
/// 初期化
/// </summary>
void Camera::Init(int& handle)
{
	m_cameraAngleX = 0.0f;
	m_cameraAngleY = 0.0f;

	m_stageHandle = handle;
}

/// <summary>
/// 更新
/// </summary>
void Camera::Update()
{
	m_angleMoveScale = 2.0f;

	auto input = Input::GetInstance().GetInputStick(true);

	//入力から角度を計算する
	float inputRateX = input.first / kAnalogInputMax;
	float inputRateY = input.second / kAnalogInputMax;

	if (inputRateX > 0.001f)
	{
		m_cameraAngleX -= m_angleMoveScale * std::abs(inputRateX);
	}
	else if (inputRateX < -0.001f)
	{
		m_cameraAngleX += m_angleMoveScale * std::abs(inputRateX);
	}

	if (inputRateY > 0.001f)
	{
		m_cameraAngleY += m_angleMoveScale * std::abs(inputRateY);
		if (m_cameraAngleY > 90.0f)
		{
			m_cameraAngleY = 89.99f;
		}
	}
	else if (inputRateY < -0.001f)
	{
		m_cameraAngleY -= m_angleMoveScale * std::abs(inputRateY);
		if (m_cameraAngleY < -90.0f)
		{
			m_cameraAngleY = -89.99f;
		}
	}

	// カメラの位置はカメラの水平角度と垂直角度から算出
	// 最初に垂直角度を反映した位置を算出
	Vec3 tempPos1;
	float sinParam = sinf(m_cameraAngleY / 180.0f * DX_PI_F);
	float cosParam = cosf(m_cameraAngleY / 180.0f * DX_PI_F);
	tempPos1.x = 0.0f;
	tempPos1.y = sinParam * kCameraDist;
	tempPos1.z = -cosParam * kCameraDist;

	// 次に水平角度を反映した位置を算出
	Vec3 tempPos2;
	sinParam = sinf(m_cameraAngleX / 180.0f * DX_PI_F);
	cosParam = cosf(m_cameraAngleX / 180.0f * DX_PI_F);
	tempPos2.x = cosParam * tempPos1.x - sinParam * tempPos1.z;
	tempPos2.y = tempPos1.y;
	tempPos2.z = sinParam * tempPos1.x + cosParam * tempPos1.z;

	m_aimPos = Vec3(m_playerPos.x, m_playerPos.y + kCameraHeight, m_playerPos.z);

	// 算出した座標に注視点の位置を加算したものがカメラの位置になる
	m_cameraPos = tempPos2 + m_aimPos;

#if false
	// 最初はステージ自体と判定
	auto hitDim = MV1CollCheck_Capsule(m_stageHandle, -1, m_aimPos.ToVECTOR(), m_cameraPos.ToVECTOR(), kCameraRadius);

	// 検出した周囲のポリゴン情報を開放する
	MV1CollResultPolyDimTerminate(hitDim);

	// ステージのポリゴンは周囲に無かったら今度はコリジョンオブジェクトのポリゴンが周囲にあるか調べる
	if (hitDim.HitNum == 0)
	{
		// 検出した周囲のポリゴン情報を開放する
		MV1CollResultPolyDimTerminate(hitDim);
	}
	else if (hitDim.HitNum != 0)
	{
		bool doCheck = true;

		while (doCheck)
		{

			doCheck = false;

			//プレイヤーの座標からカメラの移動予定後座標の方向ベクトルを計算する
			auto playerToCamera = (m_cameraPos - m_aimPos);

			//向きと大きさに分ける
			auto vec = playerToCamera.Normalize();
			auto length = playerToCamera.Length();

			//距離を縮める
			length *= 0.998f;

			auto checkPos = m_aimPos + vec * length;

			// 最初はステージ自体と判定
			hitDim = MV1CollCheck_Capsule(m_stageHandle, -1, m_aimPos.ToVECTOR(), checkPos.ToVECTOR(), kCameraRadius);
			MV1CollResultPolyDimTerminate(hitDim);

			if (hitDim.HitNum != 0)
			{
				m_cameraPos = checkPos;
				doCheck = true;
			}
			else
			{
				doCheck = false;
			}
			// HitLength と NoHitLength が十分に近づいていなかったらループ
		}
	}
#else
	// ステージと注視座標からカメラ座標の線分の当たり判定をとる
	auto hitDim = MV1CollCheck_LineDim(m_stageHandle, -1, m_aimPos.ToVECTOR(), m_cameraPos.ToVECTOR());


	//ステージと線分が当たっていたら
	if (hitDim.HitNum > 0)
	{
		//注視座標と1個目の当たった座標の距離を計算する
		float minLength = (Vec3(hitDim.Dim[0].HitPosition) - m_aimPos).Length();

		//もし当たった数が2個以上ある場合
		if (hitDim.HitNum > 1)
		{
			//2個目以降の当たった座標と注視座標の距離を計算する
			for (int i = 1; i < hitDim.HitNum; i++)
			{
				//注視座標と当たった座標の距離を計算する
				auto length = (Vec3(hitDim.Dim[i].HitPosition) - m_aimPos).Length();

				//もし今までの距離より短ければ保存する
				if (minLength > length)
				{
					minLength = length;
				}
			}
		}

		// 検出した周囲のポリゴン情報を開放する
		MV1CollResultPolyDimTerminate(hitDim);

		//長さを補正する強さにする(そのままの値だとカメラがめり込んでしまうため少し縮める)
		auto fixPower = minLength * 0.8f;

		//注視座標からカメラ座標に向かう方向ベクトルを作る
		auto aimPosToCameraPos = m_cameraPos - m_aimPos;
		//正規化
		auto aimPosToCameraPosN = aimPosToCameraPos.Normalize();
		//注視座標に方向ベクトル*補正する強さを足して補正後のカメラ座標を計算する
		auto fixedPos = m_aimPos + aimPosToCameraPosN * fixPower;
		//補正後のカメラ座標を代入する
		m_cameraPos = fixedPos;
	}
#endif

	SetLightDirectionHandle(m_lightHandle, (m_aimPos - m_cameraPos).ToVECTOR());

	SetCameraPositionAndTarget_UpVecY(m_cameraPos.ToVECTOR(), m_aimPos.ToVECTOR());
}

/// <summary>
/// カメラが向いている方向ベクトルを取得
/// </summary>
const Vec3 Camera::GetDirection() const
{
	return (m_aimPos - m_cameraPos).Normalize();
}

const Vec3 Camera::GetMapHitPosition() const
{
	auto line = (m_aimPos - m_cameraPos).Normalize() * 10000.0f;
	auto ret = MV1CollCheck_Line(m_stageHandle, -1, m_cameraPos.ToVECTOR(), line.ToVECTOR());
	if (ret.HitFlag)
	{
		//DrawSphere3D(ret.HitPosition, 8, 8, 0xffffff, 0xffffff, true);
		return ret.HitPosition;
	}
	return line;
}
