#include "TrapBase.h"

/// <summary>
/// コンストラクタ
/// </summary>
TrapBase::TrapBase():
	ObjectBase(Collidable::Priority::Low, GameObjectTag::Trap),
	m_status(),
	m_isExist(false),
	m_isAttack(false),
	m_trapName(),
	m_isPreview(false),
	m_angle(0.0f)
{
	//敵以外のオブジェクトとは当たり判定をとらない
	AddThroughTag(GameObjectTag::Crystal);
	AddThroughTag(GameObjectTag::Player);
	AddThroughTag(GameObjectTag::SwarmEnemy);
	AddThroughTag(GameObjectTag::Portion);
	AddThroughTag(GameObjectTag::PlayerShot);
	AddThroughTag(GameObjectTag::Trap);
}

/// <summary>
/// デストラクタ
/// </summary>
TrapBase::~TrapBase()
{
	MV1DeleteModel(m_modelHandle);
}

void TrapBase::PreviewDraw()
{
	m_angle += 0.04f;
	auto transparency = abs(sinf(m_angle) / 2.5f) + 0.1f;

	MV1SetOpacityRate(m_modelHandle, transparency);
	MV1DrawModel(m_modelHandle);
}

void TrapBase::SetPos(Vec3 pos)
{
	rigidbody->SetPos(pos);
	MV1SetPosition(m_modelHandle, pos.ToVECTOR());
}


void TrapBase::OnTriggerEnter(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol)
{
	//自身の索敵当たり判定に
	if (ownCol->collideTag == MyLib::ColliderBase::CollisionTag::Search)
	{
		//敵が当たっていたら
		if (send->GetTag() == GameObjectTag::Enemy && sendCol->collideTag == MyLib::ColliderBase::CollisionTag::Normal)
		{
			m_isAttack = true;
		}
	}
}
