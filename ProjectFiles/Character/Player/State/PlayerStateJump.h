#pragma once
#include "StateBase.h"

/// <summary>
/// プレイヤーのジャンプ状態
/// </summary>
class PlayerStateJump : public StateBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="own">持ち主のポインタ</param>
	PlayerStateJump(std::shared_ptr<CharacterBase> own);
	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlayerStateJump()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(std::string id)override;
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() override;

private:
	//状態遷移のためのメンバ関数ポインタ
	using UpdateFunc_t = void (PlayerStateJump::*)();
	UpdateFunc_t m_updateFunc;
	/// <summary>
	/// ジャンプ上昇状態
	/// </summary>
	void UpUpdate();
	/// <summary>
	/// ジャンプ中状態
	/// </summary>
	void LoopUpdate();
	/// <summary>
	/// ジャンプ下降状態
	/// </summary>
	void DownUpdate();

	int m_jumpFrame;	//ジャンプフレーム数
};

