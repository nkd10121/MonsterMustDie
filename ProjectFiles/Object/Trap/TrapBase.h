#pragma once
#include "ObjectBase.h"

class TrapBase : public ObjectBase
{
public:

	//設置場所の種類
	enum Kind
	{
		Floor,	//床
		Wall,	//壁
	};

	//ステータス構造体
	struct Status
	{
		int atk;
		float searchRange;
		float atkRange;
		int coolTime;
		int cost;
		Kind kind;
	};
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	TrapBase();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~TrapBase();

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;
	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;

	/// <summary>
	/// モデルのロードを申請する
	/// </summary>
	void LoadModel()override;

	/// <summary>
	/// 終了させる
	/// </summary>
	void End();

	/// <summary>
	/// 自身の攻撃力を取得
	/// </summary>
	/// <returns>攻撃力</returns>
	const int GetAtk()const { return m_status.atk; }
	/// <summary>
	/// 自身の設置コストを取得
	/// </summary>
	/// <returns></returns>
	const int GetCost()const { return m_status.cost; }

	/// <summary>
	/// 地震の座標を取得
	/// </summary>
	/// <returns></returns>
	const Vec3 GetPos()const { return rigidbody->GetPos(); }

	const std::string GetTrapName()const { return m_trapName; }

	void OnTriggerEnter(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol)override;

protected:
	Status m_status;
	bool m_isExist;		//存在フラグ
	bool m_isAttack;	//攻撃フラグ

	std::string m_trapName;	//トラップ名
};

