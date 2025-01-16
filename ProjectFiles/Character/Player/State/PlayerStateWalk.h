#pragma once
#include "StateBase.h"

class Player;

/// <summary>
/// プレイヤーの歩き状態
/// </summary>
class PlayerStateWalk : public StateBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="own"></param>
	PlayerStateWalk(std::shared_ptr<CharacterBase> own);
	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlayerStateWalk()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(std::string id)override;
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() override;

	/// <summary>
	/// 入力方向から歩きアニメーション名を取得する
	/// </summary>
	/// <param name="dir"></param>
	/// <returns></returns>
	std::string GetWalkAnimName(eDir dir);

private:
	eDir m_dir;

	int m_noInputFrame;
};