#pragma once
#include "TrapBase.h"
class IronSnareTrap : public TrapBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	IronSnareTrap();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IronSnareTrap();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(Vec3 pos, Vec3 norm)override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;
	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;

	void SetRot(Vec3 vec)override;
	void SetPos(Vec3 vec)override;

	std::vector<Vec3> GetAttackPos()override;
};