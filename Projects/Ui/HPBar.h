#pragma once
#include <vector>
#include "Vec2.h"

/// <summary>
/// HPバーのUIクラス
/// </summary>
class HPBar
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	HPBar();
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~HPBar();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="max">最大HP</param>
	void Init(int max);
	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="hp">現在のHP</param>
	void Update(int hp);
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	int m_maxHP;				//最大HP
	Vec2 m_drawRedGaugePos;		//赤ゲージの描画座標
	Vec2 m_drawYellowGaugePos;	//黄色ゲージの描画座標
	Vec2 m_drawPos;				//描画座標(枠や背景)

	Vec2 m_offset;				//揺れ幅

	int m_hpBarWidth;			//画像の横幅
	int m_hpBarHeight;			//画像の縦幅

	int m_gaugeWidth;			//赤ゲージの幅
	int m_gaugeYellowWidth;		//黄色ゲージの幅

	int m_vibrationCount;		//HPUIを揺らすフレーム数

	std::vector<int> m_handles;	//画像ハンドル
};

