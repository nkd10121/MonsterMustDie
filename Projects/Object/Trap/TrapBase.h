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
	struct TrapInfo
	{
		std::string name;	//名前
		int atk;			//攻撃力
		float searchRange;	//索敵判定の大きさ
		float atkRange;		//攻撃判定の大きさ
		int coolTime;		//クールタイム
		int cost;			//設置コスト
		Kind kind;			//設置種類

		std::string modelId;	//モデルID
		float modelSize;		//モデルサイズ
		std::string imageId;	//画像ID
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

	void Init(Vec3 pos, Vec3 vec)override {};
	/// <summary>
	/// 更新
	/// </summary>
	void Update()override {};
	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override {};

	virtual std::vector<Vec3> GetAttackPos() { return std::vector<Vec3>(); }

	void UpdateAnim() {};

	/// <summary>
	/// 仮描画
	/// </summary>
	void PreviewDraw();

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

	virtual void SetPos(Vec3 pos);
	/// <summary>
	/// 地震の座標を取得
	/// </summary>
	/// <returns></returns>
	virtual const Vec3 GetPos()const { return rigidbody->GetPos(); }

	virtual void SetRot(Vec3 vec) {};

	/// <summary>
	/// 自身の名前を取得
	/// </summary>
	/// <returns></returns>
	const std::string GetTrapName()const { return m_trapName; }

	const int GetTrapKind()const { return static_cast<int>(m_status.kind); }
	const std::string GetImageId()const { return m_status.imageId; }

	void SetIsPreview() { m_isPreview = true; }

	void OnTriggerEnter(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol)override;

protected:
	TrapInfo m_status;	//ステータス
	bool m_isExist;		//存在フラグ
	bool m_isAttack;	//攻撃フラグ

	std::string m_trapName;	//トラップ名
	
	bool m_isPreview;

private:
	float m_angle;
};

