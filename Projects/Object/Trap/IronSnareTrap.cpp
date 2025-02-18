#include "IronSnareTrap.h"
#include "LoadCSV.h"
#include "ResourceManager.h"

namespace
{
	//当たり判定の円の半径
	constexpr float kCollisionRadius = 11.0f;
}

IronSnareTrap::IronSnareTrap() :
	TrapBase()
{
	m_trapName = "IronSnare";
	//罠のステータスを取得
	m_status = LoadCSV::GetInstance().LoadTrapStatus(m_trapName.c_str());
	//モデルのハンドルを取得
	m_modelHandle = ResourceManager::GetInstance().GetHandle(m_status.modelId);
	MV1SetScale(m_modelHandle, VECTOR(m_status.modelSize, m_status.modelSize, m_status.modelSize));
}

IronSnareTrap::~IronSnareTrap()
{
}

void IronSnareTrap::Init(Vec3 pos, Vec3 norm)
{
	//当たり判定を取るようにする
	OnEntryPhysics();

	//物理挙動の初期化
	rigidbody->Init();

	//当たり判定の生成
	auto collider = Collidable::AddCollider(MyLib::ColliderBase::Kind::Sphere, true, MyLib::ColliderBase::CollisionTag::Attack);
	auto sphereCol = dynamic_cast<MyLib::ColliderSphere*>(collider.get());
	sphereCol->m_radius = kCollisionRadius + 2.0;

	//座標の更新
	rigidbody->SetPos(pos);
	rigidbody->SetNextPos(pos);

	pos.y -= 2.0f;
	//モデルのハンドルを取得
	MV1SetPosition(m_modelHandle, pos.ToVECTOR());

	//存在フラグをtrueにする
	m_isExist = true;
}

void IronSnareTrap::Update()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;
}

void IronSnareTrap::Draw()
{
	//存在していない状態なら何もさせない
	if (!m_isExist)return;

	//モデルの描画
	MV1DrawModel(m_modelHandle);
}

void IronSnareTrap::SetRot(Vec3 vec)
{
	MV1SetRotationXYZ(m_modelHandle, vec.ToVECTOR());
}

void IronSnareTrap::SetPos(Vec3 vec)
{
	vec.y -= 2.0f;

	rigidbody->SetPos(vec);
	MV1SetPosition(m_modelHandle, vec.ToVECTOR());
}

std::vector<Vec3> IronSnareTrap::GetAttackPos()
{
	std::vector<Vec3> ret;
	ret.push_back(rigidbody->GetPos());
	return ret;
}
