#include "EnemyNormal.h"
#include "ResourceManager.h"

namespace
{
	constexpr float kCollisionCapsuleSize = 3.0f;	//カプセルの大きさ
	constexpr float kCollisionCapsuleRadius = 2.0f;	//カプセルの半径
}

/// <summary>
/// コンストラクタ
/// </summary>
EnemyNormal::EnemyNormal() :
	EnemyBase()
{
	//キャラクター名を設定
	m_characterName = "EnemyNormal";
}

/// <summary>
/// 初期化
/// </summary>
void EnemyNormal::Init()
{
	//モデルハンドルを取得
	m_modelHandle = ResourceManager::GetInstance().GetHandle("M_ENEMY2");

	//モデルサイズを取得
	m_collisionRadius = kCollisionCapsuleRadius;
	m_collisionSize = kCollisionCapsuleSize;

	EnemyBase::Init();
}