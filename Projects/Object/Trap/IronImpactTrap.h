#pragma once
#include "TrapBase.h"
class IronImpactTrap : public TrapBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	IronImpactTrap();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IronImpactTrap();

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

	const Vec3 GetPos()const override;

	std::vector<Vec3> GetAttackPos()override;

private:
	/// <summary>
	/// アニメーションの更新
	/// </summary>
	/// <param name="attachNo">進行させたいアニメーション番号</param>
	/// <returns>ループしたかどうか</returns>
	bool UpdateAnim(int attachNo, float startTime = 0.0f);

	/*アニメーション関係*/
	int m_currentAnimNo;		//現在のアニメーション
	int m_nowAnimIdx;

	float m_animSpeed;			//アニメーション再生速度
	int m_animEndFrame;

private:
	Vec3 m_norm;
	int m_attackCount;
	int m_waitCount;
};

