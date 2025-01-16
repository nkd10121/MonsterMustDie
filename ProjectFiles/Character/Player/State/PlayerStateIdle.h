#pragma once
#include "StateBase.h"

/// <summary>
/// プレイヤーの待機状態
/// </summary>
class PlayerStateIdle : public StateBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="own">持ち主のポインタ</param>
	PlayerStateIdle(std::shared_ptr<CharacterBase> own);
	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlayerStateIdle()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(std::string id)override;
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() override;
};