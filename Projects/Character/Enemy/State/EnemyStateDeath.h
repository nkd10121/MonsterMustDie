#pragma once
#include "StateBase.h"

/// <summary>
/// 敵の死亡状態
/// </summary>
class EnemyStateDeath : public StateBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="own">持ち主のポインタ</param>
	EnemyStateDeath(std::shared_ptr<CharacterBase> own);
	/// <summary>
	/// 初期化
	/// </summary>
	void Init(std::string id);
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() override;

private:
	int m_frame;
	int m_trapPointGraphHandle;
};

