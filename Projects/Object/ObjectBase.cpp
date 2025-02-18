#include "ObjectBase.h"

/// <summary>
/// コンストラクタ
/// </summary>
ObjectBase::ObjectBase(Priority priority, GameObjectTag tag):
	Collidable(priority,tag),
	m_modelHandle(-1),
	m_isExist(false)
{
}

/// <summary>
/// デストラクタ
/// </summary>
ObjectBase::~ObjectBase()
{
	//リソース開放
	MV1DeleteModel(m_modelHandle);
}

void ObjectBase::Finalize()
{
	OnExistPhysics();
}

/// <summary>
/// 座標を設定
/// </summary>
void ObjectBase::SetPosition(Vec3 pos)
{
	rigidbody->SetPos(pos);
	MV1SetPosition(m_modelHandle, rigidbody->GetPosVECTOR());
}