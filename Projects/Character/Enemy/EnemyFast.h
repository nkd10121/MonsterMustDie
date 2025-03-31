#pragma once
#include "EnemyBase.h"

/// <summary>
/// 基本的な敵クラス
/// </summary>
class EnemyFast : public EnemyBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	EnemyFast();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~EnemyFast(){};

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="physics">物理クラスポインタ</param>
	virtual void Init()override;
};