#pragma once
#include "ObjectBase.h"
class Crystal : public ObjectBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="hp">体力</param>
	Crystal(int hp);
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Crystal();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="physics">物理クラスポインタ</param>
	void Init()override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;
	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;

	void DrawHP();

	/// <summary>
	/// 生成座標を設定
	/// </summary>
	/// <param name="pos">生成座標</param>
	void Set(const Vec3& pos);

	/// <summary>
	/// ほかのオブジェクトと衝突したときに呼ばれる
	/// </summary>
	/// <param name="colider">当たったオブジェクト</param>
	virtual void OnTriggerEnter(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol)override;

	/// <summary>
	/// 破壊されたかどうかを取得する
	/// </summary>
	/// <returns>破壊フラグ</returns>
	const bool GetIsBreak()const { return m_isBreak; }
	/// <summary>
	/// クリスタルの現在HPを取得する
	/// </summary>
	/// <returns>クリスタルの現在HP</returns>
	const int GetHp()const { return m_hp; }

	/// <summary>
	/// プレイヤーが死亡したら呼ばれる関数
	/// </summary>
	/// <returns></returns>
	const void PlayerDead();

	void CheckCameraRayHit(const Vec3 cameraPos,const Vec3 targetPos);
private:
	Vec3 m_pos;	//座標

	int m_preHp;	//体力
	int m_hp;	//体力

	bool m_isBreak;		//破壊されたかどうか
	
	int m_crystalStandHandle;	//クリスタルスタンドのモデルハンドル

	float m_angle;	//クリスタルを動かすための角度
	int m_effectCreateCount;

	int m_bgHandle;
	float m_textMagPower;
	bool m_isDamaged;

private:
	int m_psHandle;
	int m_vsHandle;
	int m_noizeTexHandle;


	// シェーダーに渡すデータ構造体
	struct UserData
	{
		float time;       // アニメーション用の時間
		bool isNormalDraw;	//半透明にするかどうか
		float dummy[2];   // 16バイトに合わせるためのダミー
	};
	// 自分でシェーダーに渡したい情報をセットできる（コンスタントバッファの設定）
	int cBufferHandle;
	UserData* pUserData;
};

