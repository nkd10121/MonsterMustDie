#pragma once
#include "EnemyBase.h"

/// <summary>
/// 基本的な敵クラス
/// </summary>
class EnemyFast : public EnemyBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	EnemyFast();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~EnemyFast();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="physics">物理クラスポインタ</param>
	virtual void Init()override;
	/// <summary>
	/// 終了
	/// </summary>
	/// <param name="physics">物理クラスポインタ</param>
	virtual void Finalize();
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();
	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// 索敵判定を生成する
	/// </summary>
	void CreateSearchCollision()override;
	/// <summary>
	/// 索敵判定を削除する
	/// </summary>
	void DeleteSearchCollision() override;

	/// <summary>
	/// 3Dモデルの座標更新
	/// </summary>
	virtual void UpdateModelPos()override;

	/// <summary>
	/// 索敵判定の半径を取得
	/// </summary>
	/// <returns></returns>
	virtual const float GetSearchCollisionRadius()const override;

	/// <summary>
	/// 押し出し処理を行わないオブジェクトと衝突したとき
	/// </summary>
	/// <param name="colider">衝突したオブジェクト</param>
	void OnTriggerEnter(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol)override;

	void OnTriggerExit(const std::shared_ptr<Collide>& ownCol, const std::shared_ptr<Collidable>& send, const std::shared_ptr<Collide>& sendCol)override;
};