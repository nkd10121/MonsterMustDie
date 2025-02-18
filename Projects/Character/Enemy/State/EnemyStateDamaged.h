#pragma once
#include "StateBase.h"

/// <summary>
/// 敵の被ダメージ状態
/// </summary>
class EnemyStateDamaged : public StateBase
{
public:
	EnemyStateDamaged(std::shared_ptr<CharacterBase> own);
	/// <summary>
	/// 初期化
	/// </summary>
	void Init(std::string id);
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() override;

private:
	int m_damagedCount;
};

