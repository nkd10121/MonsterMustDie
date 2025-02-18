#pragma once
#include "StateBase.h"
class PlayerStateClear : public StateBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="own">持ち主のポインタ</param>
	PlayerStateClear(std::shared_ptr<CharacterBase> own);
	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlayerStateClear()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(std::string id)override;
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() override;
};

