#include "HealPortion.h"
#include "DxLib.h"

#include "ResourceManager.h"
#include "EffectManager.h"

namespace
{
	//当たり判定の円の半径
	constexpr float kCollisionRadius = 2.4f;

	//モデルサイズ
	constexpr float kModelScale = 0.08f;
	//エフェクトを生成する間隔
	constexpr int kCreateEffectInterval = 120;
	//ポーションが上下する速度
	constexpr float kMoveSpeed = 0.04f;
	//ポーションが上下する幅
	constexpr float kMoveRange = 0.01f;
	//座標を設定するときのY座標のオフセット
	constexpr float kOffsetPosY = 2.0f;
	//ポーションのパス
	const std::string kPortionPath = "data/model/object/portion/bottle_red.mv1";

}

/// <summary>
/// コンストラクタ
/// </summary>
HealPortion::HealPortion() :
	ObjectBase(Collidable::Priority::Low, GameObjectTag::Portion),
	m_effectCreateFrame(0),
	m_angle(0.0f),
	m_posOffsetY(0.0f)
{
	//当たり判定の生成
	auto collider = Collidable::AddCollider(MyLib::ColliderBase::Kind::Sphere, true);
	auto sphereCol = dynamic_cast<MyLib::ColliderSphere*>(collider.get());
	sphereCol->m_radius = kCollisionRadius;
}

/// <summary>
/// デストラクタ
/// </summary>
HealPortion::~HealPortion()
{

}

/// <summary>
/// 初期化
/// </summary>
void HealPortion::Init()
{
	OnEntryPhysics();

	//物理挙動の初期化
	rigidbody->Init();

	//モデルのハンドルを取得
	m_modelHandle = ResourceManager::GetInstance().GetHandle("M_PORTION");
	//モデルのスケールを設定
	MV1SetScale(m_modelHandle, VGet(kModelScale, kModelScale, kModelScale));

	//存在フラグをtrueにする
	m_isExist = true;
}

/// <summary>
/// 更新
/// </summary>
void HealPortion::Update()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	rigidbody->SetVelocity(Vec3());

	//一定時間ごとにエフェクトを出す
	if (m_effectCreateFrame % kCreateEffectInterval == 0)
	{
		//エフェクトの表示座標を取得
		auto pos = rigidbody->GetPos();
		//エフェクトを生成
		EffectManager::GetInstance().CreateEffect("E_PORTION", pos, pos);
	}

	//エフェクト生成カウントを増やす
	m_effectCreateFrame++;

	/*ポーションがY軸上をsinカーブで上下しながら、Y軸回転するための処理*/
	m_angle += kMoveSpeed;
	m_posOffsetY = sinf(m_angle) * kMoveRange;

	auto pos = rigidbody->GetPos();
	pos.y += m_posOffsetY;

	rigidbody->SetPos(pos);
	MV1SetPosition(m_modelHandle, pos.ToVECTOR());
	MV1SetRotationXYZ(m_modelHandle, VGet(0.0f, m_angle / 2, 0.0f));
}

/// <summary>
/// 描画
/// </summary>
void HealPortion::Draw()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	//モデルの描画
	MV1DrawModel(m_modelHandle);
}

void HealPortion::LoadModel()
{
}

/// <summary>
/// 座標を設定
/// </summary>
void HealPortion::SetPosition(Vec3 pos)
{
	pos.y += kOffsetPosY;
	rigidbody->SetPos(pos);

	MV1SetPosition(m_modelHandle, rigidbody->GetPosVECTOR());
}

/// <summary>
/// 終了させる
/// </summary>
void HealPortion::End()
{
	m_isExist = false;
}