#include "EnemyBase.h"

namespace
{
	//攻撃判定の半径
	constexpr float kAttackCollisionRadius = 4.0f;
	constexpr float kAttackCollisionDirection = 4.0f;

	constexpr int kRandMax = 12;

	//攻撃してきたオブジェクト名配列をリセットするまでの時間
	constexpr int kAttackerNameClearLimit = 60 * 6;
}

/// <summary>
/// コンストラクタ
/// </summary>
EnemyBase::EnemyBase() :
	CharacterBase(Collidable::Priority::High, GameObjectTag::Enemy),
	kind(),
	m_isExist(false),
	m_isStartDeathAnimation(false),
	m_routeIdx(0),
	m_collisionRadius(0.0f),
	m_collisionSize(0.0f),
	m_drawPos(),
	m_isSearchInPlayer(false),
	m_hitObjectTag(),
	m_attackerNameClearCount(0),
	m_attackerNameClearLimit(kAttackerNameClearLimit)
{
}

/// <summary>
/// デストラクタ
/// </summary>
EnemyBase::~EnemyBase()
{
	//モデルを削除する
	MV1DeleteModel(m_modelHandle);
}

void EnemyBase::SetRoute(const std::vector<EnemyManager::WayPoint> wayPoints)
{
	for (auto& wp : wayPoints)
	{
		auto add = wp;
		int rand = GetRand(kRandMax) - kRandMax/2;
		add.pos.x += rand;
		add.pos.y += 6.0f;
		rand = GetRand(kRandMax) - kRandMax/2;
		add.pos.z += rand;
		m_route.push_back(add);
	}
}

/// <summary>
/// 座標を設定
/// </summary>
void EnemyBase::SetPos(Vec3 pos)
{
	m_drawPos = pos;
}

/// <summary>
/// ほかのオブジェクトと衝突したときに呼ばれる
/// </summary>
void EnemyBase::OnCollideEnter(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol)
{

}

/// <summary>
/// ほかのオブジェクトと衝突したときに呼ばれる
/// </summary>
void EnemyBase::OnTriggerEnter(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol)
{

}

/// <summary>
/// ドロップする罠ポイントを取得
/// </summary>
const int EnemyBase::GetDropPoint() const
{
	//攻撃してきたオブジェクト名の配列のサイズが0か1なら設定されていたポイントをそのまま返す
	if (m_attackerName.size() == 0 || m_attackerName.size() == 1)
	{
		//auto screenPos = ConvWorldPosToScreenPos(rigidbody->GetPosVECTOR());
		//DrawFormatString(screenPos.x,screenPos.y, 0xff0000,"%d", m_status.point);

		return m_status.point;
	}
	//サイズが2以上だったらボーナスポイントを足して返す
	else
	{
		float thirtyPer = static_cast<float>(m_status.point) * 0.3f;

		//auto screenPos = ConvWorldPosToScreenPos(rigidbody->GetPosVECTOR());
		//DrawFormatString(screenPos.x, screenPos.y, 0xff0000, "%d", m_status.point + static_cast<int>(twentyPer) * static_cast<int>(m_attackerName.size()));

		return m_status.point + static_cast<int>(thirtyPer) * static_cast<int>(m_attackerName.size());
	}
}

/// <summary>
/// モデルの向きを設定
/// </summary>
const void EnemyBase::SetModelRotation(Vec3 rot) const
{
	MV1SetRotationXYZ(m_modelHandle, rot.ToVECTOR());

}

const void EnemyBase::SetHeadCollisionFrontVec(Vec3 vec) const
{
	auto col = GetCollider(MyLib::ColliderBase::CollisionTag::Head);
	if (col != nullptr)
	{
		col->collide->localPos.SetFrontPos(vec);
	}
}

/// <summary>
/// 攻撃判定を作成
/// </summary>
const void EnemyBase::CreateAttackCollision(Vec3 vec)
{
	m_isAttack = true;

	//当たり判定の作成
	auto collider = Collidable::AddCollider(MyLib::ColliderBase::Kind::Sphere, true, MyLib::ColliderBase::CollisionTag::Attack);
	auto sphereCol = dynamic_cast<MyLib::ColliderSphere*>(collider.get());
	sphereCol->m_radius = kAttackCollisionRadius;

	auto attackPos = vec.Normalize() * kAttackCollisionDirection;
	attackPos = Vec3(attackPos.x, 0.0f, attackPos.z);

	sphereCol->localPos.localPos = attackPos;

	return void();
}

/// <summary>
/// 攻撃判定を削除
/// </summary>
const void EnemyBase::DeleteAttackCollision()
{
	m_isAttack = false;

	auto col = GetCollider(MyLib::ColliderBase::CollisionTag::Attack);
	if (col == nullptr)return;		//もし攻撃の判定を持っていなかったら早期リターン

	Collidable::DeleteRequestCollider(col);

	return void();
}

/// <summary>
/// 指定した当たり判定タグが存在するかチェック
/// </summary>
const bool EnemyBase::CheckIsExistCollisionTag(MyLib::ColliderBase::CollisionTag tag) const
{
	//存在するならtrue,しなかったらfalse
	for (auto& col : m_colliders)
	{
		if (col->collideTag == tag)
		{
			return true;
		}
	}
	return false;
}

const void EnemyBase::AddAttackerName(std::string name)
{
	for (auto& attackerName : m_attackerName)
	{
		if (attackerName == name)
		{
			return;
		}
	}

	m_attackerName.push_back(name);
}
