#pragma once
#include "EnemyBase.h"

/// <summary>
/// 基本的な敵クラス
/// </summary>
class EnemyBig : public EnemyBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	EnemyBig();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~EnemyBig(){};

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="physics">物理クラスポインタ</param>
	virtual void Init()override;
};