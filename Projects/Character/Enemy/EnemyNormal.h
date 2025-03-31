#pragma once
#include "EnemyBase.h"

/// <summary>
/// 基本的な敵クラス
/// </summary>
class EnemyNormal : public EnemyBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	EnemyNormal();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~EnemyNormal(){};

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="physics">物理クラスポインタ</param>
	virtual void Init()override;
};