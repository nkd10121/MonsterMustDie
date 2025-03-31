#include "EnemyFast.h"
#include "ResourceManager.h"

namespace
{
	constexpr float kCollisionCapsuleSize = 3.0f;	//カプセルの大きさ
	constexpr float kCollisionCapsuleRadius = 2.0f;	//カプセルの半径
}

/// <summary>
/// コンストラクタ
/// </summary>
EnemyFast::EnemyFast() :
	EnemyBase()
{
	//キャラクター名を設定
	m_characterName = "EnemyFast";
	m_isOffensive = false;
}

/// <summary>
/// 初期化
/// </summary>
void EnemyFast::Init()
{
	m_collisionRadius = kCollisionCapsuleRadius;
	m_collisionSize = kCollisionCapsuleSize;

	//モデルハンドルを取得
	m_modelHandle = ResourceManager::GetInstance().GetHandle("M_ENEMYNORMAL");

	EnemyBase::Init();
}