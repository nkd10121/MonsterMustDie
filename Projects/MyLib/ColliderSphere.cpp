#include "ColliderSphere.h"

/// <summary>
/// コンストラクタ
/// </summary>
MyLib::ColliderSphere::ColliderSphere(bool isTrigger) :
	ColliderBase(ColliderBase::Kind::Sphere, isTrigger),
	m_radius(0)
{
}
