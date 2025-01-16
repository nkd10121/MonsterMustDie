#pragma once
#include "ObjectBase.h"

class HealPortion : public ObjectBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	HealPortion();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~HealPortion();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init()override;
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
	/// 座標を設定
	/// </summary>
	/// <param name="pos">設定座標</param>
	void SetPosition(Vec3 pos)override;

	/// <summary>
	/// 終了させる
	/// </summary>
	void End();

private:
	int m_effectCreateFrame;	//フレーム数

	float m_angle;	//角度
	float m_posOffsetY;	//動き幅
};

