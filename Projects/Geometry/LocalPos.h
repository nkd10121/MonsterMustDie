#pragma once
#include "Vec3.h"

struct LocalPos
{
public:
	Vec3 centerPos;		//基準とする中心座標
	Vec3 localPos;		//中心座標からの相対座標

	Vec3 centerRotVec;		//中心座標の回転ベクトル

public:
	/// <summary>
	/// 引数なしコンストラクタ
	/// </summary>
	LocalPos();

	/// <summary>
	/// ローカル座標をワールド座標にして取得
	/// </summary>
	/// <returns></returns>
	Vec3 GetWorldPos();

	/// <summary>
	/// 正面座標から回転ベクトルをセットする
	/// </summary>
	void SetFrontPos(Vec3 frontPos);
};