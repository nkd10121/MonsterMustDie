#pragma once
#include "StateBase.h"

/// <summary>
/// 敵の攻撃状態
/// </summary>
class EnemyStateAttack : public StateBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="own">持ち主のポインタ</param>
	EnemyStateAttack(std::shared_ptr<CharacterBase> own);
	/// <summary>
	/// 初期化
	/// </summary>
	void Init(std::string id);
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() override;

private:
	int m_waitCount;		//待機フレーム
	Vec3 m_attackVec;		//攻撃判定を出す方向ベクトル
};

