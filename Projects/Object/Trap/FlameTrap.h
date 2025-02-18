#pragma once
#include "TrapBase.h"
class FlameTrap : public TrapBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	FlameTrap();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~FlameTrap();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(Vec3 pos, Vec3 direction)override;
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
	/// <summary>
	/// アニメーションの更新
	/// </summary>
	/// <param name="attachNo">進行させたいアニメーション番号</param>
	/// <returns>ループしたかどうか</returns>
	bool UpdateAnim(int attachNo, float startTime = 0.0f);
	/// <summary>
	///	アニメーションの変更
	/// </summary>
	/// <param name="animIndex">変更後のアニメーション番号</param>
	void ChangeAnim(int animIndex, float animSpeed = 0.5f);

	/*アニメーション関係*/
	int m_currentAnimNo;		//現在のアニメーション
	int m_nowAnimIdx;

	float m_animSpeed;			//アニメーション再生速度
	int m_animEndFrame;
private:
	float m_angle;
	Vec3 m_direction;
	int m_waitCount;
	int m_attackCount;
};

