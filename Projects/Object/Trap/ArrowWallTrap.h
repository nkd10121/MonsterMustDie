#pragma once
#include "TrapBase.h"
class ArrowWallTrap : public TrapBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ArrowWallTrap();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~ArrowWallTrap();

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
	int m_attackCount;		//攻撃カウント
	int m_coolTimeCount;	//クールタイム

	int m_frameIdx;		//モデルフレーム番号

	Vec3 m_arrowPos;		//矢モデルの座標	
	Vec3 m_arrowPosInit;	//矢モデルの初期座標

	Vec3 m_norm;		//法線ベクトル
};

