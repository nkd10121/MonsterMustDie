#pragma once
#include "StateBase.h"
class PlayerStateDeath : public StateBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="own"></param>
	PlayerStateDeath(std::shared_ptr<CharacterBase> own);
	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlayerStateDeath()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(std::string id)override;
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() override;
};

