#pragma once
#include "ObjectBase.h"

/// <summary>
/// 遠距離攻撃の弾クラス
/// </summary>
class Shot : public ObjectBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="tag">当たり判定タグ</param>
	Shot(GameObjectTag tag);
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Shot();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="physics">物理クラスポインタ</param>
	void Init()override;
	/// <summary>
	/// 生成座標と向きと攻撃力を設定
	/// </summary>
	/// <param name="pos">生成座標</param>
	/// <param name="m_dir">向き</param>
	/// <param name="atk">攻撃力</param>
	void Set(std::string stageId,const Vec3& pos, const Vec3& m_dir, const int& atk);
	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;
	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;

	/// <summary>
	/// 終了通知
	/// 当たったオブジェクトから呼ばれる
	/// </summary>
	void End();

	/// <summary>
	/// 存在フラグを取得する
	/// </summary>
	/// <returns>存在フラグ</returns>
	const bool GetIsExist()const { return m_isExist; }

	/// <summary>
	/// 攻撃力を取得する
	/// </summary>
	/// <returns>攻撃力</returns>
	const int GetAtk()const { return m_atk; }

private:
	Vec3 m_moveDir;		//移動方向

	int m_frameCount;	//フレーム数

	int m_mapHandle;

	int m_atk;		//攻撃力
};