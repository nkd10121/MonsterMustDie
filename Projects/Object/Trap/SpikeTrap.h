#pragma once
#include "TrapBase.h"
class SpikeTrap : public TrapBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SpikeTrap();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~SpikeTrap();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(Vec3 pos,Vec3 norm)override;
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
	int m_attackCount;	//攻撃カウント
	int m_coolTimeCount;	//クールタイム

	int m_frameIdx;		//スパイクのモデルフレーム数

	Vec3 m_spikePos;	//スパイクモデルの座標	
	Vec3 m_spikePosInit;	//スパイクモデルの初期座標

	Vec3 m_norm;
	Vec3 m_movedPos;		//スパイクモデルの移動量
};

