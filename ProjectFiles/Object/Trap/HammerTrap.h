#pragma once
#include "TrapBase.h"
class HammerTrap : public TrapBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	HammerTrap();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~HammerTrap();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(Vec3 pos);
	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;
	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;
};

