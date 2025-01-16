#include "TrapBase.h"

TrapBase::TrapBase():
	ObjectBase(Collidable::Priority::Low, GameObjectTag::Trap),
	m_status(),
	m_isExist(false),
	m_isAttack(false)
{
	//敵以外のオブジェクトとは当たり判定をとらない
	AddThroughTag(GameObjectTag::Crystal);
	AddThroughTag(GameObjectTag::Player);
	AddThroughTag(GameObjectTag::SwarmEnemy);
	AddThroughTag(GameObjectTag::Portion);
	AddThroughTag(GameObjectTag::PlayerShot);
	AddThroughTag(GameObjectTag::Trap);
}

TrapBase::~TrapBase()
{
	MV1DeleteModel(m_modelHandle);
}

void TrapBase::Update()
{
}

void TrapBase::Draw()
{
}

void TrapBase::LoadModel()
{
}


void TrapBase::End()
{
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
