#include "ColliderBase.h"

/// <summary>
/// コンストラクタ
/// </summary>
MyLib::ColliderBase::ColliderBase(Kind kind, bool isTrigger):
	localPos(),
	kind(kind),
	isTrigger(isTrigger),
	isStatic(false)
{

}

const void MyLib::ColliderBase::SetCenterPos(Vec3 pos)
{
	localPos.centerPos = pos;
}

const void MyLib::ColliderBase::SetOffsetPos(Vec3 pos)
{
	localPos.localPos = pos;
}
