#pragma once
#include "TrapBase.h"
class CutterTrap : public TrapBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	CutterTrap();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~CutterTrap();

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

	std::vector<Vec3> GetAttackPos()override;

private:
	int m_frameIdx;
	int m_attackCount;	//攻撃カウント
};

