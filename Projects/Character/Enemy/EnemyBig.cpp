#include "EnemyBig.h"
#include "ResourceManager.h"

namespace
{
	constexpr float kCollisionCapsuleSize = 5.0f;	//カプセルの大きさ
	constexpr float kCollisionCapsuleRadius = 3.0f;	//カプセルの半径
}

/// <summary>
/// コンストラクタ
/// </summary>
EnemyBig::EnemyBig() :
	EnemyBase()
{
	//キャラクター名を設定
	m_characterName = "EnemyBig";
}

/// <summary>
/// 初期化
/// </summary>
void EnemyBig::Init()
{
	//モデルが大きめのため、ルートを少し上にずらす
	for (auto& p : m_route)
	{
		p.pos.y += 2.0f;
	}

	//モデルハンドルを取得
	m_modelHandle = ResourceManager::GetInstance().GetHandle("M_ENEMY3");

	//当たり判定のサイズを設定
	m_collisionRadius = kCollisionCapsuleRadius;
	m_collisionSize = kCollisionCapsuleSize;

	EnemyBase::Init();
}